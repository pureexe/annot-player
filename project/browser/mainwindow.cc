// mainwindow.cc
// 3/14/2012
#include "mainwindow.h"
#include "mainwindow_p.h"
#include "settings.h"
#include "rc.h"
#include "global.h"
#include "application.h"
#include "preferences.h"
#include "project/common/acabout.h"
#include "project/common/acsettings.h"
#include "project/common/acui.h"
#include "project/common/acbrowser.h"
#include "project/common/acplayer.h"
#include "project/common/acdownloader.h"
#include "project/common/acconsole.h"
#include "module/webbrowser/network/wbnetworkaccessmanager.h"
#include "module/animation/fadeanimation.h"
#include "module/searchengine/searchenginefactory.h"
#include "module/nicoutil/nicoutil.h"
#include "module/download/downloader.h"
#include "module/mousegesture/mousegesture.h"
#include "module/qtext/algorithm.h"
#include "module/qtext/networkcookie.h"
#include "module/qtext/webview.h"
#ifdef WITH_MODULE_MAGNIFIER
#  include "module/magnifier/magnifier.h"
#endif // WITH_MODULE_MAGNIFIER
#ifdef Q_WS_WIN
#  include "win/qtwin/qtwin.h"
#endif // Q_WS_WIN
#include <boost/tuple/tuple.hpp>
#include <QtGui>
#include <QtWebKit>

#define DEBUG "mainwindow"
#include "module/debug/debug.h"

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses
#endif // __GNUC__

#define HOMEPAGE_EN    "http://www.youtube.com/FUNimation"
#define HOMEPAGE_JP    "http://ch.nicovideo.jp/menu/anime"
#define HOMEPAGE_ZH    "http://www.bilibili.tv/video/bangumi.html"

#define MIN_SIZE        QSize(400, 300)
#ifdef Q_WS_WIN
#  define DEFAULT_SIZE  QSize(1000 + 9*2, 650)
#else
#  define DEFAULT_SIZE  QSize(1000 + 9*2, 710) // width for nicovideo.jp + margin*2, height for newtab on mac
#endif // Q_WS_WIN

// - Construction -

#define WINDOW_FLAGS ( \
  Qt::Window | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint )

#ifdef Q_WS_WIN
#  define TEXT_SIZE_SCALE 0.9
#else
#  define TEXT_SIZE_SCALE 1.0
#endif // Q_WS_MAC

#ifdef Q_WS_MAC
#  define USE_MDI
#endif // Q_WS_MAC

