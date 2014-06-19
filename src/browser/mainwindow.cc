// mainwindow.cc
// 3/14/2012
#include "mainwindow.h"
#include "mainwindow_p.h"
#include "settings.h"
#include "rc.h"
#include "global.h"
#include "application.h"
#include "preferences.h"
#include "src/common/acabout.h"
#include "src/common/acsettings.h"
#include "src/common/acui.h"
#include "src/common/acbrowser.h"
#include "src/common/actranslator.h"
#include "src/common/acplayer.h"
#include "src/common/acdownloader.h"
#include "src/common/acconsole.h"
#include "src/common/acprotocol.h"
#include "lib/webbrowser/network/wbnetworkaccessmanager.h"
#include "lib/animation/fadeanimation.h"
#include "lib/searchengine/searchenginefactory.h"
#include "lib/nicoutil/nicoutil.h"
#include "lib/download/downloader.h"
#include "lib/mousegesture/mousegesture.h"
#include "lib/annotdown/annotationdownloader.h"
#include "qtx/qxalgorithm.h"
#include "qtx/qxnetworkcookie.h"
#include "qtx/qxwebview.h"
#ifdef WITH_LIB_MAGNIFIER
# include "lib/magnifier/magnifier.h"
#endif // WITH_LIB_MAGNIFIER
#ifdef Q_OS_WIN
# include "win/qtwin/qtwin.h"
#endif // Q_OS_WIN
#include <QtGui>
#include <QtWebKit>
#include <boost/tuple/tuple.hpp>

#define DEBUG "mainwindow"
#include "qtx/qxdebug.h"

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses
#endif // __GNUC__

#define HOMEPAGE_EN    "http://www.youtube.com/FUNimation"
#define HOMEPAGE_JP    "http://ch.nicovideo.jp/menu/anime"
#define HOMEPAGE_ZH    "http://www.bilibili.com/video/bangumi.html"

#define MIN_SIZE        QSize(400, 300)
#ifdef Q_OS_WIN
# define DEFAULT_SIZE  QSize(1000 + 9*2, 650)
#else
# define DEFAULT_SIZE  QSize(1000 + 9*2, 710) // width for nicovideo.jp + margin*2, height for newtab on mac
#endif // Q_OS_WIN

// - Construction -

#define WINDOW_FLAGS ( \
  Qt::Window | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint )

#ifdef Q_OS_WIN
# define TEXT_SIZE_SCALE 0.9
#else
# define TEXT_SIZE_SCALE 1.0
#endif // Q_OS_MAC

#ifdef Q_OS_MAC
# define USE_MDI
#endif // Q_OS_MAC

