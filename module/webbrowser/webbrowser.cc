// webbrowser.cc
// 7/20/2011

#include "webbrowser.h"
#include "webbrowser_p.h"
#include "ui_webbrowser.h" // generated
#include "webkit/wbwebview.h"
#include "network/wbnetworkaccessmanager.h"
#include "network/wbnetworkcookiejar.h"
#include "global/wbrc.h"
#include "global/wbss.h"
#include "module/searchengine/searchenginefactory.h"
#include "module/mousegesture/mousegesture.h"
#include "module/mousegesture/mousegesturefilter.h"
#include "module/qtext/filesystem.h"
#include "module/qtext/algorithm.h"
#include <QtGui>
#include <QtNetwork/QNetworkDiskCache>
#include <QtWebKit/QWebHistory>

#define DEBUG "webbrowser"
#include "module/debug/debug.h"

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses
#endif // __GNUC__

#define TEXT_SIZE_SCALE 0.85

#ifdef Q_OS_MAC
# define SHORTCUT_LOCATION     "CTRL+L"
#else
# define SHORTCUT_LOCATION     "ALT+D"
#endif // Q_OS_MAC

#ifdef Q_OS_MAC
# define K_META     "META"
#else
# define K_META     "CTRL"
#endif // Q_OS_MAC

#define SHORTCUT_SEARCH         "CTRL+E"

enum { MAX_HISTORY = 30 };

enum { StatusMessageTimeout = 10000 }; // 10 seconds

// - RC -

#ifdef Q_OS_LINUX
# define RC_PREFIX     DOCDIR "/"
#else
# define RC_PREFIX     QCoreApplication::applicationDirPath() + "/doc/"
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
    foreach (const SearchEngine *e, searchEngines_)
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
  if (!ret.isEmpty() && !ret.contains(QRegExp("^\\w+://"))) {
    int se = searchEngineForAddress(ret);
    if (se >= 0 && se < searchEngines_.size()) {
      DOUT("searchEngine =" << se);
      ret.remove(QRegExp("^\\S*\\s"));
      ret = ret.trimmed();
      const_cast<Self *>(this)->addRecentSearch(ret);
      ret = searchAddress(ret, se);
    } else if (ret.startsWith('/'))
      ret.prepend("file://");
    else if (ret.contains(QRegExp("^\\w")) &&
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
      ret = searchAddress(url, SearchEngineFactory::GoogleLucky);
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
  else if (host == ANNOT_PROXY_DOMAIN && url.path().startsWith("/watch/"))
    ret.setHost("www.nicovideo.jp");
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
  : Base(parent), nam_(nullptr),
    textSizeMultiplier_(TEXT_SIZE_SCALE), loadProgress_(100)
{
  ui = new Form;

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
  //statusBar()->hide();

  // Focus:
  ui->addressEdit->setFocus();
}

WebBrowser::~WebBrowser()
{ delete ui; }

void
WebBrowser::setupUi()
{
  ui->setupUi(this);

  toolBarWidgets_.append(ui->forwardButton);
  toolBarWidgets_.append(ui->backButton);
  toolBarWidgets_.append(ui->reloadButton);
  toolBarWidgets_.append(ui->addressEdit);
  toolBarWidgets_.append(ui->searchEdit);

  // Add newTab corner button
  // TODO: move newTabButton into UI form
  QToolButton *newTabButton = new WbToolButton(this);
  newTabButton->setToolTip(tr("New Tab"));
  newTabButton->setStatusTip(tr("New Tab"));
  newTabButton->setStyleSheet(SS_TOOLBUTTON_NEWTAB);
  ui->tabWidget->setCornerWidget(newTabButton);
  newTabButton->setFixedSize(QSize(26, 16));
  connect(newTabButton, SIGNAL(clicked()), SLOT(newTabAfterCurrentWithBlankPage()));

  ui->addressEdit->setDefaultItems(homePages_);

  ui->searchEdit->setEngines(QtExt::subList(searchEngines_, SearchEngineFactory::VisibleEngineCount));

  // Set up connections
  connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
  connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(updateAddressbar()));
  connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(updateLoadProgress()));
  connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(updateWindowTitle()));
  connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(updateTabTexts()));
  connect(ui->tabWidget, SIGNAL(doubleClicked()), SLOT(newTabAtLastWithBlankPage()));
  connect(ui->tabWidget, SIGNAL(tabDoubleClicked(int)), SIGNAL(fullScreenRequested()));
