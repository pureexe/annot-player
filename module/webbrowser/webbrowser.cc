// webbrowser.cc
// 7/20/2011

#include "webbrowser.h"
#include "webbrowser_p.h"
#include "ui_webbrowser.h" // generated
#include "webkit/wbwebview.h"
#include "network/wbnetworkaccessmanager.h"
#include "network/wbnetworkcookiejar.h"
#include "core/wbsearchenginefactory.h"
#include "global/wbrc.h"
#include "global/wbss.h"
#include "module/mousegesture/mousegesture.h"
#include "module/mousegesture/mousegesturefilter.h"
#include "module/qtext/filesystem.h"
#include "module/qtext/algorithm.h"
#include <QtWebKit/QWebHistory>
#include <QtNetwork/QNetworkDiskCache>
#include <QtGui>
#include <boost/typeof/typeof.hpp>

#define DEBUG "webbrowser"
#include "module/debug/debug.h"

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses
#endif // __GNUC__

#define TEXT_SIZE_SCALE 0.85

#ifdef Q_WS_MAC
#  define SHORTCUT_LOCATION     "CTRL+L"
#else
#  define SHORTCUT_LOCATION     "ALT+D"
#endif // Q_WS_MAC

#ifdef Q_WS_MAC
#  define K_META     "META"
#else
#  define K_META     "CTRL"
#endif // Q_WS_MAC

#define SHORTCUT_SEARCH         "CTRL+E"

enum { MAX_HISTORY = 30 };

enum { StatusMessageTimeout = 10000 }; // 10 seconds

// - RC -

#ifdef Q_OS_LINUX
#  define RC_PREFIX     DOCDIR "/"
#else
#  define RC_PREFIX     QCoreApplication::applicationDirPath() + "/doc/"
#endif // Q_OS_LINUX

#define RC_HTML_START   RC_PREFIX "start.html"

namespace { // anonymous
  inline QByteArray rc_html_start_()
  {
    QFile f(RC_HTML_START);
    return f.open(QIODevice::ReadOnly) ? f.readAll() : QByteArray();
  }
} // anonymous namespace

// - Helpers -

QString
WebBrowser::tidyUrl(const QString &url)
{
  QString ret = url;
  ret.remove(QRegExp("^http://", Qt::CaseInsensitive));
  ret.remove(QRegExp("/$"));
  return ret;
}

int
WebBrowser::searchEngineForAddress(const QString &address) const
{
  QRegExp rx("^(\\S+)\\s");
  if (rx.indexIn(address) >= 0) {
    Q_ASSERT(rx.captureCount() == 1);
    QString a = rx.cap(1);
    DOUT("keyword =" << a);
    foreach (const WbSearchEngine *e, searchEngines_)
      if (e->acronyms().contains(a, Qt::CaseInsensitive))
        return e->id();
  }
  return -1;
}

QString
WebBrowser::completeUrl(const QString &url) const
{
  int i, j;
  QString ret = url.trimmed();
  if (!ret.contains(QRegExp("^\\w+://"))) {
    int se = searchEngineForAddress(ret);
    if (se >= 0 && se < searchEngines_.size()) {
      DOUT("searchEngine =" << se);
      ret.remove(QRegExp("^\\S*\\s"));
      ret = ret.trimmed();
      const_cast<Self *>(this)->addRecentSearch(ret);
      ret = searchAddress(ret, se);
    } else if (ret.contains(QRegExp("^\\w")) &&
        (i=ret.indexOf('/')) >= 0 &&
        ((j=ret.indexOf(' ')) < 0 || j > i))
      ret.prepend("http://");
    else if (!ret.contains(".") && ret.compare("localhost", Qt::CaseInsensitive) ||
             ret.contains(QRegExp(
      "[^"
        "a-zA-Z0-9"
        "\"" "\'" "\\\\" "\\." "\\^" "\\[" "\\]" "\\-"
        "~`!@#$%&*(){}_=+|:;<>/?"
      "]"
      )))
      ret = searchAddress(url, WbSearchEngineFactory::Google);
    else
      ret.prepend("http://");
  }
  return ret;
}

