// tabwidget.cc
// 7/9/2011
#include "maintabwidget.h"
#include <QtGui>
#include <QtNetwork>
#include <QtWebKit>

#define URL_STARTPAGE  "http://annot.me/"

#ifdef Q_OS_MAC
  #define PATH_TO_IMAGE ":/image/mac"
#else
  #define PATH_TO_IMAGE ":/image/win"
#endif

#define ICON_SIZE       QSize(16, 16)
#define ICON_ADDTAB     QIcon(PATH_TO_IMAGE "/addtab.png")
#define ICON_CLOSETAB   QIcon(PATH_TO_IMAGE "/closetab.png")
#define ICON_CLOSEALL   QIcon(PATH_TO_IMAGE "/closetab.png")

#define STYLESHEET_TOPCORNER "padding:0px;"
          //"border-width:0px;"
          //"min-width:0em;"
          //"border-radius:1px;"
          //"background-color:purple;"
          //"border-style:outset;"
          //"border-color:beige;"
          //"font: bold14px;"

MainTabWidget::MainTabWidget(QWidget *parent)
  : Base(parent)
{
  //globalActionList.clear();
  //staticCentralWidget = this;
  //setTabShape(QTabWidget::Rounded);

#ifdef Q_OS_MAC
  //setDocumentMode(true);
#endif
  setTabsClosable(true);
  connect(this, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
  setMovable(true);

  createStartPage();
  createToolButtons();
}

void
MainTabWidget::createToolButtons()
{
  { // Top left corner: TODO: open, detach, close

    QToolButton *newTabButton = new QToolButton(this);
    newTabButton->setAutoRaise(true);
    newTabButton->setToolTip(tr("Open file in new tab"));
    newTabButton->setIcon(ICON_ADDTAB);
    connect(newTabButton, SIGNAL(clicked()), SLOT(newTab()));
    setCornerWidget(newTabButton, Qt::TopLeftCorner);

    /*
    QToolBar *left = new QToolBar(this);
    left->setStyleSheet(STYLESHEET_TOPCORNER);

    QToolButton *closeTabButton = new QToolButton(this);
    closeTabButton->setAutoRaise(true);
    closeTabButton->setToolTip(tr("Close current tab"));
    closeTabButton->setIcon(ICON_CLOSETAB);
    connect(closeTabButton, SIGNAL(clicked()), SLOT(closeTab()));
    left->addWidget(closeTabButton);

    setCornerWidget(left, Qt::TopLeftCorner);
    */
  }

  { // Top right corner: TODO: minimize, maximize, close
    QToolButton *closeAllButton = new QToolButton(this);
    closeAllButton->setAutoRaise(true);
    closeAllButton->setToolTip(tr("Close all tabs"));
    closeAllButton->setIcon(ICON_CLOSEALL);

    setCornerWidget(closeAllButton, Qt::TopRightCorner);
    connect(closeAllButton, SIGNAL(clicked()), SLOT(closeAll()));
  }
}

void
MainTabWidget::createStartPage()
{
  // TODO: move a new StartPageWidget
  // See fancybrowser example.
  QNetworkProxyFactory::setUseSystemConfiguration(true);
  QWebView *startPage = new QWebView(this);
  QUrl url(URL_STARTPAGE);
  addTab(startPage, tr("Start Page"));
  startPage->load(url);
}

void
MainTabWidget::closeTab(int index)
{
  if (index >= 0 && index < count()) {
    if (count() == 1)
      closeAll();
    else {
      QWidget *tab = widget(index);
      removeTab(index);
      if (tab)
        QTimer::singleShot(0, tab, SLOT(deleteLater()));
    }
  }
}

// EOF