MainWindow::MainWindow(QWidget *parent)
  : Base(parent), console_(0), magnifier_(0)
{
  setWindowTitle(tr("Annot Browser"));
  setWindowIcon(QIcon(RC_IMAGE_APP));
  setWindowFlags(WINDOW_FLAGS);
  setTextSizeMultiplier(TEXT_SIZE_SCALE);

#ifdef Q_WS_WIN
  if (!QtWin::isWindowsVistaOrLater())
#endif // Q_WS_WIN
  { setContentsMargins(4, 2, 4, 2); } // left, top, right, bottom

  autoHideToolBarTimer_ = new QTimer(this);
  autoHideToolBarTimer_->setSingleShot(true);
  autoHideToolBarTimer_->setInterval(5000); // 5 seconds
  connect(autoHideToolBarTimer_, SIGNAL(timeout()), SLOT(autoHideToolBar()));

  loadCookieJar();

  setCacheDirectory(G_PATH_CACHES);

  setBlockedUrls(QList<QUrl>()
    << QString("http://googlesyndication.com")
    << QString("http://wangmeng.baidu.com")
    << QString("http://www.baidu.com/adrc.php")
    << QString("http://cpro.baidu.com")
    << QString("http://static.loli.my/ad-images")
    << QString("http://ads.nicovideo.jp")
    << QString("http://taobao.com")
    << QString("http://u17.com")
    << QString("http://u17i.com")
    << QString("http://u17t.com")
    << QString("http://17kuxun.com")
    << QString("http://huoying.com")
    << QString("http://8864.com")
    << QString("http://www.bilibili.tv/html/arcgg.html")
  );

  int lang = AcSettings::globalSettings()->language();
  switch (lang) {
  case QLocale::English: setHomePage(HOMEPAGE_JP); break;
  case QLocale::Chinese: setHomePage(HOMEPAGE_ZH); break;
  default:               setHomePage(HOMEPAGE_JP);
  }
  setHomePages(QStringList()
    << "google.com"
    << "nicovideo.jp"
    << "ch.nicovideo.jp/menu/anime"
    << "ja.wikipedia.org/wiki/"
    << "www.light.gr.jp"
    << WbNetworkAccessManager::supportedSites()
  );

  AcUi::globalInstance()->setWindowStyle(this);
  AcUi::globalInstance()->setStatusBarStyle(statusBar());
  setWindowOpacity(1.0); // window is opaque

  fadeAni_ = new FadeAnimation(this, "windowOpacity", this);

  // - IPC -
  playerDelegate_ = new AcPlayer(this);
  downloaderDelegate_ = new AcDownloader(this);
  connect(this, SIGNAL(openUrlWithAcPlayerRequested(QString)), SLOT(openUrlWithAcPlayer(QString)));
  connect(this, SIGNAL(importUrlToAcPlayerRequested(QString)), SLOT(importUrlToAcPlayer(QString)));
  connect(this, SIGNAL(openUrlWithAcDownloaderRequested(QString)), SLOT(openUrlWithAcDownloader(QString)));
  connect(this, SIGNAL(newWindowRequested()), SLOT(newWindow()));
  connect(this, SIGNAL(fullScreenRequested()), SLOT(toggleFullScreen()));
  /*
  connect(AcBrowserController::globalController(), SIGNAL(message(QString)),
          SLOT(showMessage(QString)), Qt::QueuedConnection);
  connect(AcBrowserController::globalController(), SIGNAL(error(QString)),
          SLOT(error(QString)), Qt::QueuedConnection);
  connect(AcBrowserController::globalController(), SIGNAL(warning(QString)),
          SLOT(warn(QString)), Qt::QueuedConnection);
  connect(AcBrowserController::globalController(), SIGNAL(notification(QString)),
          SLOT(notify(QString)), Qt::QueuedConnection);
  connect(AcBrowserController::globalController(), SIGNAL(arguments(QStringList)),
          SLOT(openUrls(QStringList)), Qt::QueuedConnection);
  */

  // - Message -
  DownloaderController *dc = DownloaderController::globalController();
  connect(dc, SIGNAL(message(QString)), SLOT(showMessage(QString)), Qt::QueuedConnection);
  connect(dc, SIGNAL(warning(QString)), SLOT(warn(QString)), Qt::QueuedConnection);
  connect(dc, SIGNAL(error(QString)), SLOT(error(QString)), Qt::QueuedConnection);
  connect(dc, SIGNAL(notification(QString)), SLOT(notify(QString)), Qt::QueuedConnection);

  // - Shortcuts -
  new QShortcut(QKeySequence::New, this, SLOT(newWindow()));

  createMenus();
  createGestures();

  // - Settings -
  Settings *settings = Settings::globalSettings();
  setClosedUrls(settings->closedUrls());
  addRecentUrls(settings->recentUrls() << WB_BLANK_PAGE);

  int searchEngine = settings->searchEngine();
  if (searchEngine < 0)
    switch (lang) {
    case QLocale::Japanese: searchEngine = SearchEngineFactory::Nicovideo; break;
    case QLocale::Chinese:  searchEngine = SearchEngineFactory::Bilibili; break;
    case QLocale::English:
    default:                searchEngine = SearchEngineFactory::Google;
    }
  setSearchEngine(searchEngine);

  QStringList searches = settings->recentSearches();
  addRecentSearches(searches);
  if (!searches.isEmpty())
    setSearchedText(searches.first());

  DOUT("searchEngine =" << searchEngine << Base::searchEngine());

  setMinimumSize(MIN_SIZE);

  if (settings->isFullScreen()) {
    resize(DEFAULT_SIZE);
    //setFullScreen(true);
  } else {
    QSize size = settings->recentSize();
    if (size.isEmpty() || !settings->hasRecentTabs() ||
        !isValidWindowSize(size))
      size = DEFAULT_SIZE;
    resize(size);
  }

  //grabGesture(Qt::PanGesture);
  //grabGesture(Qt::SwipeGesture);
  //grabGesture(Qt::PinchGesture);

  DOUT("size =" << size());
  Application::globalInstance()->addWindow(this);
}