#ifdef Q_OS_WIN
  connect(ui->tabWidget, SIGNAL(rightButtonClicked()), SLOT(toggleMenuBarVisible()));
#endif // Q_OS_WIN
  connect(ui->searchEdit, SIGNAL(textEntered(QString)), SLOT(search(QString)));
  connect(ui->searchEdit, SIGNAL(engineChanged(int)), SLOT(setSearchEngine(int)));
  connect(ui->searchEdit, SIGNAL(editTextChanged(QString)), SLOT(invalidateSearch()));
  connect(ui->searchEdit, SIGNAL(searchWithEngineRequested(QString,int)), SLOT(searchInNewTab(QString,int)));
  connect(ui->addressEdit, SIGNAL(textEntered(QString)), SLOT(openUrl(QString)));
  connect(ui->addressEdit, SIGNAL(openUrlWithAcPlayerRequested(QString)), SIGNAL(openUrlWithAcPlayerRequested(QString)));
  connect(ui->addressEdit, SIGNAL(importUrlToAcPlayerRequested(QString)), SIGNAL(importUrlToAcPlayerRequested(QString)));
  connect(ui->addressEdit, SIGNAL(openUrlWithAcDownloaderRequested(QString)), SIGNAL(openUrlWithAcDownloaderRequested(QString)));
  connect(ui->addressEdit, SIGNAL(openUrlWithOperatingSystemRequested(QString)), SLOT(openUrlWithOperatingSystem(QString)));
  connect(ui->addressEdit, SIGNAL(downloadAnnotationUrlRequested(QString)), SIGNAL(downloadAnnotationUrlRequested(QString)));
  connect(ui->backButton, SIGNAL(clicked()), SLOT(back()));
  connect(ui->forwardButton, SIGNAL(clicked()), SLOT(forward()));
  connect(ui->reloadButton, SIGNAL(clicked()), SLOT(reload()));

  setTabOrder(ui->addressEdit, ui->searchEdit);
}

void
WebBrowser::createSearchEngines()
{
  searchEngine_ = SearchEngineFactory::Google;
  for (int engine = 0; engine < SearchEngineFactory::EngineCount; engine++) {
    SearchEngine *e = SearchEngineFactory::globalInstance()->create(engine);
    e->setParent(this);
    searchEngines_.append(e);
  }
}