QString
WebBrowser::decodeUrl(const QUrl &url)
{
  QUrl ret = url;
  QString host = ret.host();
  if (host == ANNOT_HOST_IP)
    ret.setHost("annot.me");
  else if (host == ANNOT_DOC_IP)
    ret.setHost("doc.annot.me");
  //else if (host == "niconicohost")
  //  ret.setHost("nicovideo.jp");
  //else if (host.endsWith(".niconicohost")) {
  //  QRegExp rx("(.*)\\.niconicohost");
  //  if (!rx.exactMatch(host)) {
  //    Q_ASSERT(0);
  //    return url.toString();
  //  }
  //  Q_ASSERT(rx.captureCount() == 1);
  //  QString t = rx.cap(1);
  //  ret.setHost(t + ".nicovideo.jp");
  //}
  return ret.toString();
}

QUrl
WebBrowser::encodeUrl(const QString &url)
{
  QUrl ret = url;
  if (!ret.host().compare("annot.me", Qt::CaseInsensitive))
    ret.setHost(ANNOT_HOST_IP);
  else if (!ret.host().compare("doc.annot.me", Qt::CaseInsensitive))
    ret.setHost(ANNOT_DOC_IP);
  return ret;
}

// - Constructions -

WebBrowser::WebBrowser(QWidget *parent)
  : Base(parent), ui_(new Form), nam_(0),
    textSizeMultiplier_(TEXT_SIZE_SCALE)
{
  cookieJar_ = new WbNetworkCookieJar(this);
  hideStatusBarTimer_ = new QTimer(this);

  createGestures();
  createSearchEngines();
  setupUi();
  createActions();
  updateButtons();

  hideStatusBarTimer_->setInterval(StatusMessageTimeout);
  hideStatusBarTimer_->setSingleShot(true);
  connect(hideStatusBarTimer_, SIGNAL(timeout()), statusBar(), SLOT(hide()));
  statusBar()->hide();

  // Focus:
  ui_->addressEdit->setFocus();
}

WebBrowser::~WebBrowser()
{ delete ui_; }