void
MainWindow::createGestures()
{
  MouseGesture *g;

  addMouseGesture(g = new MouseGesture(MouseGesture::DirectionList()
    << MouseGesture::NoMatch,
    tr("Unrecognized Gesture")
   ));

  addMouseGesture(g = new MouseGesture(MouseGesture::DirectionList()
    << MouseGesture::Down,
    tr("Open Link in Background Tab")
   )); connect(g, SIGNAL(triggered()), SLOT(openLinkInNewTab()), Qt::QueuedConnection);

  addMouseGesture(g = new MouseGesture(MouseGesture::DirectionList()
    << MouseGesture::Down << MouseGesture::Right,
    tr("Close Tab")
  )); connect(g, SIGNAL(triggered()), SLOT(closeTab()), Qt::QueuedConnection);

  addMouseGesture(g = new MouseGesture(MouseGesture::DirectionList()
    << MouseGesture::Right << MouseGesture::Left,
    tr("New Tab")
  )); connect(g, SIGNAL(triggered()), SLOT(newTabAfterCurrentWithBlankPage()), Qt::QueuedConnection);

  addMouseGesture(g = new MouseGesture(MouseGesture::DirectionList()
    << MouseGesture::Left << MouseGesture::Right,
    tr("Undo Close Tab")
  )); connect(g, SIGNAL(triggered()), SLOT(undoCloseTab()), Qt::QueuedConnection);

  addMouseGesture(g = new MouseGesture(MouseGesture::DirectionList()
    << MouseGesture::Up << MouseGesture::Down,
    tr("Reload")
  )); connect(g, SIGNAL(triggered()), SLOT(reload()));

  addMouseGesture(g = new MouseGesture(MouseGesture::DirectionList()
    << MouseGesture::Left,
    tr("Forward")
  )); connect(g, SIGNAL(triggered()), SLOT(forward()), Qt::QueuedConnection);

  addMouseGesture(g = new MouseGesture(MouseGesture::DirectionList()
    << MouseGesture::Right,
    tr("Backward")
  )); connect(g, SIGNAL(triggered()), SLOT(back()), Qt::QueuedConnection);

  addMouseGesture(g = new MouseGesture(MouseGesture::DirectionList()
    << MouseGesture::Up << MouseGesture::Left,
    tr("Previous Tab")
  )); connect(g, SIGNAL(triggered()), SLOT(previousTab()));

  addMouseGesture(g = new MouseGesture(MouseGesture::DirectionList()
    << MouseGesture::Up << MouseGesture::Right,
    tr("Next Tab")
  )); connect(g, SIGNAL(triggered()), SLOT(nextTab()), Qt::QueuedConnection);

  addMouseGesture(g = new MouseGesture(MouseGesture::DirectionList()
    << MouseGesture::Right << MouseGesture::Down,
    tr("Minimize Window")
  )); connect(g, SIGNAL(triggered()), SLOT(showMinimized()), Qt::QueuedConnection);

  addMouseGesture(g = new MouseGesture(MouseGesture::DirectionList()
    << MouseGesture::Right << MouseGesture::Up,
    tr("Toggle Full Screen")
  )); connect(g, SIGNAL(triggered()), SLOT(toggleFullScreen()), Qt::QueuedConnection);

  addMouseGesture(g = new MouseGesture(MouseGesture::DirectionList()
    << MouseGesture::Left << MouseGesture::Up,
    tr("Scroll Top")
  )); connect(g, SIGNAL(triggered()), SLOT(scrollTop()), Qt::QueuedConnection);

  addMouseGesture(g = new MouseGesture(MouseGesture::DirectionList()
    << MouseGesture::Left << MouseGesture::Down,
    tr("Scroll Bottom")
  )); connect(g, SIGNAL(triggered()), SLOT(scrollBottom()), Qt::QueuedConnection);
}