void
WebBrowser::createActions()
{
  new QShortcut(QKeySequence(SHORTCUT_LOCATION), this, SLOT(focusLocationBar()));

  new QShortcut(QKeySequence(QKeySequence::Find), this, SLOT(focusSearchBar()));
  new QShortcut(QKeySequence(SHORTCUT_SEARCH), this, SLOT(focusSearchBar()));

  new QShortcut(QKeySequence("F11"), this, SLOT(toggleFullScreen()));

  new QShortcut(QKeySequence::AddTab, this, SLOT(newTabAfterCurrentWithBlankPage()));

  new QShortcut(QKeySequence("CTRL+W"), this, SLOT(closeTab()));

  new QShortcut(QKeySequence::Quit, this, SLOT(quit()));

  new QShortcut(QKeySequence(K_META "+TAB"), this, SLOT(nextTab()));
  new QShortcut(QKeySequence(K_META "+SHIFT+TAB"), this, SLOT(previousTab()));
  new QShortcut(QKeySequence("CTRL+}"), this, SLOT(nextTab()));
  new QShortcut(QKeySequence("CTRL+{"), this, SLOT(previousTab()));

  new QShortcut(QKeySequence("CTRL+SHIFT+T"), this, SLOT(undoCloseTab()));

  new QShortcut(QKeySequence("CTRL+1"), this, SLOT(focusTab0()));
  new QShortcut(QKeySequence("CTRL+2"), this, SLOT(focusTab1()));
  new QShortcut(QKeySequence("CTRL+3"), this, SLOT(focusTab2()));
  new QShortcut(QKeySequence("CTRL+4"), this, SLOT(focusTab3()));
  new QShortcut(QKeySequence("CTRL+5"), this, SLOT(focusTab4()));
  new QShortcut(QKeySequence("CTRL+6"), this, SLOT(focusTab5()));
  new QShortcut(QKeySequence("CTRL+7"), this, SLOT(focusTab6()));
  new QShortcut(QKeySequence("CTRL+8"), this, SLOT(focusTab7()));
  new QShortcut(QKeySequence("CTRL+9"), this, SLOT(focusTab8()));
  new QShortcut(QKeySequence("CTRL+0"), this, SLOT(focusLastTab()));
#ifndef Q_OS_MAC
  new QShortcut(QKeySequence("ALT+1"), this, SLOT(focusTab0()));
  new QShortcut(QKeySequence("ALT+2"), this, SLOT(focusTab1()));
  new QShortcut(QKeySequence("ALT+3"), this, SLOT(focusTab2()));
  new QShortcut(QKeySequence("ALT+4"), this, SLOT(focusTab3()));
  new QShortcut(QKeySequence("ALT+5"), this, SLOT(focusTab4()));
  new QShortcut(QKeySequence("ALT+6"), this, SLOT(focusTab5()));
  new QShortcut(QKeySequence("ALT+7"), this, SLOT(focusTab6()));
  new QShortcut(QKeySequence("ALT+8"), this, SLOT(focusTab7()));
  new QShortcut(QKeySequence("ALT+9"), this, SLOT(focusTab8()));
  new QShortcut(QKeySequence("ALT+0"), this, SLOT(focusLastTab()));
#endif // Q_OS_MAC
}

void
WebBrowser::createGestures()
{
  Qt::MouseButtons buttons = Qt::MiddleButton;
#ifndef Q_OS_MAC
  buttons |= Qt::RightButton;
#endif // Q_OS_MAC
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
  ui->addressEdit->setDefaultItems(homePages_);
}

int
WebBrowser::tabCount() const
{ return ui->tabWidget->count(); }

int
WebBrowser::tabIndex() const
{ return ui->tabWidget->currentIndex(); }

QWidget*
WebBrowser::tabWidget(int i) const
{ return i >= 0 && i < tabCount() ? ui->tabWidget->widget(i) : 0; }

QWidget*
WebBrowser::tabWidget() const
{ return ui->tabWidget->currentWidget(); }

QString
WebBrowser::tabTitle(int i) const
{ return i >= 0 && i < tabCount() ? ui->tabWidget->tabToolTip(i) : QString(); }

