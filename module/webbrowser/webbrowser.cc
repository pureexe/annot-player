// webbrowser.cc
// 7/20/2011

#include "webbrowser.h"
#include "webbrowserprivate.h"
#include "ui_webbrowser.h"
#include <QtGui>
#include <QtWebKit>

// - Helpers -

QString
WebBrowser::tidyUrl(const QString &url)
{
  QString ret = url;
  if (ret.startsWith("http://", Qt::CaseInsensitive))
    ret = ret.mid(7); // strlen("http://") = 8
  if (ret.endsWith('/'))
    ret.chop(1);
  return ret;
}

// - Constructions -

WebBrowser::WebBrowser(QWidget *parent)
  : Base(parent), ui_(new Form),
    homePage_(WEBBROWSER_HOMEPAGE), searchEngine_(WEBBROWSER_SEARCHENGINE),
    textSizeMultiplier_(0)
{
  ui_->setupUi(this);

  QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);

  connect(ui_->tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));

  // TODO: mvoe newTabButton into UI form
  QToolButton *newTabButton = new QToolButton(this);
  newTabButton->setStyleSheet(
    "QToolButton{border-image:url(:/images/newtab.png);border:2px;}"
    "QToolButton:hover{border-image:url(:/images/newtab_hover.png);}"
    "QToolButton:pressed{border-image url(:/images/newtab_pressed.png);}"
  );

  ui_->tabWidget->setCornerWidget(newTabButton, Qt::TopRightCorner);
  newTabButton->setFixedSize(QSize(26, 16));

  connect(newTabButton, SIGNAL(clicked()), SLOT(newTab()));
  connect(ui_->addressLine, SIGNAL(returnPressed()), SLOT(openUrl()));
  connect(ui_->searchEdit, SIGNAL(returnPressed()), SLOT(openSearch()));
  connect(ui_->tabWidget, SIGNAL(currentChanged(int)), SLOT(updateAddressbar()));
  connect(ui_->backButton, SIGNAL(clicked()), SLOT(back()));
  connect(ui_->forwardButton, SIGNAL(clicked()), SLOT(forward()));
  connect(ui_->reloadButton, SIGNAL(clicked()), SLOT(reload()));

  ui_->tabWidget->removeTab(0);
  newTab();

  updateButtons();

  QShortcut *locationShortcut = new QShortcut(QKeySequence("Ctrl+L"), this);
  QShortcut *fullScreenShortcut = new QShortcut(QKeySequence("F11"), this);
  connect(locationShortcut , SIGNAL(activated()), SLOT(location()));
  connect(fullScreenShortcut , SIGNAL(activated()), SLOT(toggleFullScreen()));

  // Focus:
  ui_->addressLine->setFocus();
}

WebBrowser::~WebBrowser()
{ delete ui_; }

// - Properties -

qreal
WebBrowser::textSizeMultiplier() const
{ return textSizeMultiplier_; }

void
WebBrowser::setTextSizeMultiplier(qreal factor)
{ textSizeMultiplier_ = factor; }

int
WebBrowser::tabCount() const
{ return ui_->tabWidget->count(); }

const QString&
WebBrowser::homePage() const
{ return homePage_; }

void
WebBrowser::setHomePage(const QString &url)
{ homePage_ = url; }

const QString&
WebBrowser::searchEngine() const
{ return searchEngine_; }

void
WebBrowser::setSearchEngine(const QString &url)
{ searchEngine_ = url; }

// - Actions -

void
WebBrowser::openUrl(const QString &url)
{
  if (tabCount() == 0)
    newTab();

  QWebView *view = qobject_cast<QWebView*>(ui_->tabWidget->currentWidget());
  if (view) {
    ui_->addressLine->setText(url);
    ui_->tabWidget->setTabText(ui_->tabWidget->currentIndex(), url);

    QWebPage *page = view->page();
    QNetworkAccessManager *manager = page->networkAccessManager();

    view->setUrl(url);

    QString cacheDir = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
    if (!cacheDir.isEmpty()) {
      // Enable persistent disk cache
      // See: http://www.qtcentre.org/threads/24354-using-QWebView-cache-with-QNetworkDiskCache
      QNetworkDiskCache *cache = new QNetworkDiskCache(view);
      cache->setCacheDirectory(cacheDir);
      manager->setCache(cache);

      //QNetworkRequest req;
      //req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
      //req.setUrl(address);
      //manager->get(req);
    }

    view->setUrl(url);
  }
}

void
WebBrowser::openUrl()
{
  if (tabCount() == 0)
    newTab();

  QWebView *view = qobject_cast<QWebView*>(ui_->tabWidget->currentWidget());
  if (view) {
    QString address = ui_->addressLine->text();
    if (!address.startsWith("http://")) {
      address.prepend("http://");
      ui_->addressLine->setText(address);
    }
    ui_->tabWidget->setTabText(ui_->tabWidget->currentIndex(), address);

    QWebPage *page = view->page();
    QNetworkAccessManager *manager = page->networkAccessManager();
    connect(manager, SIGNAL(finished(QNetworkReply*)), SLOT(completeNetworkReply(QNetworkReply*)));

    view->setUrl(address);

    QString cacheDir = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
    if (!cacheDir.isEmpty()) {
      // Enable persistent disk cache
      // See: http://www.qtcentre.org/threads/24354-using-QWebView-cache-with-QNetworkDiskCache
      QNetworkDiskCache *cache = new QNetworkDiskCache(view);
      cache->setCacheDirectory(cacheDir);
      manager->setCache(cache);

      //QNetworkRequest req;
      //req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
      //req.setUrl(address);
      //manager->get(req);
    }

    view->setUrl(address);
  }
}

void
WebBrowser::openSearch()
{
  if (ui_->tabWidget->count() == 0)
    newTab();

  QWebView *view = qobject_cast<QWebView*>(ui_->tabWidget->currentWidget());
  if (view) {
    // TODO: Use QUrl to eliminate http here
    QString address = searchEngine()+ QUrl::toPercentEncoding(ui_->searchEdit->text());
    ui_->tabWidget->setTabText(ui_->tabWidget->currentIndex(), address);
    view->setUrl("http://" + address);
  }
}

void
WebBrowser::closeTab(int tab)
{
  QWidget *widget = ui_->tabWidget->widget(tab);
  if (widget) {
    ui_->tabWidget->removeTab(ui_->tabWidget->indexOf(widget));
    delete widget;
  }
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

  view->setUrl(QString());
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
    QString address = view->url().toString();
    address = tidyUrl(address);
    ui_->addressLine->setText(address);
    ui_->tabWidget->setTabText(ui_->tabWidget->currentIndex(), address);
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
WebBrowser::location()
{
  ui_->addressLine->selectAll();
  ui_->addressLine->setFocus();
}

void
WebBrowser::toggleFullScreen()
{
  if (isFullScreen())
    showNormal();
  else
    showFullScreen();
}

// EOF