void
MainWindow::createMenus()
{
#ifdef Q_WS_WIN
  menuBar()->hide();
#endif // Q_WS_WIN
  QMenu *
  m = menuBar()->addMenu(tr("&File")); {
    m->addAction(tr("New Window"), this, SLOT(newWindow()), QKeySequence::New);
    m->addAction(tr("New Tab"), this, SLOT(newTabAfterCurrentWithBlankPage()), QKeySequence::AddTab);
    m->addSeparator();
    m->addAction(tr("Clip"), this, SLOT(clip()), QKeySequence::Save);
    m->addSeparator();
    m->addAction(tr("Close Tab"), this, SLOT(closeTab()), QKeySequence("CTRL+W"));
    m->addAction(tr("Close Window"), this, SLOT(quit()));
    m->addAction(tr("Exit"), Application::globalInstance(), SLOT(close()), QKeySequence::Quit);
  }

  //editMenu = menuBar()->addMenu(tr("&Edit"));
  //editMenu->addAction(undoAct);
  //editMenu->addAction(insertAct);
  //editMenu->addAction(analyzeAct);
  //editMenu->addAction(transformAct);
  //editMenu->addAction(applyAct);
  //editMenu->addAction(generateAct);
  //editMenu->addAction(clearAct);

  m = menuBar()->addMenu(tr("&View")); {
    m->addAction(tr("Reload"), this, SLOT(reload()), QKeySequence("CTRL+R"));
    m->addAction(tr("Stop"), this, SLOT(stop()), QKeySequence("CTRL+."));
    m->addAction(tr("Inspect"), this, SLOT(inspect()), QKeySequence("CTRL+I"));
    m->addSeparator();
    m->addAction(tr("Actual Size"), this, SLOT(zoomReset()), QKeySequence("CTRL+0"));
    m->addAction(tr("Zoom In"), this, SLOT(zoomIn()), QKeySequence("CTRL+="));
    m->addAction(tr("Zoom Out"), this, SLOT(zoomOut()), QKeySequence("CTRL+-"));
    m->addSeparator();
    m->addAction(tr("Toggle Full Screen"), this, SLOT(toggleFullScreen()), QKeySequence("CTRL+META+F"));
  }

  m = menuBar()->addMenu(tr("&Settings")); {
    m->addAction(tr("&Preferences"), this, SLOT(preferences()), QKeySequence("CTRL+,"));
  }

  m = menuBar()->addMenu(tr("&Help")); {
    //m->addAction(tr("&Help"), this, SLOT(help())); // DO NOT TRANSLATE ME
    m->addAction(tr("&Console"), this, SLOT(showConsole()));
    m->addAction(tr("&About"), this, SLOT(about())); // DO NOT TRANSLATE ME
  }
#ifdef Q_WS_WIN
  new QShortcut(QKeySequence("ALT+O"), this, SLOT(preferences()));
#endif // Q_WS_WIN
}

QStringList
MainWindow::startupUrls()
{
  QStringList ret = Settings::globalSettings()->recentTabs();
  if (!ret.isEmpty())
    showMessage(tr("restoring last sessions ..."));
  //else
  //  switch (AcSettings::globalSettings()->language()) {
  //  case QLocale::Chinese:
  //    ret.append(HOMEPAGE_ZH);
  //    ret.append(HOMEPAGE_JP);
  //    break;
  //  case QLocale::Japanese:
  //    ret.append(HOMEPAGE_JP);
  //    ret.append(HOMEPAGE_ZH);
  //    break;
  //  default:
  //    ret.append(HOMEPAGE_EN);
  //    ret.append(HOMEPAGE_JP);
  //  }
  //}
  return ret;
}

void
MainWindow::login()
{
  DOUT("enter");
  QString userName, password;
  boost::tie(userName, password) = AcSettings::globalSettings()->bilibiliAccount();
  if (!userName.isEmpty() && !password.isEmpty()) {
    showMessage(tr("logging in bilibili.tv as %1 ...").arg(userName));
    bilibili::login(userName, password, cookieJar());
  }
  boost::tie(userName, password) = AcSettings::globalSettings()->nicovideoAccount();
  if (!userName.isEmpty() && !password.isEmpty()) {
    showMessage(tr("logging in nicovideo.jp as %1 ...").arg(userName));
    nico::login(userName, password, cookieJar());
  }
  DOUT("exit");
}