QUrl
WebBrowser::tabUrl(int index) const
{
  QWidget *w = tabWidget(index);
  if (w) {
    auto view = qobject_cast<QWebView *>(w);
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
  foreach (const QString &url, urls) {
    newTab();
    openUrl(url);
  }
  focusTab(index);
}

void
WebBrowser::focusTab(int index)
{
  if (index >= 0 && index < tabCount())
    ui->tabWidget->setCurrentIndex(index);
}

void
WebBrowser::updateLoadProgress()
{
  int progress = loadProgress_;
  auto v = qobject_cast<WbWebView *>(tabWidget());
  if (v)
    progress = v->progress();
  if (loadProgress_ != progress ) {
    loadProgress_ = progress;
    showLoadProgress();
    ui->addressEdit->setProgress(loadProgress_);
  }
}

void
WebBrowser::showLoadProgress()
{
  QString message;
  switch (loadProgress_) {
  case 0: message = tr("Loading ..."); break;
  case 100: message = tr("Loading complete"); break;
  default:
    message = tr("Loading ...") + QString(" %1/100").arg(QString::number(loadProgress_));
  }
  showMessage(message);
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

  auto iconDaemon = new detail::SetAddressIcon(ui->addressEdit, url, ui->tabWidget, view);
  connect(view, SIGNAL(loadStarted()), iconDaemon, SLOT(trigger()));
  connect(view, SIGNAL(loadFinished(bool)), iconDaemon, SLOT(trigger(bool)));
  if (view == tabWidget()) {
    QString address = tidyUrl(url);
    ui->addressEdit->setEditText(address);
    ui->addressEdit->lineEdit()->setCursorPosition(0);
    ui->tabWidget->setTabText(tabIndex(), detail::shortenText(url));
  }
  view->load(realUrl);
  DOUT("exit: ok");
}

void
WebBrowser::addRecentUrls(const QStringList &urls)
{
  for (auto p = urls.constEnd(); p != urls.constBegin();
       addRecentUrl(*--p));
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
  QComboBox *edit = ui->addressEdit;
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
  int count = ui->addressEdit->count();
  if (!count)
    return ret;

  QStringList defvals = ui->addressEdit->defaultItems();
  for (int i = 0; i < count; i++) {
    QString t = ui->addressEdit->itemText(i).trimmed();
    if (!t.isEmpty() && !defvals.contains(t))
      ret.append(t);
  }
  return ret;
}

QStringList
WebBrowser::recentSearches() const
{ return ui->searchEdit->recent(); }

void
WebBrowser::addRecentSearch(const QString &text)
{ return ui->searchEdit->addRecent(text); }

void
WebBrowser::addRecentSearches(const QStringList &l)
{
  for (auto p = l.constEnd(); p != l.constBegin();
       addRecentSearch(*--p));
}

void
WebBrowser::setSearchedText(const QString &t)
{ ui->searchEdit->setText(t); }

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
    auto w = qobject_cast<QWebView *>(widget);
    if (w) {
      QUrl url = w->url();
      if (!url.isEmpty())
        closedUrls_.append(url);
    }
    ui->tabWidget->removeTab(tab);
    delete widget;
  }
  if (tabCount() <= 0)
    quit();
}

void
WebBrowser::openUrlWithOperatingSystem(const QString &url)
{
  QString href = url.trimmed();
  if (!href.isEmpty() && !href.contains("://"))
    href.prepend("http://");
  if (!href.isEmpty()) {
    showMessage(tr("opening") + ": " + href);
    QDesktopServices::openUrl(QUrl(href));
  }
}

void
WebBrowser::openLinkInNewTab()
{
  auto v = qobject_cast<WbWebView *>(tabWidget());
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
    quit();
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
  auto view = qobject_cast<QWebView *>(tabWidget());
  if (view) {
    ui->backButton->setEnabled(view->history()->canGoBack());
    ui->forwardButton->setEnabled(view->history()->canGoForward());
  }
}

void
WebBrowser::back()
{
  auto view = qobject_cast<QWebView *>(tabWidget());
  if (view)
    view->back();
}