MainWindow::MainWindow(QWidget *parent)
  : Base(parent), console_(0), magnifier_(0)
{
  setWindowTitle(tr("Annot Browser"));
  setWindowIcon(QIcon(RC_IMAGE_APP));
  setWindowFlags(WINDOW_FLAGS);
  setTextSizeMultiplier(TEXT_SIZE_SCALE);

  AcUi::globalInstance()->setStatusBarStyle(statusBar());

#ifdef Q_OS_WIN
  if (!QtWin::isWindowsVistaOrLater())
#endif // Q_OS_WIN
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
    << QString("http://drmcmm.baidu.com")
    << QString("http://www.baidu.com/cb.php")
    << QString("http://static.loli.my/ad-images")
    << QString("http://ads.nicovideo.jp")
    << QString("http://static.atm.youku.com")
    << QString("http://taobao.com")
    << QString("http://taobaocdn.com")
    << QString("http://u17.com")
    << QString("http://u17i.com")
    << QString("http://u17t.com")
    << QString("http://37cs.com")
    << QString("http://17kuxun.com")
    << QString("http://huoying.com")
    << QString("http://linekong.com")
    << QString("http://8864.com")
    << QString("http://www.bilibili.com/html/arcgg.html")
  );

  if (AcSettings::globalSettings()->isJapanese())
    setHomePage(HOMEPAGE_JP);
  else if (AcSettings::globalSettings()->isChinese())
    setHomePage(HOMEPAGE_ZH);
  else
    setHomePage(HOMEPAGE_JP);
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
  DOUT("create app delegates");
  playerDelegate_ = new AcPlayer(this);
  downloaderDelegate_ = new AcDownloader(this);
  translatorDelegate_ = new AcTranslator(this);
  connect(this, SIGNAL(openUrlWithAcPlayerRequested(QString)), SLOT(openUrlWithAcPlayer(QString)));
  connect(this, SIGNAL(importUrlToAcPlayerRequested(QString)), SLOT(importUrlToAcPlayer(QString)));
  connect(this, SIGNAL(openUrlWithAcDownloaderRequested(QString)), SLOT(openUrlWithAcDownloader(QString)));
  connect(this, SIGNAL(downloadAnnotationUrlRequested(QString)), SLOT(downloadAnnotationUrl(QString)));
  connect(this, SIGNAL(newWindowRequested()), SLOT(newWindow()));
  connect(this, SIGNAL(fullScreenRequested()), SLOT(toggleFullScreen()));

  // TODO: Translate selected text
  //connect(this, SIGNAL(selectedTextChanged(QString)), SLOT(translateText(QString)));

  //AC_CONNECT_MESSAGES(AcBrowserController::globalController(), this, Qt::QueuedConnection);
  //connect(AcBrowserController::globalController(), SIGNAL(arguments(QStringList)),
  //        SLOT(openUrls(QStringList)), Qt::QueuedConnection);

  annotationDownloader_ = new AnnotationDownloader(this);
  annotationDownloader_->setDownloadsLocation(QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));
  connect(annotationDownloader_, SIGNAL(fileSaved(QString)), SLOT(notifyFileSaved(QString)), Qt::QueuedConnection);

  // - Messages -
  AC_CONNECT_MESSAGE(annotationDownloader_, this, Qt::QueuedConnection);
  AC_CONNECT_ERROR(annotationDownloader_, this, Qt::QueuedConnection);
  AC_CONNECT_WARNING(annotationDownloader_, this, Qt::QueuedConnection);

  AC_CONNECT_MESSAGES(DownloaderController::globalController(), this, Qt::QueuedConnection);

  // - Shortcuts -
  new QShortcut(QKeySequence::New, this, SLOT(newWindow()));

  DOUT("create menus");
  createMenus();

  DOUT("create gestures");
  createGestures();

  // - Settings -

  DOUT("load settings");
  Settings *settings = Settings::globalSettings();
  setClosedUrls(settings->closedUrls());
  addRecentUrls(settings->recentUrls() << WB_BLANK_PAGE);

  DOUT("load search history");
  int searchEngine = settings->searchEngine();
  if (searchEngine < 0)
    searchEngine = AcSettings::globalSettings()->isJapanese() ? SearchEngineFactory::Nicovideo :
                   AcSettings::globalSettings()->isChinese() ? SearchEngineFactory::Bilibili :
                   SearchEngineFactory::Google;
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
    tr("Backward")
  )); connect(g, SIGNAL(triggered()), SLOT(back()), Qt::QueuedConnection);

  addMouseGesture(g = new MouseGesture(MouseGesture::DirectionList()
    << MouseGesture::Right,
    tr("Forward")
  )); connect(g, SIGNAL(triggered()), SLOT(forward()), Qt::QueuedConnection);

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
#ifdef Q_OS_WIN
  menuBar()->hide();
#endif // Q_OS_WIN
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
#ifdef Q_OS_WIN
  new QShortcut(QKeySequence("ALT+O"), this, SLOT(preferences()));
#endif // Q_OS_WIN
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
    showMessage(tr("logging in bilibili.com as %1 ...").arg(userName));
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
#ifdef Q_OS_WIN
    menuBar()->setVisible(!menuBar()->isVisible());
#endif // Q_OS_WIN
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
      auto fe = static_cast<QFileOpenEvent *>(e);
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
          QTimer::singleShot(0, new detail::OpenUrl(url, this), SLOT(trigger()));
        else
          QTimer::singleShot(0, new detail::NewTab(url, this), SLOT(trigger()));
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
    qxStableUniqueList(urls);
    Settings::globalSettings()->setRecentTabs(urls);
  } else
    Settings::globalSettings()->clearRecentTabs();
}

void
MainWindow::closeEvent(QCloseEvent *event)
{
  DOUT("enter");
  enum { RecentUrlsLimit = 30 };
  enum { ClosedUrlsLimit = 10 };
  enum { RecentSearchLimit = 20 };

  Application::globalInstance()->removeWindow(this);

  if (fadeAni_->state() == QAbstractAnimation::Running)
    fadeAni_->stop();

  // Save settings
  saveRecentTabs();
  Settings *settings = Settings::globalSettings();
  settings->setClosedUrls(qxSkipEmpty(closedUrls()), ClosedUrlsLimit);
  settings->setRecentUrls(qxSkipEmpty(recentUrls()), RecentUrlsLimit);
  settings->setRecentSearches(recentSearches(), RecentSearchLimit);
  settings->setRecentTabIndex(tabIndex());
  if (isFullScreen())
    settings->clearRecentSize();
  else
    settings->setRecentSize(size());
  DOUT("size =" << size());

  settings->setSearchEngine(searchEngine());

  settings->setFullScreen(isFullScreen());

  saveCookieJar();

#ifdef Q_OS_WIN
  QTimer::singleShot(0, qApp, SLOT(quit())); // ensure quit app and clean up zombie threads
#endif // Q_OS_WIN

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
  showMessage(tr("opening new window") + " ...");
  (new Self)->show();
#else
  bool ok = false;
  QString app = QCoreApplication::applicationFilePath();
#ifdef Q_OS_MAC
  Q_UNUSED(app)
  //ok = QtMac::open(app);
#elif defined(Q_OS_WIN)
  ok = QProcess::startDetached('"' + app + '"');
#else
  ok = QProcess::startDetached(app);
#endif // Q_OS_
  if (ok)
    showMessage(tr("opening new window") + " ...");
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
  showMessage(tr("opening") + ": " + url);
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
  showMessage(tr("opening") + ": " + url);
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
  showMessage(tr("opening") + ": " + url);
  downloaderDelegate_->openUrl(url);
  DOUT("exit");
}