// - Events -

void
MainWindow::quit()
{
  fadeAni_->fadeOut();
  QTimer::singleShot(fadeAni_->duration(), this, SLOT(hide()));
  QTimer::singleShot(fadeAni_->duration(), this, SLOT(close()));
}

void
MainWindow::mouseMoveEvent(QMouseEvent *event)
{
  if (isFullScreen() &&
      isGlobalPosAroundToolBar(event->globalPos())) {
    setToolBarVisible(true);
    setTabBarVisible(true);
    //autoHideToolBarTimer_->start();
  }
  Base::mouseMoveEvent(event);
}

void
MainWindow::mousePressEvent(QMouseEvent *event)
{
  if (isFullScreen() && event->button() == Qt::LeftButton &&
      isGlobalPosAroundToolBar(event->globalPos())) {
    bool t = isTabBarVisible();
    setToolBarVisible(!t);
    setTabBarVisible(!t);
   if (autoHideToolBarTimer_->isActive())
      autoHideToolBarTimer_->stop();
  }
  Base::mousePressEvent(event);
}

void
MainWindow::focusInEvent(QFocusEvent *e)
{
  if (this != Application::globalInstance()->mainWindow())
    Application::globalInstance()->setMainWindow(this);
  Base::focusInEvent(e);
}

void
MainWindow::keyPressEvent(QKeyEvent *e)
{
  switch (e->key()) {
  case Qt::Key_CapsLock:
#ifdef Q_WS_WIN
    menuBar()->setVisible(!menuBar()->isVisible());
#endif // Q_WS_WIN
    break;
  default: ;
  }
  Base::keyReleaseEvent(e);
}

bool
MainWindow::event(QEvent *e)
{
  bool accept = true;
  switch (e->type()) {
  case QEvent::FileOpen: // See: http://www.qtcentre.org/wiki/index.php?title=Opening_documents_in_the_Mac_OS_X_Finder
    {
      QFileOpenEvent *fe = static_cast<QFileOpenEvent *>(e);
      Q_ASSERT(fe);
      QString url = fe->file();
      if (!url.isEmpty())
        url.prepend("file:///");
      else {
        url = fe->url().toString();
        if (!url.isEmpty())
          url = QUrl::fromPercentEncoding(url.toLocal8Bit());
      }
      if (!url.isEmpty()) {
        if (tabCount() && tabAddress(tabIndex()) == WB_BLANK_PAGE)
          QTimer::singleShot(0, new slot_::OpenUrl(url, this), SLOT(trigger()));
        else
          QTimer::singleShot(0, new slot_::NewTab(url, this), SLOT(trigger()));
      }
    } break;
  case QEvent::Gesture:
    DOUT("gesture event");
  default: accept = Base::event(e);
  }
  return accept;
}

void
MainWindow::saveRecentTabs()
{
  if (tabCount() > 1) {
    QStringList urls;
    foreach (QString t, tabAddresses()) {
      t = t.trimmed();
      if (!t.isEmpty() && t != WB_BLANK_PAGE)
        urls.append(t);
    }
    urls = QtExt::uniqueList(urls);
    Settings::globalSettings()->setRecentTabs(urls);
  } else
    Settings::globalSettings()->clearRecentTabs();
}

void
MainWindow::closeEvent(QCloseEvent *event)
{
  DOUT("enter");
  enum { UrlSizeLimit = 20 };
  enum { SearchSizeLimit = 20 };

  Application::globalInstance()->removeWindow(this);

  if (fadeAni_->state() == QAbstractAnimation::Running)
    fadeAni_->stop();

  // Save settings
  saveRecentTabs();
  Settings *settings = Settings::globalSettings();
  settings->setClosedUrls(QtExt::skipEmpty(closedUrls()), UrlSizeLimit);
  settings->setRecentUrls(QtExt::skipEmpty(recentUrls()), UrlSizeLimit);
  settings->setRecentSearches(recentSearches(), SearchSizeLimit);
  settings->setRecentTabIndex(tabIndex());
  if (isFullScreen())
    settings->clearRecentSize();
  else
    settings->setRecentSize(size());
  DOUT("size =" << size());

  settings->setSearchEngine(searchEngine());

  settings->setFullScreen(isFullScreen());

  saveCookieJar();

#ifdef Q_WS_WIN
  QTimer::singleShot(0, qApp, SLOT(quit())); // ensure quit app and clean up zombie threads
#endif // Q_WS_WIN

  Base::closeEvent(event);
  DOUT("close");
}