void
WebBrowser::forward()
{
  auto view = qobject_cast<QWebView *>(tabWidget());
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
  auto v = qobject_cast<QWebView *>(tabWidget());
  if (v) {
    v->setContent(::rc_html_start_(), "text/html");
    ui->tabWidget->setTabText(tabIndex(), tr("Start Page"));
    int i = ui->addressEdit->findText(WB_BLANK_PAGE);
    if (i >= 0)
      ui->addressEdit->setCurrentIndex(i);
    ui->addressEdit->setIcon(WBRC_IMAGE_APP);
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

  auto wbview = qobject_cast<WbWebView *>(view);
  if (wbview) {
    wbview->setSearchEngines(QtExt::subList(searchEngines_, SearchEngineFactory::VisibleEngineCount));
    wbview->setSearchEngine(searchEngine_);

    connect(wbview, SIGNAL(message(QString)), SLOT(showMessage(QString)));
    connect(wbview, SIGNAL(errorMessage(QString)), SLOT(showError(QString)));
    connect(wbview, SIGNAL(warning(QString)), SLOT(warn(QString)));
    connect(wbview, SIGNAL(notification(QString)), SLOT(notify(QString)));

    connect(wbview, SIGNAL(selectedTextChanged(QString)), SIGNAL(selectedTextChanged(QString)));
    connect(wbview, SIGNAL(windowCreated(QWebView*)), SLOT(newTab(QWebView*)));
    connect(wbview, SIGNAL(openLinkRequested(QString)), SLOT(newTabInBackground(QString)));

    connect(wbview, SIGNAL(openUrlWithAcPlayerRequested(QString)), SIGNAL(openUrlWithAcPlayerRequested(QString)));
    connect(wbview, SIGNAL(importUrlToAcPlayerRequested(QString)), SIGNAL(importUrlToAcPlayerRequested(QString)));
    connect(wbview, SIGNAL(openUrlWithAcDownloaderRequested(QString)), SIGNAL(openUrlWithAcDownloaderRequested(QString)));
    connect(wbview, SIGNAL(downloadAnnotationUrlRequested(QString)), SIGNAL(downloadAnnotationUrlRequested(QString)));
    connect(wbview, SIGNAL(undoClosedTabRequested()), SLOT(undoCloseTab()));
    connect(wbview, SIGNAL(newWindowRequested()), SIGNAL(newWindowRequested()));
    connect(wbview, SIGNAL(fullScreenRequested()), SIGNAL(fullScreenRequested()));

    connect(wbview, SIGNAL(searchWithEngineRequested(QString,int)), SLOT(searchInNewTab(QString,int)));
#ifdef Q_OS_WIN
    connect(wbview, SIGNAL(menuBarVisibilityChangeRequested(bool)), menuBar(), SLOT(setVisible(bool)));
    connect(wbview, SIGNAL(toggleMenuBarVisibleRequested()), SLOT(toggleMenuBarVisible()));
#endif // Q_OS_WIN
    connect(this, SIGNAL(searchEngineChanged(int)), wbview, SLOT(setSearchEngine(int)));
  }

  connect(view, SIGNAL(loadProgress(int)), SLOT(updateLoadProgress()));
  connect(view, SIGNAL(statusBarMessage(QString)), SLOT(showMessage(QString)));
  connect(view, SIGNAL(urlChanged(QUrl)), SLOT(updateAddressbar()));
  connect(view, SIGNAL(loadStarted()), SLOT(handleLoadStarted()));
  connect(view, SIGNAL(loadFinished(bool)), SLOT(handleLoadFinished()));
  connect(view, SIGNAL(linkClicked(QUrl)), SLOT(addRecentUrl(QUrl)));

  view->installEventFilter(mouseGestureFilter_);

  QString t; //= tr("New Tab");
  if (index < 0 || index >= tabCount())
    index = ui->tabWidget->addTab(view, t);
  else
    ui->tabWidget->insertTab(index, view, t);

  auto textDaemon = new detail::SetTabText(ui->tabWidget, view);
  connect(view, SIGNAL(titleChanged(QString)), textDaemon, SLOT(trigger(QString)));

  auto iconDaemon = new detail::SetTabIcon(ui->tabWidget, view);
  connect(view, SIGNAL(loadStarted()), iconDaemon, SLOT(trigger()));
  connect(view, SIGNAL(loadFinished(bool)), iconDaemon, SLOT(trigger(bool)));

  auto searchDaemon = new detail::SearchTab(this, ui->tabWidget, view);
  connect(view, SIGNAL(loadFinished(bool)), searchDaemon, SLOT(trigger(bool)));

  connect(view, SIGNAL(titleChanged(QString)), SLOT(updateWindowTitle()));

  if (focus)
    focusTab(index);
}

void
WebBrowser::updateWindowTitle()
{
  QString t = tabTitle();
  if (t.isEmpty())
    t = tr("Annot Browser");
  setWindowTitle(t);
}

void
WebBrowser::reload()
{
  auto view = qobject_cast<QWebView  *>(tabWidget());
  if (view)
    view->reload();
}

void
WebBrowser::updateAddressbar()
{
  auto view = qobject_cast<QWebView *>(tabWidget());
  if (view) {
    WbComboEdit *edit = ui->addressEdit;
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
  auto view = qobject_cast<QWebView *>(tabWidget());
  if (view)
    view->setCursor(QCursor());
}

void
WebBrowser::handleLoadStarted()
{
  auto view = qobject_cast<QWebView *>(tabWidget());
  if (view)
    view->setCursor(QCursor(Qt::BusyCursor));
}

void
WebBrowser::focusLocationBar()
{
  ui->addressEdit->lineEdit()->selectAll();
  ui->addressEdit->setFocus();
}

void
WebBrowser::focusSearchBar()
{
  ui->searchEdit->lineEdit()->selectAll();
  ui->searchEdit->setFocus();
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
  DOUT("enter");
  //removeEventFilter(mouseGestureFilter_);
  while (tabCount())
    closeTab(0);
  Base::closeEvent(event);
  DOUT("exit");
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
  //  WbWebView *v = qobjectcast<WbWebView *>(tabWidget());
  //  if (v && v->isLoading())
  //    showLoadProgress(v->progress());
}

void
WebBrowser::showError(const QString &text)
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
    ui->searchEdit->setEngine(searchEngine_);
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
{ return ui->tabWidget->tabBar()->underMouse(); }

bool
WebBrowser::tabBarHasFocus() const
{ return ui->tabWidget->tabBar()->hasFocus(); }


void
WebBrowser::setTabBarVisible(bool visible)
{ ui->tabWidget->tabBar()->setVisible(visible); }

bool
WebBrowser::isTabBarVisible() const
{ return ui->tabWidget->tabBar()->isVisible(); }

void
WebBrowser::toggleMenuBarVisible()
{ menuBar()->setVisible(!menuBar()->isVisible()); }

// - Search -

void
WebBrowser::invalidateSearch()
{
  enum { min = 2, max = 100 };
  QString t = ui->searchEdit->currentText().trimmed();
  if (t.size() >= min && t.size() <= max) {
    auto v = qobject_cast<QWebView *>(tabWidget());
    if (v) {
      v->findText(QString(), QWebPage::HighlightAllOccurrences);
      v->findText(t, QWebPage::FindWrapsAroundDocument | QWebPage::HighlightAllOccurrences);
    }
  }
}

void
WebBrowser::updateTabTexts()
{
  for (int i = 0; i < tabCount(); i++) {
    QString t = ui->tabWidget->tabText(i);
    QString prefix = QString::number(i+1) + ". ";
    if (!t.startsWith(prefix)) {
      t.remove(QRegExp("^\\d+\\. "));
      t.prepend(prefix);
      ui->tabWidget->setTabText(i, t);
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

  path += QDir::separator() + fileName;
  bool ok = ::dlget(path, req, true, retries, this); // async = true
  if (ok)
    showMessage(path);
  else
    warn(tr("failed to download %1").arg(req.url().toString()));
  QApplication::beep();
#else
  Q_UNUSED(req)
  warn(tr("download is not allowed"));
#endif // WITH_MODULE_DOWNLOAD
}

void
WebBrowser::openUrl()
{
  if (tabCount() == 0)
    newTab();

  QString url = ui->addressLine->text();
  if (!url.contains("://"))
    url.prepend("http://");

  openUrl(url);

  //auto view = qobject_cast<QWebView *>(ui->tabWidget->currentWidget());
  //if (view) {
  //  QString address = ui->addressLine->text();
  //  if (!address.startsWith("http://")) {
  //    address.prepend("http://");
  //    ui->addressLine->setText(address);
  //  }
  //  ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), address);
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