void
MainWindow::downloadAnnotationUrl(const QString &address)
{
  DOUT("enter: address =" << address);
  QString url = address.trimmed();
  if (url.isEmpty())
    return;
  if (!url.startsWith("http://", Qt::CaseInsensitive))
    url.prepend("http://");
  showMessage(tr("saving") + ": " + url);
  annotationDownloader_->download(address);
  DOUT("exit");
}

// - Caches -

void
MainWindow::loadCookieJar()
{
  bool ok = QxNetwork::readCookiesfromFile(cookieJar(), G_PATH_COOKIE);
  Q_UNUSED(ok)
  DOUT("ok =" << ok);
}

void
MainWindow::saveCookieJar()
{
  bool ok = QxNetwork::writeCookiesToFile(cookieJar(), G_PATH_COOKIE);
  Q_UNUSED(ok)
  DOUT("ok =" << ok);
}

// - Translate -

void
MainWindow::translateText(const QString &text)
{ translatorDelegate_->translate(text); }

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
  auto w = qobject_cast<QxWebView *>(tabWidget());
  if (w)
    w->clip();
}

void
MainWindow::reload()
{
  auto w = qobject_cast<QxWebView *>(tabWidget());
  if (w)
    w->reload();
}

void
MainWindow::stop()
{
  auto w = qobject_cast<QxWebView *>(tabWidget());
  if (w)
    w->stop();
}

void
MainWindow::zoomIn()
{
  auto w = qobject_cast<QxWebView *>(tabWidget());
  if (w)
    w->zoomIn();
}

void
MainWindow::zoomOut()
{
  auto w = qobject_cast<QxWebView *>(tabWidget());
  if (w)
    w->zoomOut();
}

void
MainWindow::zoomReset()
{
  auto w = qobject_cast<QxWebView *>(tabWidget());
  if (w)
    w->zoomReset();
}

void
MainWindow::inspect()
{
  auto w = qobject_cast<QWebView *>(tabWidget());
  if (w) {
    QWebPage *page = w->page();
    if (page)
      page->triggerAction(QWebPage::InspectElement);
  }
}

void
MainWindow::scrollTop()
{
  auto w = qobject_cast<QxWebView *>(tabWidget());
  if (w)
    w->scrollTop();
}

void
MainWindow::scrollBottom()
{
  auto w = qobject_cast<QxWebView *>(tabWidget());
  if (w)
    w->scrollBottom();
}

void
MainWindow::about()
{
  static QWidget *w = nullptr;
  if (!w) {
    w = new AcAbout(G_APPLICATION, G_VERSION);
    connect(qApp, SIGNAL(aboutToQuit()), w, SLOT(close()));
  }
  w->show();
}

void
MainWindow::preferences()
{
  static QWidget *w = nullptr;
  if (!w) {
    w = new Preferences;
    connect(qApp, SIGNAL(aboutToQuit()), w, SLOT(close()));
  }
  w->show();
}

void
MainWindow::setFullScreen(bool t)
{
#ifdef Q_OS_WIN
  if (AcUi::globalInstance()->isAeroEnabled())
    AcUi::globalInstance()->setWindowDwmEnabled(this, false);
#endif // Q_OS_WIN

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

#ifdef Q_OS_WIN
  if (AcUi::globalInstance()->isAeroEnabled())
    AcUi::globalInstance()->setWindowDwmEnabled(this, true);
#endif // Q_OS_WIN

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
#ifdef WITH_LIB_MAGNIFIER
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
#endif // WITH_LIB_MAGNIFIER
}

// - Log messages -

void
MainWindow::notifyFileSaved(const QString &fileName)
{
  showMessage(tr("file saved") + ": " + fileName);
  QApplication::beep();
}

// EOF