void
WebBrowser::setupUi()
{
  ui_->setupUi(this);

  toolBarWidgets_.append(ui_->forwardButton);
  toolBarWidgets_.append(ui_->backButton);
  toolBarWidgets_.append(ui_->reloadButton);
  toolBarWidgets_.append(ui_->addressEdit);
  toolBarWidgets_.append(ui_->searchEdit);

  // Add newTab corner button
  // TODO: move newTabButton into UI form
  QToolButton *newTabButton = new WbToolButton(this);
  newTabButton->setToolTip(tr("New Tab"));
  newTabButton->setStatusTip(tr("New Tab"));
  newTabButton->setStyleSheet(SS_TOOLBUTTON_NEWTAB);
  ui_->tabWidget->setCornerWidget(newTabButton);
  newTabButton->setFixedSize(QSize(26, 16));
  connect(newTabButton, SIGNAL(clicked()), SLOT(newTabAfterCurrentWithBlankPage()));

  ui_->addressEdit->setDefaultItems(homePages_);

  ui_->searchEdit->setEngines(QtExt::subList(searchEngines_, WbSearchEngineFactory::VisibleEngineCount));

  // Set up connections
  connect(ui_->tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
  connect(ui_->tabWidget, SIGNAL(currentChanged(int)), SLOT(updateAddressbar()));
  connect(ui_->tabWidget, SIGNAL(doubleClicked()), SLOT(newTabAtLastWithBlankPage()));
  connect(ui_->tabWidget, SIGNAL(tabDoubleClicked(int)), SIGNAL(fullScreenRequested()));
  connect(ui_->searchEdit, SIGNAL(textEntered(QString)), SLOT(search(QString)));
  connect(ui_->searchEdit, SIGNAL(engineChanged(int)), SLOT(setSearchEngine(int)));
  connect(ui_->searchEdit, SIGNAL(editTextChanged(QString)), SLOT(invalidateSearch()));
  connect(ui_->addressEdit, SIGNAL(textEntered(QString)), SLOT(openUrl(QString)));
  connect(ui_->addressEdit, SIGNAL(openUrlWithAcPlayerRequested(QString)), SIGNAL(openUrlWithAcPlayerRequested(QString)));
  connect(ui_->addressEdit, SIGNAL(importUrlToAcPlayerRequested(QString)), SIGNAL(importUrlToAcPlayerRequested(QString)));
  connect(ui_->addressEdit, SIGNAL(openUrlWithAcDownloaderRequested(QString)), SIGNAL(openUrlWithAcDownloaderRequested(QString)));
  connect(ui_->addressEdit, SIGNAL(openUrlWithOperatingSystemRequested(QString)), SLOT(openUrlWithOperatingSystem(QString)));
  connect(ui_->backButton, SIGNAL(clicked()), SLOT(back()));
  connect(ui_->forwardButton, SIGNAL(clicked()), SLOT(forward()));
  connect(ui_->reloadButton, SIGNAL(clicked()), SLOT(reload()));

  setTabOrder(ui_->addressEdit, ui_->searchEdit);
}

void
WebBrowser::createSearchEngines()
{
  searchEngine_ = WbSearchEngineFactory::Google;
  for (int engine = 0; engine < WbSearchEngineFactory::EngineCount; engine++)
    searchEngines_.append(WbSearchEngineFactory::globalInstance()->create(engine));
}

void
WebBrowser::createActions()
{
  connect(new QShortcut(QKeySequence(SHORTCUT_LOCATION), this), SIGNAL(activated()), SLOT(focusLocationBar()));

  connect(new QShortcut(QKeySequence(QKeySequence::Find), this), SIGNAL(activated()), SLOT(focusSearchBar()));
  connect(new QShortcut(QKeySequence(SHORTCUT_SEARCH), this), SIGNAL(activated()), SLOT(focusSearchBar()));

  connect(new QShortcut(QKeySequence("F11"), this), SIGNAL(activated()), SLOT(toggleFullScreen()));

  connect(new QShortcut(QKeySequence::AddTab, this), SIGNAL(activated()), SLOT(newTabAfterCurrentWithBlankPage()));

  connect(new QShortcut(QKeySequence("CTRL+W"), this) , SIGNAL(activated()), SLOT(closeTab()));

  connect(new QShortcut(QKeySequence::Quit, this) , SIGNAL(activated()), SLOT(close()));

  connect(new QShortcut(QKeySequence(K_META "+TAB"), this) , SIGNAL(activated()), SLOT(nextTab()));
  connect(new QShortcut(QKeySequence(K_META "+SHIFT+TAB"), this) , SIGNAL(activated()), SLOT(previousTab()));
  connect(new QShortcut(QKeySequence("CTRL+}"), this) , SIGNAL(activated()), SLOT(nextTab()));
  connect(new QShortcut(QKeySequence("CTRL+{"), this) , SIGNAL(activated()), SLOT(previousTab()));

  connect(new QShortcut(QKeySequence("CTRL+SHIFT+T"), this) , SIGNAL(activated()), SLOT(undoCloseTab()));

  connect(new QShortcut(QKeySequence("CTRL+1"), this) , SIGNAL(activated()), SLOT(focusTab0()));
  connect(new QShortcut(QKeySequence("CTRL+2"), this), SIGNAL(activated()), SLOT(focusTab1()));
  connect(new QShortcut(QKeySequence("CTRL+3"), this) , SIGNAL(activated()), SLOT(focusTab2()));
  connect(new QShortcut(QKeySequence("CTRL+4"), this) , SIGNAL(activated()), SLOT(focusTab3()));
  connect(new QShortcut(QKeySequence("CTRL+5"), this) , SIGNAL(activated()), SLOT(focusTab4()));
  connect(new QShortcut(QKeySequence("CTRL+6"), this) , SIGNAL(activated()), SLOT(focusTab5()));
  connect(new QShortcut(QKeySequence("CTRL+7"), this) , SIGNAL(activated()), SLOT(focusTab6()));
  connect(new QShortcut(QKeySequence("CTRL+8"), this) , SIGNAL(activated()), SLOT(focusTab7()));
  connect(new QShortcut(QKeySequence("CTRL+9"), this) , SIGNAL(activated()), SLOT(focusTab8()));
  connect(new QShortcut(QKeySequence("CTRL+0"), this) , SIGNAL(activated()), SLOT(focusLastTab()));
}

void
WebBrowser::createGestures()
{
  Qt::MouseButtons buttons = Qt::MiddleButton;
#ifndef Q_WS_MAC
  buttons |= Qt::RightButton;
#endif // Q_WS_MAC
  mouseGestureFilter_ = new MouseGestureFilter(buttons, Qt::NoModifier, this);
}

void
WebBrowser::addMouseGesture(MouseGesture *g)
{
  Q_ASSERT(g);
  if (!g->parent())
    g->setParent(mouseGestureFilter_);
  mouseGestureFilter_->add(g);
  connect(g, SIGNAL(message(QString)), SLOT(showMessage(QString)));
}

QNetworkAccessManager*
WebBrowser::networkAccessManager()
{
  if (!nam_) {
    WbNetworkAccessManager *nam = new WbNetworkAccessManager(this);
    if (!blockedUrls_.isEmpty())
      nam->setBlockedUrls(blockedUrls_);
    if (cookieJar_)
      nam->setCookieJar(cookieJar_);

    if (!cacheDirectory_.isEmpty()) {
      // See: http://www.qtcentre.org/threads/24354-using-QWebView-cache-with-QNetworkDiskCache
      QNetworkDiskCache *cache = new QNetworkDiskCache(nam);
      cache->setCacheDirectory(cacheDirectory_);
      nam->setCache(cache);
    }

    nam_ = nam;
  }
  return nam_;
}

// - Properties -

void
WebBrowser::setHomePages(const QStringList &urls)
{
  homePages_ = urls;
  ui_->addressEdit->setDefaultItems(homePages_);
}

int
WebBrowser::tabCount() const
{ return ui_->tabWidget->count(); }

int
WebBrowser::tabIndex() const
{ return ui_->tabWidget->currentIndex(); }

QWidget*
WebBrowser::tabWidget(int i) const
{ return i >= 0 && i < tabCount() ? ui_->tabWidget->widget(i) : 0; }

QWidget*
WebBrowser::tabWidget() const
{ return ui_->tabWidget->currentWidget(); }

QUrl
WebBrowser::tabUrl(int index) const
{
  QWidget *w = tabWidget(index);
  if (w) {
    QWebView *view = qobject_cast<QWebView *>(w);
    if (w)
      return view->url();
  }
  return QUrl();
}

QList<QUrl>
WebBrowser::tabUrls() const
{
  QList<QUrl> ret;
  for (int i = 0; i < tabCount(); i++)
    ret.append(tabUrl(i));
  return ret;
}


QString
WebBrowser::tabAddress(int index) const
{
  QUrl url = tabUrl(index);
  if (url.isEmpty())
    return QString();
  else
    return tidyUrl(decodeUrl(url));
}

QStringList
WebBrowser::tabAddresses() const
{
  QStringList ret;
  for (int i = 0; i < tabCount(); i++)
    ret.append(tabAddress(i));
  return ret;
}

// - Actions -

void
WebBrowser::openUrls(const QStringList &urls)
{
  switch (urls.size()) {
  case 0: return;
  case 1:
    newTab();
    openUrl(urls.first());
    return;
  }

  int index = tabCount();
  foreach (QString url, urls) {
    newTab();
    openUrl(url);
  }
  focusTab(index);
}

void
WebBrowser::focusTab(int index)
{
  if (index >= 0 && index < tabCount())
    ui_->tabWidget->setCurrentIndex(index);
}

void
WebBrowser::openUrl(const QString &url, QWebView *view)
{
  DOUT("enter: url =" << url);
  if (tabCount() <= 0)
    newTab();

  if (!view)
    view = qobject_cast<QWebView  *>(tabWidget());
  if (!view) {
    Q_ASSERT(0);
    DOUT("exit: failed to create new tab");
    return;
  }

  QUrl viewUrl = view->url();
  QString viewAddress = viewUrl.toString();

  if (viewAddress == url) {
    DOUT("exit: already opened");
    return;
  }

  QString originalUrl = completeUrl(url);
  if (viewAddress == originalUrl) {
    DOUT("exit: already opened");
    return;
  }

  QUrl realUrl = encodeUrl(originalUrl);
  if (viewUrl == realUrl) {
    DOUT("exit: already opened");
    return;
  }

  addRecentUrl(url);

  BOOST_AUTO(iconDaemon, new daemon_::SetAddressIcon(ui_->addressEdit, url, ui_->tabWidget, view));
  connect(view, SIGNAL(loadStarted()), iconDaemon, SLOT(trigger()));
  connect(view, SIGNAL(loadFinished(bool)), iconDaemon, SLOT(trigger(bool)));
  if (view == tabWidget()) {
    QString address = tidyUrl(url);
    ui_->addressEdit->setEditText(address);
    ui_->addressEdit->lineEdit()->setCursorPosition(0);
    ui_->tabWidget->setTabText(tabIndex(), ::shortenText(url));
  }
  view->load(realUrl);
  DOUT("exit: ok");
}

void
WebBrowser::addRecentUrls(const QStringList &urls)
{
  if (!urls.isEmpty())
    foreach (const QString &url, QtExt::revertList(urls))
      addRecentUrl(url);
}

void
WebBrowser::addRecentUrl(const QString &url)
{
  //DOUT("enter: url =" << url);
  if (url.contains(ANNOT_HOST_IP) ||
      url.contains(ANNOT_DOC_IP)) {
    //DOUT("exit: skip my hosts");
    return;
  }
  QString address = tidyUrl(url);
  QComboBox *edit = ui_->addressEdit;
  int index = edit->findText(address);
  if (index >= 0)
    edit->removeItem(index);
  edit->insertItem(0, address); // FIXME: This will change current item!
  if (edit->count() > MAX_HISTORY)
    edit->removeItem(MAX_HISTORY);
  //DOUT("exit");
}

QStringList
WebBrowser::recentUrls() const
{
  QStringList ret;
  int count = ui_->addressEdit->count();
  if (!count)
    return ret;

  QStringList defvals = ui_->addressEdit->defaultItems();
  for (int i = 0; i < count; i++) {
    QString t = ui_->addressEdit->itemText(i).trimmed();
    if (!t.isEmpty() && !defvals.contains(t))
      ret.append(t);
  }
  return ret;
}

QStringList
WebBrowser::recentSearches() const
{ return ui_->searchEdit->recent(); }

void
WebBrowser::addRecentSearch(const QString &text)
{ return ui_->searchEdit->addRecent(text); }

void
WebBrowser::addRecentSearches(const QStringList &l)
{
  foreach (const QString &t, QtExt::revertList(l))
    addRecentSearch(t);
}

void
WebBrowser::setSearchedText(const QString &t)
{ ui_->searchEdit->setText(t); }

void
WebBrowser::search(const QString &text, int engine)
{
  addRecentSearch(text);
  openUrl(searchAddress(text, engine));
}

void
WebBrowser::searchInNewTab(const QString &text, int engine)
{
  addRecentSearch(text);
  newTabAfterCurrent();
  openUrl(searchAddress(text, engine));
}

QString
WebBrowser::searchAddress(const QString &text, int engine) const
{
  if (engine < 0)
    engine = searchEngine_;
  QString ret = text.trimmed();
  ret = ret.toUtf8();
  //ret = QUrl::toPercentEncoding(ret);
  ret = searchEngines_[engine]->search(ret);
  return ret;
}

void
WebBrowser::closeTab(int tab)
{
  switch (tabCount()) {
  case 0: return;
  case 1: hide(); break;
  }
  QWidget *widget = tabWidget(tab);
  if (widget) {
    QWebView *w = qobject_cast<QWebView *>(widget);
    if (w) {
      QUrl url = w->url();
      if (!url.isEmpty())
        closedUrls_.append(url);
    }
    ui_->tabWidget->removeTab(tab);
    delete widget;
  }
  if (tabCount() <= 0)
    close();
}

void
WebBrowser::openUrlWithOperatingSystem(const QString &url)
{
  QString href = url.trimmed();
  if (!href.isEmpty() && !href.contains("://"))
    href.prepend("http://");
  if (!href.isEmpty()) {
    showMessage(tr("openning") + ": " + href);
    QDesktopServices::openUrl(QUrl(href));
  }
}

void
WebBrowser::openLinkInNewTab()
{
  WbWebView *v = dynamic_cast<WbWebView *>(tabWidget());
  if (v) {
    QString url = v->hoveredLink();
    if (!url.isEmpty())
      newTabInBackground(url);
  }
}

void
WebBrowser::closeTab()
{
  if (tabCount())
    closeTab(tabIndex());
  else
    close();
}

void
WebBrowser::undoCloseTab()
{
  DOUT("enter");
  if (closedUrls_.isEmpty())
    showMessage(tr("no recent closed tabs"));
  else {
    QUrl url = closedUrls_.takeLast();
    newTab(decodeUrl(url));
  }
  DOUT("exit");
}

void
WebBrowser::updateButtons()
{
  QWebView *view = qobject_cast<QWebView *>(tabWidget());
  if (view) {
    ui_->backButton->setEnabled(view->history()->canGoBack());
    ui_->forwardButton->setEnabled(view->history()->canGoForward());
  }
}

void
WebBrowser::back()
{
  QWebView *view = qobject_cast<QWebView *>(tabWidget());
  if (view)
    view->back();
}

void
WebBrowser::forward()
{
  QWebView *view = qobject_cast<QWebView *>(tabWidget());
  if (view)
    view->forward();
}

void
WebBrowser::previousTab()
{
  int n = tabCount();
  if (n) {
    int i = tabIndex();
    if (--i < 0)
      i = n - 1;
    focusTab(i);
  }
}

void
WebBrowser::nextTab()
{
  DOUT("enter");
  int n = tabCount();
  if (n) {
    int i = tabIndex();
    if (++i >= n)
      i = 0;
    focusTab(i);
  }
  DOUT("exit");
}

void
WebBrowser::newTabAfterCurrentWithBlankPage()
{
  newTabAfterCurrent();
  openBlankPage();
}

void
WebBrowser::newTabAtLastWithBlankPage()
{
  newTab();
  openBlankPage();
}

void
WebBrowser::openBlankPage()
{
  if (tabCount() <= 0)
    newTab();
  QWebView *v = qobject_cast<QWebView *>(tabWidget());
  if (v) {
    v->setContent(::rc_html_start_(), "text/html");
    ui_->tabWidget->setTabText(tabIndex(), tr("Start Page"));
    int i = ui_->addressEdit->findText(WB_BLANK_PAGE);
    if (i >= 0)
      ui_->addressEdit->setCurrentIndex(i);
    ui_->addressEdit->setIcon(WBRC_IMAGE_APP);
  }
}

void
WebBrowser::newTab(const QString &url)
{
  newTabAfterCurrent();
  openUrl(url);
}

void
WebBrowser::newTabInBackground(const QString &url)
{
  newTabAfterCurrent(false); // focus = false
  openUrl(url, qobject_cast<WbWebView *>(tabWidget(tabIndex() +1)));
}

void
WebBrowser::newTabAfterCurrent(bool focus)
{ newTab(0, tabIndex() + 1, focus); }

void
WebBrowser::newTab(QWebView *view, int index, bool focus)
{
  if (!view)
    view = new WbWebView(this);
  view->page()->setNetworkAccessManager(networkAccessManager());

  if (textSizeMultiplier_ > 0)
    view->setTextSizeMultiplier(textSizeMultiplier_);

  WbWebView *wbview = qobject_cast<WbWebView *>(view);
  if (wbview) {
    wbview->setSearchEngines(QtExt::subList(searchEngines_, WbSearchEngineFactory::VisibleEngineCount));
    wbview->setSearchEngine(searchEngine_);

    connect(wbview, SIGNAL(message(QString)), SLOT(showMessage(QString)));
    connect(wbview, SIGNAL(errorMessage(QString)), SLOT(error(QString)));
    connect(wbview, SIGNAL(warning(QString)), SLOT(warn(QString)));
    connect(wbview, SIGNAL(notification(QString)), SLOT(notify(QString)));
    connect(wbview, SIGNAL(windowCreated(QWebView*)), SLOT(newTab(QWebView*)));
    connect(wbview, SIGNAL(openLinkRequested(QString)), SLOT(newTabInBackground(QString)));

    connect(wbview, SIGNAL(openUrlWithAcPlayerRequested(QString)), SIGNAL(openUrlWithAcPlayerRequested(QString)));
    connect(wbview, SIGNAL(importUrlToAcPlayerRequested(QString)), SIGNAL(importUrlToAcPlayerRequested(QString)));
    connect(wbview, SIGNAL(openUrlWithAcDownloaderRequested(QString)), SIGNAL(openUrlWithAcDownloaderRequested(QString)));
    connect(wbview, SIGNAL(undoClosedTabRequested()), SLOT(undoCloseTab()));
    connect(wbview, SIGNAL(newWindowRequested()), SIGNAL(newWindowRequested()));
    connect(wbview, SIGNAL(fullScreenRequested()), SIGNAL(fullScreenRequested()));

    connect(wbview, SIGNAL(searchWithEngineRequested(QString,int)), SLOT(searchInNewTab(QString,int)));
    connect(this, SIGNAL(searchEngineChanged(int)), wbview, SLOT(setSearchEngine(int)));
  }

  connect(view, SIGNAL(statusBarMessage(QString)), SLOT(showMessage(QString)));
  connect(view, SIGNAL(urlChanged(QUrl)), SLOT(updateAddressbar()));
  connect(view, SIGNAL(loadStarted()), SLOT(handleLoadStarted()));
  connect(view, SIGNAL(loadFinished(bool)), SLOT(handleLoadFinished()));
  connect(view, SIGNAL(linkClicked(QUrl)), SLOT(addRecentUrl(QUrl)));

  view->installEventFilter(mouseGestureFilter_);

  QString t; //= tr("New Tab");
  if (index < 0 || index >= tabCount())
    index = ui_->tabWidget->addTab(view, t);
  else
    ui_->tabWidget->insertTab(index, view, t);

  BOOST_AUTO(textDaemon, new daemon_::SetTabText(ui_->tabWidget, view));
  connect(view, SIGNAL(titleChanged(QString)), textDaemon, SLOT(trigger(QString)));

  BOOST_AUTO(iconDaemon, new daemon_::SetTabIcon(ui_->tabWidget, view));
  connect(view, SIGNAL(loadStarted()), iconDaemon, SLOT(trigger()));
  connect(view, SIGNAL(loadFinished(bool)), iconDaemon, SLOT(trigger(bool)));

  BOOST_AUTO(searchDaemon, new daemon_::SearchTab(this, ui_->tabWidget, view));
  connect(view, SIGNAL(loadFinished(bool)), searchDaemon, SLOT(trigger(bool)));

  if (focus)
    focusTab(index);
}

void
WebBrowser::reload()
{
  QWebView *view = qobject_cast<QWebView  *>(tabWidget());
  if (view)
    view->reload();
}

void
WebBrowser::updateAddressbar()
{
  QWebView *view = qobject_cast<QWebView *>(tabWidget());
  if (view) {
    WbComboEdit *edit = ui_->addressEdit;
    QUrl url = view->url();
    QString address = tidyUrl(decodeUrl(url));
    int i = edit->findText(address);
    if (i >= 0) {
      //edit->setItemIcon(i, view->icon()); // already set by daemon
      edit->setCurrentIndex(i);
    } else {
      edit->setEditText(address);
      edit->lineEdit()->setCursorPosition(0);
    }
    if (address == WB_BLANK_PAGE)
      edit->setIcon(WBRC_IMAGE_APP);
    else
      edit->clearIcon();
    updateButtons();
  }
}

void
WebBrowser::handleLoadFinished()
{
  QWebView *view = qobject_cast<QWebView *>(tabWidget());
  if (view)
    view->setCursor(QCursor());
}

void
WebBrowser::handleLoadStarted()
{
  QWebView *view = qobject_cast<QWebView *>(tabWidget());
  if (view)
    view->setCursor(QCursor(Qt::BusyCursor));
}

void
WebBrowser::focusLocationBar()
{
  ui_->addressEdit->lineEdit()->selectAll();
  ui_->addressEdit->setFocus();
}

void
WebBrowser::focusSearchBar()
{
  ui_->searchEdit->lineEdit()->selectAll();
  ui_->searchEdit->setFocus();
}

void
WebBrowser::toggleFullScreen()
{
  if (isFullScreen())
    showNormal();
  else
    showFullScreen();
}

// - Events -

void
WebBrowser::closeEvent(QCloseEvent *event)
{
  //removeEventFilter(mouseGestureFilter_);

  while (tabCount())
    closeTab(0);
  Base::closeEvent(event);
}

// - Log -

void
WebBrowser::showMessage(const QString &text)
{
  statusBar()->setStyleSheet(SS_STATUSBAR_MESSAGE);
  statusBar()->showMessage(text);
  if (!text.isEmpty()) {
    statusBar()->show();
    hideStatusBarTimer_->start();
  }
}

void
WebBrowser::error(const QString &text)
{
  statusBar()->setStyleSheet(SS_STATUSBAR_ERROR);
  statusBar()->showMessage(text);
  if (!text.isEmpty()) {
    statusBar()->show();
    hideStatusBarTimer_->start();
  }
}

void
WebBrowser::warn(const QString &text)
{
  statusBar()->setStyleSheet(SS_STATUSBAR_WARNING);
  statusBar()->showMessage(text);
  if (!text.isEmpty()) {
    statusBar()->show();
    hideStatusBarTimer_->start();
  }
}

void
WebBrowser::notify(const QString &text)
{
  statusBar()->setStyleSheet(SS_STATUSBAR_NOTIFY);
  statusBar()->showMessage(text);
  if (!text.isEmpty()) {
    statusBar()->show();
    hideStatusBarTimer_->start();
  }
}

// - Search engine -

void
WebBrowser::setSearchEngine(int engine)
{
  if (searchEngine_ != engine) {
    searchEngine_ = engine;
    ui_->searchEdit->setEngine(searchEngine_);
    emit searchEngineChanged(searchEngine_);
  }
}

// - Hide toolbars -

bool
WebBrowser::isToolBarUnderMouse() const
{
  foreach (const QWidget *w, toolBarWidgets_)
    if (w->underMouse())
      return true;
  return false;
}

bool
WebBrowser::toolBarHasFocus() const
{
  foreach (const QWidget *w, toolBarWidgets_)
    if (w->hasFocus())
      return true;
  return false;
}

void
WebBrowser::setToolBarVisible(bool visible)
{
  foreach (QWidget *w, toolBarWidgets_)
    w->setVisible(visible);
}

bool
WebBrowser::isTabBarUnderMouse() const
{ return ui_->tabWidget->tabBar()->underMouse(); }

bool
WebBrowser::tabBarHasFocus() const
{ return ui_->tabWidget->tabBar()->hasFocus(); }


void
WebBrowser::setTabBarVisible(bool visible)
{ ui_->tabWidget->tabBar()->setVisible(visible); }

// - Search -

void
WebBrowser::invalidateSearch()
{
  enum { min = 2, max = 100 };
  QString t = ui_->searchEdit->currentText().trimmed();
  if (t.size() >= min && t.size() <= max) {
    QWebView *v = qobject_cast<QWebView *>(tabWidget());
    if (v) {
      v->findText(QString(), QWebPage::HighlightAllOccurrences);
      v->findText(t, QWebPage::FindWrapsAroundDocument | QWebPage::HighlightAllOccurrences);
    }
  }
}

// EOF

/*

// - Download -

void
WebBrowser::download(const QNetworkRequest &req)
{
#ifdef WITH_MODULE_DOWNLOAD
  enum { retries = 3 };

  QString path = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
  QString fileName = QFileInfo(req.url().toString()).fileName();
  fileName = QtExt::escapeFileName(fileName);

  path += FILE_PATH_SEP + fileName;
  bool ok = ::dlget(path, req, true, retries, this); // async = true
  if (ok)
    showMessage(path);
  else
    warn(tr("failed to download %1").arg(req.url().toString()));
  QApplication::beep();
#else
  Q_UNUSED(req);
  warn(tr("download is not allowed"));
#endif // WITH_MODULE_DOWNLOAD
}

void
WebBrowser::openUrl()
{
  if (tabCount() == 0)
    newTab();

  QString url = ui_->addressLine->text();
  if (!url.contains("://"))
    url.prepend("http://");

  openUrl(url);

  //QWebView *view = qobject_cast<QWebView *>(ui_->tabWidget->currentWidget());
  //if (view) {
  //  QString address = ui_->addressLine->text();
  //  if (!address.startsWith("http://")) {
  //    address.prepend("http://");
  //    ui_->addressLine->setText(address);
  //  }
  //  ui_->tabWidget->setTabText(ui_->tabWidget->currentIndex(), address);
//
  //  QWebPage *page = view->page();
  //  QNetworkAccessManager *manager = page->networkAccessManager();
  //  connect(manager, SIGNAL(finished(QNetworkReply*)), SLOT(completeNetworkReply(QNetworkReply*)));
//
  //  view->load(address);
//
  //  QString cacheDir = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
  //  if (!cacheDir.isEmpty()) {
  //    // Enable persistent disk cache
  //    // See: http://www.qtcentre.org/threads/24354-using-QWebView-cache-with-QNetworkDiskCache
  //    QNetworkDiskCache *cache = new QNetworkDiskCache(view);
  //    cache->setCacheDirectory(cacheDir);
  //    manager->setCache(cache);
//
  //    //QNetworkRequest req;
  //    //req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
  //    //req.load(address);
  //    //manager->get(req);
  //  }
//
  //  view->load(address);
  //}
}
*/