void
MainWindow::setVisible(bool visible)
{
  if (visible && !isVisible())
    fadeAni_->fadeIn();
  Base::setVisible(visible);

  if (visible && tabCount() <= 0) {
    Settings *s = Settings::globalSettings();

    openUrls(startupUrls());
    focusTab(s->recentTabIndex());
    s->clearRecentTabs();
    s->setRecentTabIndex(0);

    if (tabCount() <= 0)
      newTabAtLastWithBlankPage();

    QTimer::singleShot(0, this, SLOT(focusLocationBar()));
  }
}

// - New window -

void
MainWindow::newWindow()
{
#ifdef USE_MDI
  showMessage(tr("openning new window") + " ...");
  (new Self)->show();
#else
  bool ok = false;
  QString app = QCoreApplication::applicationFilePath();
#ifdef Q_WS_MAC
  Q_UNUSED(app);
  //ok = QtMac::open(app);
#elif defined Q_WS_WIN
  ok = QProcess::startDetached('"' + app + '"');
#else
  ok = QProcess::startDetached(app);
#endif // Q_WS_
  if (ok)
    showMessage(tr("openning new window") + " ...");
  else
    warn(tr("failed open new window"));
#endif // USE_MDI
}

// - IPC -

void
MainWindow::openUrlWithAcPlayer(const QString &address)
{
  DOUT("enter: address =" << address);
  QString url = address.trimmed();
  if (url.isEmpty())
    return;
  if (!url.startsWith("http://"))
    url.prepend("http://");
  showMessage(tr("openning") + " " + url);
  playerDelegate_->openUrl(url);
  DOUT("exit");
}

void
MainWindow::importUrlToAcPlayer(const QString &address)
{
  DOUT("enter: address =" << address);
  QString url = address.trimmed();
  if (url.isEmpty())
    return;
  if (!url.startsWith("http://"))
    url.prepend("http://");
  showMessage(tr("openning") + " " + url);
  playerDelegate_->importUrl(url);
  DOUT("exit");
}

void
MainWindow::openUrlWithAcDownloader(const QString &address)
{
  DOUT("enter: address =" << address);
  QString url = address.trimmed();
  if (url.isEmpty())
    return;
  if (!url.startsWith("http://", Qt::CaseInsensitive))
    url.prepend("http://");
  showMessage(tr("openning") + " " + url);
  downloaderDelegate_->openUrl(url);
  DOUT("exit");
}

// - Caches -

void
MainWindow::loadCookieJar()
{
  bool ok = QtExt::readCookiesfromFile(cookieJar(), G_PATH_COOKIE);
  Q_UNUSED(ok);
  DOUT("ok =" << ok);
}

void
MainWindow::saveCookieJar()
{
  bool ok = QtExt::writeCookiesToFile(cookieJar(), G_PATH_COOKIE);
  Q_UNUSED(ok);
  DOUT("ok =" << ok);
}

// - Window -

bool
MainWindow::isValidWindowSize(const QSize &size) const
{
  enum { MinWidth = 400, MinHeight = 300 };
  if (size.width() < MinWidth || size.height() < MinHeight)
    return false;

  QSize desktop = QApplication::desktop()->availableGeometry(this).size();
  return desktop.isEmpty() ||
         size.width() > desktop.width() && size.height() > desktop.height();
}

// - Actions -

void
MainWindow::clip()
{
  QtExt::WebView *w = qobject_cast<QtExt::WebView *>(tabWidget());
  if (w)
    w->clip();
}

void
MainWindow::reload()
{
  QtExt::WebView *w = qobject_cast<QtExt::WebView *>(tabWidget());
  if (w)
    w->reload();
}

void
MainWindow::stop()
{
  QtExt::WebView *w = qobject_cast<QtExt::WebView *>(tabWidget());
  if (w)
    w->stop();
}

