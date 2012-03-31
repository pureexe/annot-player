// webbrowser.cc // 7/20/2011

#include "webbrowser.h"
#include "ui_webbrowser.h" // generated
#include "webbrowserprivate.h"
#include "module/download/download.h"
#include "module/qtext/filesystem.h"
#include <QtGui>
#include <QtWebKit>

#define DEBUG "webbrowser"
#include "module/debug/debug.h"

#ifdef Q_OS_MAC
  #define SHORTCUT_LOCATION     "CTRL+L"
#else
  #define SHORTCUT_LOCATION     "ALT+D"
#endif // Q_WS_MAC

#define SHORTCUT_SEARCH         "CTRL+E"

enum { MAX_HISTORY = 30 };

#ifdef Q_WS_WIN
#  define PATH_SEP      "\\"
#else
#  define PATH_SEP      "/"
#endif // Q_WS_WIN

#define SS_STATUSBAR_(_color) \
  "QStatusBar" "{" \
    "color:" #_color ";" \
  "}"
#define SS_STATUSBAR_MESSAGE    SS_STATUSBAR_(cyan)
#define SS_STATUSBAR_WARNING    SS_STATUSBAR_(orange)
#define SS_STATUSBAR_ERROR      SS_STATUSBAR_(red)

enum { StatusMessageTimeout = 10000 }; // 10 seconds

// - Helpers -

QString
WebBrowser::tidyUrl(const QString &url)
{
  QString ret = url;
  ret.remove(QRegExp("^http://", Qt::CaseInsensitive));
  ret.remove(QRegExp("/$"));
  return ret;
}

QString
WebBrowser::completeUrl(const QString &url)
{
  QString ret = url;
  if (!ret.contains("://"))
    ret.prepend("http://");
  return ret;
}

QString
WebBrowser::decodeUrl(const QUrl &url)
{
  QUrl ret = url;
  QString host = ret.host();
  if (!host.compare("annotcloud.com", Qt::CaseInsensitive))
    ret.setHost("annot.me");
  else if (host == "niconicohost")
    ret.setHost("nicovideo.jp");
  else if (host.endsWith(".niconicohost")) {
    QRegExp rx("(.*)\\.niconicohost");
    if (!rx.exactMatch(host)) {
      Q_ASSERT(0);
      return url.toString();
    }
    Q_ASSERT(rx.captureCount() == 1);
    QString t = rx.cap(1);
    ret.setHost(t + ".nicovideo.jp");
  }
  return ret.toString();
}

QUrl
WebBrowser::encodeUrl(const QString &url)
{
  QUrl ret = url;
  if (!ret.host().compare("annot.me", Qt::CaseInsensitive))
    ret.setHost("annotcloud.com");
  return ret;
}

// - Constructions -

