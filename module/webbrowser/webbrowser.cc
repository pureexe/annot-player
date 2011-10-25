// webbrowser.cc
// 7/20/2011

#include "webbrowser.h"
#include "ui_webbrowser.h"
#include <QWebView>
#include <QWebHistory>
#include <QPlastiqueStyle>
#include <QShortcut>
#include <qdebug.h>

// - Constructions -

WebBrowser::WebBrowser(QWidget *parent)
  : Base(parent), ui_(new Form),
    homePage_(WEBBROWSER_HOMEPAGE), searchEngine_(WEBBROWSER_SEARCHENGINE)
{
  ui_->setupUi(this);

  QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);

  connect(ui_->tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));

  // TODO: mvoe newTabButton into UI form
  QToolButton *newTabButton = new QToolButton(this);
  newTabButton->setStyleSheet(
    "QToolButton{border-image: url(:/images/newtab.png); border:2px;}"
    "QToolButton:hover{border-image: url(:/images/newtab_hover.png);}"
    "QToolButton:pressed{border-image: url(:/images/newtab_pressed.png);}"
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
  //newTab();

  updateButtons();

  QShortcut *locationShortcut = new QShortcut(QKeySequence("Ctrl+L"), this);
  QShortcut *fullScreenShortcut = new QShortcut(QKeySequence("F11"), this);
  connect(locationShortcut , SIGNAL(activated()), this , SLOT(location()));
  connect(fullScreenShortcut , SIGNAL(activated()), this , SLOT(toggleFullScreen()));

  // Focus:
  ui_->addressLine->setFocus();
}

WebBrowser::~WebBrowser()
{ delete ui_; }

// - Properties -

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
WebBrowser::openUrl()
{
  if (ui_->tabWidget->count() == 0)
    newTab();

  QWebView *view = qobject_cast<QWebView*>(ui_->tabWidget->currentWidget());
  if (view) {
    QString address = ui_->addressLine->text();
    if (!address.startsWith("http://")) {
      address.prepend("http://");
      ui_->addressLine->setText(address);
    }
    view->setUrl(address);
    ui_->tabWidget->setTabText(ui_->tabWidget->currentIndex(), address);
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
  newTab(homePage());
}

void
WebBrowser::newTab(const QString &url)
{
  QWebView *view = new QWebView(this);
  connect(view, SIGNAL(urlChanged(QUrl)), SLOT(updateAddressbar()));
  connect(view, SIGNAL(loadStarted()), SLOT(handleLoadStarted()));
  connect(view, SIGNAL(loadFinished(bool)), SLOT(handleLoadFinished()));

  ui_->tabWidget->addTab(view, url);
  view->setUrl(url);
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