void
MainWindow::zoomIn()
{
  QtExt::WebView *w = qobject_cast<QtExt::WebView *>(tabWidget());
  if (w)
    w->zoomIn();
}

void
MainWindow::zoomOut()
{
  QtExt::WebView *w = qobject_cast<QtExt::WebView *>(tabWidget());
  if (w)
    w->zoomOut();
}

void
MainWindow::zoomReset()
{
  QtExt::WebView *w = qobject_cast<QtExt::WebView *>(tabWidget());
  if (w)
    w->zoomReset();
}

void
MainWindow::inspect()
{
  QWebView *w = qobject_cast<QWebView *>(tabWidget());
  if (w) {
    QWebPage *page = w->page();
    if (page)
      page->triggerAction(QWebPage::InspectElement);
  }
}

void
MainWindow::scrollTop()
{
  QtExt::WebView *w = qobject_cast<QtExt::WebView *>(tabWidget());
  if (w)
    w->scrollTop();
}

void
MainWindow::scrollBottom()
{
  QtExt::WebView *w = qobject_cast<QtExt::WebView *>(tabWidget());
  if (w)
    w->scrollBottom();
}

void
MainWindow::about()
{
  static QWidget *w = 0;
  if (!w) {
    w = new AcAbout(G_APPLICATION, G_VERSION);
    connect(qApp, SIGNAL(aboutToQuit()), w, SLOT(close()));
  }
  w->show();
}

void
MainWindow::preferences()
{
  static QWidget *w = 0;
  if (!w) {
    w = new Preferences;
    connect(qApp, SIGNAL(aboutToQuit()), w, SLOT(close()));
  }
  w->show();
}

void
MainWindow::setFullScreen(bool t)
{
#ifdef Q_WS_WIN
  if (AcUi::globalInstance()->isAeroEnabled())
    AcUi::globalInstance()->setWindowDwmEnabled(this, false);
#endif // Q_WS_WIN

  //setMouseTracking(t);
  setDraggable(!t);
  if (t) {
    setWindowFlags(Qt::Window);
    showFullScreen();
    notify(tr("Double click any tab to exit full screen."));
  } else {
    setWindowFlags(WINDOW_FLAGS);
    showNormal();
  }

#ifdef Q_WS_WIN
  if (AcUi::globalInstance()->isAeroEnabled())
    AcUi::globalInstance()->setWindowDwmEnabled(this, true);
#endif // Q_WS_WIN

  if (t)
    autoHideToolBarTimer_->start();
  else {
    autoHideToolBarTimer_->stop();
    setToolBarVisible(true);
    setTabBarVisible(true);
  }
}

void
MainWindow::autoHideToolBar()
{
  if (isToolBarUnderMouse() || isTabBarUnderMouse() ||
      toolBarHasFocus() || tabBarHasFocus())
    autoHideToolBarTimer_->start();
  else {
    notify(tr("Click any edge of the screen to toggle tool bars."));
    setToolBarVisible(false);
    setTabBarVisible(false);
  }
}

bool
MainWindow::isGlobalPosAroundToolBar(const QPoint &pos) const
{
  enum { margin = 10, top = 10 };
  QRect r(margin, margin + top, width() - (margin + margin), height() - (margin + margin + top));
  return !r.contains(pos);
}

// - Console -

void
MainWindow::showConsole()
{
  if (!console_) {
    Application::globalInstance()->installMessageHandlers();
    console_ = new AcConsole(this);
    Application::globalInstance()->addMessageHandler(AcConsole::messageHandler);
  }
  console_->show();
}

void
MainWindow::toggleMagnifier()
{
#ifdef WITH_MODULE_MAGNIFIER
  if (!magnifier_) {
    magnifier_ = new Magnifier(this);
    magnifier_->setWidget(QApplication::desktop());
    new QShortcut(QKeySequence("CTRL+E"), magnifier_, SLOT(hide()));

    QRect r = QApplication::desktop()->geometry();
    QPoint pos = r.bottomRight() - magnifier_->rect().bottomRight();
    magnifier_->move(pos);
  }
  if (magnifier_->isVisible())
    magnifier_->fadeOut();
  else {
    magnifier_->show();
    magnifier_->raise();
  }
#endif // WITH_MODULE_MAGNIFIER
}

// EOF