WebBrowser::WebBrowser(QWidget *parent)
  : Base(parent), ui_(new Form),
    homePage_(WEBBROWSER_HOMEPAGE), searchEngine_(WEBBROWSER_SEARCHENGINE),
    textSizeMultiplier_(0)
{
  cookieJar_ = new WbNetworkCookieJar(this);
  hideStatusBarTimer_ = new QTimer(this);

  setupUi();
  createActions();
  updateButtons();

  addHistory(QStringList()
    << "http://google.com/"
    << "http://nicovideo.jp/"
    << "http://akabeesoft2.com/"
    << "http://syangrila.com/"
    << "http://www.light.gr.jp/"
    << "http://erogamescape.dyndns.org/~ap2/ero/toukei_kaiseki/"
  );

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

  // Add newTab corner button
  // TODO: move newTabButton into UI form
  QToolButton *newTabButton = new QToolButton(this);
  newTabButton->setStyleSheet(
    "QToolButton{border-image:url(:/images/newtab.png);border:2px;}"
    "QToolButton:hover{border-image:url(:/images/newtab_hover.png);}"
    "QToolButton:pressed{border-image url(:/images/newtab_pressed.png);}"
  );
  ui_->tabWidget->setCornerWidget(newTabButton, Qt::TopRightCorner);
  newTabButton->setFixedSize(QSize(26, 16));
  connect(newTabButton, SIGNAL(clicked()), SLOT(newTabWithDefaultPage()));

  setTabOrder(ui_->addressEdit, ui_->searchEdit);

  // Set up connections
  connect(ui_->tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
  connect(ui_->tabWidget, SIGNAL(currentChanged(int)), SLOT(updateAddressbar()));
  connect(ui_->addressEdit, SIGNAL(activated(int)), SLOT(openUrl()));
  connect(ui_->addressEdit->lineEdit(), SIGNAL(returnPressed()), SLOT(openUrl()));
  connect(ui_->searchEdit->lineEdit(), SIGNAL(returnPressed()), SLOT(openSearch()));
  connect(ui_->backButton, SIGNAL(clicked()), SLOT(back()));
  connect(ui_->forwardButton, SIGNAL(clicked()), SLOT(forward()));
  connect(ui_->reloadButton, SIGNAL(clicked()), SLOT(reload()));

  ui_->searchEdit->setEditText("Google");
  ui_->searchEdit->setToolTip("Google");
  ui_->searchEdit->setMinimumWidth(150);

  // Remove default homepage
  ui_->tabWidget->removeTab(0);
  //newTab();
}

void
WebBrowser::createActions()
{
  QShortcut *cl = new QShortcut(QKeySequence(SHORTCUT_LOCATION), this);
  connect(cl , SIGNAL(activated()), SLOT(focusLocationBar()));

  QShortcut *ce = new QShortcut(QKeySequence(SHORTCUT_SEARCH), this);
  connect(ce , SIGNAL(activated()), SLOT(focusSearchBar()));

  QShortcut *f11 = new QShortcut(QKeySequence("F11"), this);
  connect(f11 , SIGNAL(activated()), SLOT(toggleFullScreen()));

  QShortcut *t = new QShortcut(QKeySequence::AddTab, this);
  connect(t , SIGNAL(activated()), SLOT(newTab()));

  QShortcut *w = new QShortcut(QKeySequence::Close, this);
  connect(w , SIGNAL(activated()), SLOT(closeTab()));

  QShortcut *q = new QShortcut(QKeySequence::Quit, this);
  connect(q , SIGNAL(activated()), SLOT(close()));
}

QNetworkAccessManager*
WebBrowser::makeNetworkAccessManager()
{
  QNetworkAccessManager *nam = new WbNetworkAccessManager(this);
  if (cookieJar_) {
    QObject *parent = cookieJar_->parent();
    nam->setCookieJar(cookieJar_);
    cookieJar_->setParent(parent);
  }
  return nam;

  //QString cacheDir = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
  //if (!cacheDir.isEmpty()) {
  //  // Enable persistent disk cache
  //  // See: http://www.qtcentre.org/threads/24354-using-QWebView-cache-with-QNetworkDiskCache
  //  QNetworkDiskCache *cache = new QNetworkDiskCache(nam);
  //  cache->setCacheDirectory(cacheDir);
  //  nam->setCache(cache);
//
  //  //QNetworkRequest req;
  //  //req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
  //  //req.setUrl(address);
  //  //nam->get(req);
  //}
}

void
WebBrowser::setupWebPage(QWebPage *page)
{
  Q_ASSERT(page);
  QNetworkAccessManager *nam = makeNetworkAccessManager();
  nam->setParent(page);
  page->setNetworkAccessManager(nam);

  connect(page, SIGNAL(downloadRequested(QNetworkRequest)), SLOT(download(QNetworkRequest)));
  //connect(page->action(QWebPage::OpenLinkInNewWindow), SIGNAL(triggered()), SLOT(
}

// - Properties -

int
WebBrowser::tabCount() const
{ return ui_->tabWidget->count(); }

QUrl
WebBrowser::tabUrl(int index) const
{
  QWidget *w = ui_->tabWidget->widget(index);
  if (w) {
    QWebView *view = qobject_cast<QWebView*>(w);
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
  case 1: newTab(); openUrl(urls.first()); return;
  }

  int index = tabCount();
  foreach (QString url, urls) {
    newTab();
    openUrl(url);
  }
  if (index < tabCount())
    ui_->tabWidget->setCurrentIndex(index);
}

void
WebBrowser::openUrl(const QString &url)
{
  if (tabCount() <= 0)
    newTab();
  QWebView *view = qobject_cast<QWebView *>(ui_->tabWidget->currentWidget());
  if (view) {
    QUrl realUrl = encodeUrl(completeUrl(url));
    if (view->url() != realUrl) {
      addHistory(url);
      ui_->addressEdit->setEditText(tidyUrl(url));
      ui_->tabWidget->setTabText(ui_->tabWidget->currentIndex(), url);
      view->setUrl(realUrl);
    }
  }
}

void
WebBrowser::addHistory(const QStringList &urls)
{
  foreach (const QString &url, urls)
    addHistory(url);
}

void
WebBrowser::addHistory(const QString &url)
{
  QString address = tidyUrl(url);
  QComboBox *edit = ui_->addressEdit;
  int index = edit->findText(address);
  if (index >= 0)
    edit->removeItem(index);
  edit->insertItem(0, address);
  if (edit->count() > MAX_HISTORY)
    edit->removeItem(MAX_HISTORY);
}

void
WebBrowser::openUrl()
{
  QString url = ui_->addressEdit->currentText();
  url = completeUrl(url);

  openUrl(url);
}

void
WebBrowser::openSearch()
{
  if (tabCount() <= 0)
    newTab();

  QWebView *view = qobject_cast<QWebView*>(ui_->tabWidget->currentWidget());
  if (view) {
    QString text = ui_->searchEdit->currentText();
    ui_->searchEdit->insertItem(0, text);
    QString address = searchEngine_ + QUrl::toPercentEncoding(text);
    ui_->tabWidget->setTabText(ui_->tabWidget->currentIndex(), address);
    view->setUrl(completeUrl(address));
  }
}

void
WebBrowser::closeTab(int tab)
{
  switch (tabCount()) {
  case 0: return;
  case 1: hide(); break;
  }
  QWidget *widget = ui_->tabWidget->widget(tab);
  if (widget) {
    ui_->tabWidget->removeTab(tab);
    delete widget;
  }
  if (tabCount() <= 0)
    close();
}

void
WebBrowser::closeTab()
{
  switch (tabCount()) {
  case 0: return;
  case 1: hide(); break;
  }
  QWidget *widget = ui_->tabWidget->currentWidget();
  if (widget) {
    ui_->tabWidget->removeTab(ui_->tabWidget->currentIndex());
    delete widget;
  }
  if (tabCount() <= 0)
    close();
}

void
WebBrowser::updateButtons()
{
  QWebView *view = qobject_cast<QWebView*>(ui_->tabWidget->currentWidget());
  if (view) {
    ui_->backButton->setEnabled(view->history()->canGoBack());
    ui_->forwardButton->setEnabled(view->history()->canGoForward());
  }
}

void
WebBrowser::back()
{
  QWebView *view = qobject_cast<QWebView*>(ui_->tabWidget->currentWidget());
  if (view)
    view->back();
}

void
WebBrowser::forward()
{
  QWebView *view = qobject_cast<QWebView*>(ui_->tabWidget->currentWidget());
  if (view)
    view->forward();
}

void
WebBrowser::newTabWithDefaultPage()
{
  newTab();
  openUrl(homePage());
}

void
WebBrowser::newTab(QWebView *view)
{
  if (!view)
    view = new WbWebView(this);
  setupWebPage(view->page());

  if (textSizeMultiplier_ > 0)
    view->setTextSizeMultiplier(textSizeMultiplier_);

  WbWebView *wbview = qobject_cast<WbWebView *>(view);
  if (wbview)
    connect(wbview, SIGNAL(windowCreated(QWebView*)), SLOT(newTab(QWebView*)));

  connect(view, SIGNAL(urlChanged(QUrl)), SLOT(updateAddressbar()));
  connect(view, SIGNAL(loadStarted()), SLOT(handleLoadStarted()));
  connect(view, SIGNAL(loadFinished(bool)), SLOT(handleLoadFinished()));

  int index = ui_->tabWidget->addTab(view, tr("New tab"));
  connect(view, SIGNAL(titleChanged(QString)),
          new slot_::SetTabText(ui_->tabWidget, index, view), SLOT(setTabText(QString)));

  ui_->tabWidget->setCurrentIndex(index);
}

void
WebBrowser::reload()
{
  QWebView *view = qobject_cast<QWebView *>(ui_->tabWidget->currentWidget());
  if (view)
    view->reload();
}

void
WebBrowser::updateAddressbar()
{
  QWebView *view = qobject_cast<QWebView*>(ui_->tabWidget->currentWidget());
  if (view) {
    QUrl url = view->url();
    QString address = tidyUrl(decodeUrl(url));
    ui_->addressEdit->setEditText(address);
    //ui_->tabWidget->setTabText(ui_->tabWidget->currentIndex(), address);
    updateButtons();
  }
}

void
WebBrowser::handleLoadFinished()
{
  QWebView *view = qobject_cast<QWebView*>(ui_->tabWidget->currentWidget());
  if (view)
    view->setCursor(QCursor());
}

void
WebBrowser::handleLoadStarted()
{
  QWebView *view = qobject_cast<QWebView*>(ui_->tabWidget->currentWidget());
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
  while (tabCount())
    closeTab(0);
  Base::closeEvent(event);
}

// - Download -

void
WebBrowser::download(const QNetworkRequest &req)
{
  enum { retries = 3 };

  QString path = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
  QString fileName = QFileInfo(req.url().toString()).fileName();
  fileName = QtExt::escapeFileName(fileName);

  path += PATH_SEP + fileName;
  bool ok = ::dlget(path, req, true, retries, this); // async = true
  if (ok)
    showMessage(tr("saving to %1 ...").arg(path));
  else
    warn(tr("failed to download %1").arg(req.url().toString()));
}

// - Log -

void
WebBrowser::showMessage(const QString &text)
{
  statusBar()->setStyleSheet(SS_STATUSBAR_MESSAGE);
  statusBar()->showMessage(text);
  statusBar()->show();
  hideStatusBarTimer_->start();
}

void
WebBrowser::error(const QString &text)
{
  statusBar()->setStyleSheet(SS_STATUSBAR_ERROR);
  statusBar()->showMessage(text);
  statusBar()->show();
  hideStatusBarTimer_->start();
}

void
WebBrowser::warn(const QString &text)
{
  statusBar()->setStyleSheet(SS_STATUSBAR_WARNING);
  statusBar()->showMessage(text);
  statusBar()->show();
  hideStatusBarTimer_->start();
}

// EOF

/*
void
WebBrowser::openUrl()
{
  if (tabCount() == 0)
    newTab();

  QString url = ui_->addressLine->text();
  if (!url.contains("://"))
    url.prepend("http://");

  openUrl(url);

  //QWebView *view = qobject_cast<QWebView*>(ui_->tabWidget->currentWidget());
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
  //  view->setUrl(address);
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
  //    //req.setUrl(address);
  //    //manager->get(req);
  //  }
//
  //  view->setUrl(address);
  //}
}
*/
