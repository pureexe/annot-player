// webbrowser.cc // 7/20/2011

#include "webbrowser.h"
#include "ui_webbrowser.h" // generated
#include "webbrowserprivate.h"
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
  cookieJar_ = new WBNetworkCookieJar(this);

  setupUi();
  createActions();
  updateButtons();

  // Focus:
  ui_->addressLine->setFocus();
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

  setTabOrder(ui_->addressLine, ui_->searchEdit);

  // Set up connections
  connect(newTabButton, SIGNAL(clicked()), SLOT(newTab()));
  connect(ui_->tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
  connect(ui_->tabWidget, SIGNAL(currentChanged(int)), SLOT(updateAddressbar()));
  connect(ui_->addressLine, SIGNAL(returnPressed()), SLOT(openUrl()));
  connect(ui_->searchEdit, SIGNAL(returnPressed()), SLOT(openSearch()));
  connect(ui_->backButton, SIGNAL(clicked()), SLOT(back()));
  connect(ui_->forwardButton, SIGNAL(clicked()), SLOT(forward()));
  connect(ui_->reloadButton, SIGNAL(clicked()), SLOT(reload()));

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
  QNetworkAccessManager *nam = new WBNetworkAccessManager(this);
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

  //connect(page->action(QWebPage::OpenLinkInNewWindow), SIGNAL(triggered()), SLOT(
}

// - Properties -

int
WebBrowser::tabCount() const
{ return ui_->tabWidget->count(); }

// - Actions -

void
WebBrowser::openUrls(const QStringList &urls)
{
  switch (urls.size()) {
  case 0: return;
  case 1: openUrl(urls.first()); return;
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
    ui_->addressLine->setText(tidyUrl(url));
    ui_->tabWidget->setTabText(ui_->tabWidget->currentIndex(), url);

    QWebPage *page = view->page();
    setupWebPage(page);
    view->setUrl(encodeUrl(url));
  }
}

void
WebBrowser::openUrl()
{
  QString url = ui_->addressLine->text();
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
    QString address = searchEngine_ + QUrl::toPercentEncoding(ui_->searchEdit->text());
    ui_->tabWidget->setTabText(ui_->tabWidget->currentIndex(), address);
    view->setUrl(completeUrl(address));
  }
}

void
WebBrowser::closeTab(int tab)
{
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
WebBrowser::newTab()
{
  QWebView *view = new QWebView(this); {
    if (textSizeMultiplier_)
      view->setTextSizeMultiplier(textSizeMultiplier_);
  }

  connect(view, SIGNAL(urlChanged(QUrl)), SLOT(updateAddressbar()));
  connect(view, SIGNAL(loadStarted()), SLOT(handleLoadStarted()));
  connect(view, SIGNAL(loadFinished(bool)), SLOT(handleLoadFinished()));

  int index = ui_->tabWidget->addTab(view, tr("New tab"));
  connect(view, SIGNAL(titleChanged(QString)),
          new slot_::SetTabText(ui_->tabWidget, index), SLOT(setTabText(QString)));

  ui_->tabWidget->setCurrentIndex(index);
}

void
WebBrowser::reload()
{
  QWebView *view = qobject_cast<QWebView*>(ui_->tabWidget->currentWidget());
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
    ui_->addressLine->setText(address);
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
  ui_->addressLine->selectAll();
  ui_->addressLine->setFocus();
}

void
WebBrowser::focusSearchBar()
{
  ui_->searchEdit->selectAll();
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
