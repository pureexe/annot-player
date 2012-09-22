// mainwindow.cc
// 6/30/2011

#include "mainwindow.h"
#include "mainwindow_p.h"
#include "application.h"
#include "preferences.h"

#include "datamanager.h"
#include "dataserver.h"
#include "eventlogger.h"
#include "grabber.h"
#include "mainconsole.h"
#include "miniconsole.h"
#include "osdwindow.h"
#include "playerui.h"
#include "videoview.h"
#include "gameview.h"
#include "textcodecmanager.h"
#include "embeddedplayer.h"
#include "embeddedcanvas.h"
#include "embeddedinfoview.h"
#include "positionslider.h"
#include "signalhub.h"
#include "mainplayer.h"
#include "miniplayer.h"
#include "clipboardmonitor.h"
#include "annotationsettings.h"
#include "annotationgraphicsview.h"
#include "annotationanalyticsview.h"
#include "annotationeditor.h"
#include "annotationfilter.h"
#include "gamelibrary.h"
#include "medialibrary.h"
#include "medialibraryview.h"
#include "mainlibraryview.h"
#include "tokenview.h"
#include "mediainfoview.h"
//#include "commentview.h"
#include "tray.h"
#include "annotationlistview.h"
#include "stylesheet.h"
#include "rc.h"
#include "tr.h"
#include "localizer.h"
#include "settings.h"
//#include "siteaccountview.h"
#include "annotationcountdialog.h"
#include "blacklistview.h"
#include "backlogdialog.h"
#include "consoledialog.h"
#include "devicedialog.h"
#include "helpdialog.h"
#include "logindialog.h"
//#include "networkproxydialog.h"
#include "pickdialog.h"
#include "seekdialog.h"
#include "mediaurldialog.h"
#include "annoturldialog.h"
#include "annotdownurldialog.h"
#include "shutdowndialog.h"
#include "sleepdialog.h"
#include "audiodelaydialog.h"
#include "userview.h"
#include "useranalyticsview.h"
#include "global.h"
#ifdef Q_OS_WIN
# include "registry.h"
#endif // Q_OS_WIN
#ifdef AC_ENABLE_GAME
# include "threadview.h"
# include "messagehandler.h"
# include "processview.h"
# include "processfilter.h"
# include "syncview.h"
# include "win/applocale/applocale.h"
#endif // AC_ENABLE_GAME
#include "src/common/acabout.h"
#include "src/common/acbrowser.h"
#include "src/common/acupdater.h"
#include "src/common/acdownloader.h"
#include "src/common/actranslator.h"
#include "src/common/acprotocol.h"
#include "src/common/acpaths.h"
#include "src/common/acplayer.h"
#include "src/common/acsettings.h"
#include "src/common/acui.h"
#include "lib/annotdb/annotdb.h"
#include "lib/annotdown/annotationdownloader.h"
#include "lib/mecab/mecabparser.h"
#include "lib/player/player.h"
#include "lib/player/playerdefs.h"
#include "lib/annotcodec/annotationcodecmanager.h"
#include "lib/animation/fadeanimation.h"
#include "lib/mrlanalysis/mrlanalysis.h"
#include "lib/mrlresolver/mrlresolvermanager.h"
#include "lib/mrlresolver/mrlresolversettings.h"
#include "lib/translator/translatormanager.h"
#include "lib/download/downloader.h"
#include "lib/searchengine/searchenginerc.h"
#include "lib/magnifier/magnifier.h"
#include "qtx/qxalgorithm.h"
#include "qtx/qxstring.h"
#include "qtx/qxactionwithid.h"
#include "qtx/qxcountdowntimer.h"
#include "qtx/qxdatetime.h"
#include "qtx/qxfs.h"
#include "htmlutil/htmltags.h"
#include "qtx/qxrubberband.h"
#include "lib/eventlistener/mouseclickeventlistener.h"
#include "lib/mediacodec/flvcodec.h"
#include "lib/imagefilter/rippleimagefilter.h"
#include "lib/textcodec/textcodec.h"
#include "lib/annotcloud/gamethread.h"
#include "lib/annotcloud/annottag.h"
#include "lib/annotcloud/annothtml.h"
#include "lib/annotcache/annotationcachemanager.h"
#include "lib/annotservice/annotserveragent.h"
#ifdef WITH_WIN_ATLAS
# include "win/atlas/atlas.h"
#endif // WITH_WIN_ATLAS
#ifdef WITH_LIB_DOLL
# include "lib/doll/doll.h"
#endif // WITH_LIB_DOLL
#ifdef WITH_WIN_HOOK
# include "win/hook/hook.h"
#endif // WITH_WIN_HOOK
#ifdef WITH_WIN_MOUSEHOOK
# include "win/mousehook/mousehook.h"
#endif // WITH_WIN_MOUSEHOOK
//#ifdef WITH_WIN_DWM
//# include "win/dwm/dwm.h"
//#endif // WITH_WIN_DWM
#ifdef WITH_WIN_TEXTHOOK
# include "win/texthook/texthook.h"
#endif // WITH_WIN_TEXTHOOK
#ifdef Q_OS_WIN
# include "win/qtwin/qtwin.h"
# include "win/qtwinnt/qtwinnt.h"
#endif // Q_OS_WIN
#ifdef WITH_WIN_PICKER
# include "win/picker/picker.h"
#endif // WITH_WIN_PICKER
#ifdef Q_OS_MAC
# include "mac/qtmac/qtmac.h"
//# include "mac/qtcocoa/qtcocoa.h"
//# include "mac/qtvlc/qtvlc.h"
#endif // Q_OS_MAC
#ifdef Q_OS_LINUX
# include "unix/qtx11/qtx11.h"
#endif // Q_OS_LINUX
#ifdef Q_OS_UNIX
# include "unix/qtunix/qtunix.h"
#endif // Q_OS_UNIX
#include <QtGui>
#include <QtNetwork/QNetworkAccessManager>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>

#include <ctime>

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses around && within ||
#endif // __GNUC__

#ifdef Q_OS_MAC
# define K_CMD         "CTRL"
# define K_ALT         "CTRL+ALT"
# define K_CTRL        "cmd"
# define K_SHIFT       "shift"
# define K_OPT         "opt"
#else
# define K_CMD         "ALT"
# define K_ALT         "ALT"
# define K_CTRL        "Ctrl"
# define K_SHIFT       "Shift"
# define K_OPT         "Alt"
#endif // Q_OS_MAC

using namespace AnnotCloud;

#define DEBUG "mainwindow"
#include "qtx/qxDebug.h"

enum { DEFAULT_LIVE_INTERVAL = 3000 }; // 3 seconds
enum { HISTORY_SIZE = 100 };   // Size of playPos/Sub/AudioTrack history
enum { HOLD_TIMEOUT = 2000 };
enum { DBLCLICK_TIMEOUT = 600 }; // around ::GetDoubleClickTime()
enum { CLICK_TIMEOUT = 500 };
enum { RECENT_COUNT = 35 };

enum { MiniConsoleTimeout = 6000, MainConsoleTimeout = 3000 };

#define PREFERRED_WINDOW_SIZE   QSize(840, 480)
#define PREFERRED_MIN_WIDTH     400
#define PREFERRED_MIN_HEIGHT    300

#define DEFAULT_TRANSLATORS \
( \
  TranslatorManager::OcnBit \
)

// - Focus -

void
MainWindow::onFocusedWidgetChanged(QWidget *w_old, QWidget *w_new)
{
  Q_UNUSED(w_old)
  Q_UNUSED(w_new)
//#ifdef Q_OS_WIN
//  // When EmbeddedPlayer's lineEdit cleared its focus.
//  if (!w_new && w_old == embeddedPlayer_->inputComboBox())
//    QtWin::sendMouseClick(QPoint(0, 0), Qt::LeftButton);
//#else
//  Q_UNUSED(w_old)
//  Q_UNUSED(w_new)
//#endif // Q_OS_WIN
}

bool
MainWindow::isEditing() const
{
  return inputLineHasFocus()
      && prefixLineHasFocus();
}

bool
MainWindow::inputLineHasFocus() const
{
  return mainPlayer_->inputComboBox()->hasFocus()
      || miniPlayer_->inputComboBox()->hasFocus()
      || embeddedPlayer_->inputComboBox()->hasFocus();
}

bool
MainWindow::prefixLineHasFocus() const
{
  return mainPlayer_->prefixComboBox()->hasFocus()
      || miniPlayer_->prefixComboBox()->hasFocus()
      || embeddedPlayer_->prefixComboBox()->hasFocus();
}

// - Constructions -

void
MainWindow::resetPlayer()
{
  DOUT("enter");
  Q_ASSERT(player_);
  player_->reset();

  player_->setKeyboardEnabled(true);  // default true
  player_->setMouseEnabled(true);     // default true
  //player_->setEncoding("UTF-8");      // default do nothing

  player_->setDownloadsLocation(G_PATH_DOWNLOADS);

#ifdef Q_OS_WIN
  player_->setMouseEventEnabled();
#endif // Q_OS_WIN

#ifdef Q_OS_MAC
  videoView_->showView();
  player_->setEmbeddedWindow(videoView_->view());
  QTimer::singleShot(0, videoView_, SLOT(hideView()));
  //videoView_->hideView();
#else
  setEmbeddedWindow(annotationView_->trackedWindow());
#endif // Q_OS_MAC
  player_->setFullScreen(false);

  Settings *settings = Settings::globalSettings();
  player_->setHue(settings->hue());
  player_->setContrast(settings->contrast());
  player_->setSaturation(settings->saturation());
  player_->setBrightness(settings->brightness());
  player_->setGamma(settings->gamma());

  static bool once = true;
  if (once) {
    once = false;
    installPlayerLogger();
  }
  DOUT("exit");
}

void
MainWindow::setupWindowStyle()
{
#ifdef Q_OS_WIN
  if (!AcUi::isAeroAvailable())
#endif // Q_OS_WIN
  { rippleEnabled_ = true; }

#ifdef WITH_WIN_DWM
  if(AcUi::globalInstance()->isAeroEnabled()) {
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);

    AcUi::globalInstance()->setWindowDwmEnabled(this, true);
    return;
  }
#endif // WITH_WIN_DWM
  //if (qss)
  //  w->setStyleSheet(SS_BACKGROUND_CLASS(MainWindow));
  if (!rippleEnabled_)
    AcUi::globalInstance()->setWindowBackground(this, true); // persistent = true
  setWindowOpacity(AC_WINDOW_OPACITY);
}

void
MainWindow::launch(const QStringList &args)
{
  const bool noargs = args.size() <= 1;

  qint64 gamePid = 0;
#ifdef AC_ENABLE_GAME
  if (noargs)
    gamePid = processFilter_->currentGamePid();
#endif // AC_ENABLE_GAME

  bool showLibrary = noargs && !gamePid &&
      Settings::globalSettings()->showLibrary() &&
      QFile::exists(G_PATH_MEDIADB);

  resize(showLibrary ? LIBRARY_WINDOW_SIZE : INIT_WINDOW_SIZE);

  if (showLibrary)
    showMainLibrary();
  DOUT("show");
  show();

  // Automatic login
  DOUT("automatic login");
  QString userName = AcSettings::globalSettings()->userName(),
          password = AcSettings::globalSettings()->password();
  if (userName.isEmpty() || password.isEmpty()) {
    userName = User::guest().name();
    password = User::guest().password();
  }
  //else
  //  w.checkInternetConnection();
  login(userName, password);

  switch (args.size()) {
  case 0:
  case 1:
#ifdef AC_ENABLE_GAME
    if (gamePid) {
      showMessageOnce(tr("Synchronizing ..."));
      showMessage(tr("synchronizing with running game") +
                  QString(" (pid = %1)").arg(QString::number(gamePid,16)));
      openProcessId(gamePid);
    }
#endif // AC_ENABLE_GAME
    break;
  case 2: openSource(args.last()); break;
  default:
    {
      QStringList l = args;
      l.removeFirst();
      openSources(l);
    }
  }
}

MainWindow::MainWindow(bool unique, QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f), disposed_(false), submit_(true),
    liveInterval_(DEFAULT_LIVE_INTERVAL),
    preferences_(nullptr), tray_(nullptr), magnifier_(nullptr),
    libraryWindow_(nullptr), libraryView_(nullptr), annotationAnalyticsView_(nullptr), userAnalyticsView_(nullptr),
    consoleDialog_(nullptr), annotationBrowser_(nullptr),
    dragPos_(BAD_POS), windowModeUpdateTime_(0), tokenType_(0), recentUrlLocked_(false), cancelContextMenu_(false),
    rippleEnabled_(false), rippleFilter_(nullptr), rippleTimer_(nullptr),
    preferredSubtitleTrack_(0), preferredAudioTrack_(0), preferredAudioChannel_(0),
    lastTrackedWindow_(0)
{
  DOUT("unique =" << unique);
#ifndef Q_OS_MAC
  // Broken on Windows 7
  //if (Settings::globalSettings()->isWindowOnTop())
  //  setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
#endif // Q_OS_MAC

//#ifdef Q_OS_MAC
//  setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
//#endif // Q_OS_MAC

  //setWindowTitle(TR(T_TITLE_PROGRAM));  // FIXME 5/12/2012: CRASH ON MAC orz
  setWindowIcon(QIcon(RC_IMAGE_APP)); // X11 require setting icon at runtime
  setContentsMargins(0, 0, 0, 0);

  setupWindowStyle();

  menuBar()->setVisible(Settings::globalSettings()->isMenuBarVisible());

  DOUT("createComponents");
  createComponents(unique);
#ifdef WITH_WIN_ATLAS
  createAtlas();
#endif // WITH_WIN_ATLAS

  DOUT("createConnections");
  createConnections();

  DOUT("createActions");
  createActions();

  DOUT("createMenus");
  createMenus();

  DOUT("createDockWindows");
  createDockWindows();

  // - Post behaviors
  DOUT("post behaviors");

  // Accept drag/drop behavior (default enabled on Windows but disabled on Mac)
  setAcceptDrops(true);

  // Receive mouseMoveEvent even no mouse button is clicked
  // Set this to enable resetAutoHide embeddedPlayer when mouse is moved.
  setMouseTracking(true);

  // Adjust EmbeddedPlayer to fit in OsdWindow.
  embeddedPlayer_->updateGeometry();

  // Global hooks
#ifdef WITH_WIN_HOOK
  HOOK->setMouseHookEnabled(true);
  //HOOK->setKeyboardHookEnabled(false);
  HOOK->setThreadsFilterEnabled(false);         // Use global hook.
  HOOK->setWindowsFilterEnabled(true);          // Only enable for specified window
  HOOK->setContextMenuEventEnabled(true);       // Listen to context menu event.
  HOOK->setDoubleClickEventEnabled(true);       // Listen to double click event.
  HOOK->setMouseMoveEventEscaped(true);         // Always receive mouse move event
  HOOK->installEventFilter(new HookEventForwarder(this));

  //connect(player_, SIGNAL(mediaOpen()), videoView_, SLOT(addToWindowsHook()));
  connect(player_, SIGNAL(playing()), videoView_, SLOT(addToWindowsHook()));
  connect(player_, SIGNAL(mediaClosed()), videoView_, SLOT(removeFromWindowsHook()));

  HOOK->start();
#endif // WITH_WIN_HOOK

#ifdef WITH_WIN_MOUSEHOOK
  MouseHook::globalInstance()->setEventListener(this);
  //MouseHook::globalInstance()->start();
#endif // WITH_WIN_MOUSEHOOK

  // Load settings
  Settings *settings = Settings::globalSettings();
  AcSettings *ac = AcSettings::globalSettings();

  submit_ = settings->isAutoSubmit();

  setSubtitleColor(settings->subtitleColor());

  recentUrlTitles_ = settings->recentTitles();
  recentFiles_ = settings->recentFiles();
  updateRecentMenu();

  recentPath_ = settings->recentPath();

  playPosHistory_ = settings->playPosHistory();
  subtitleHistory_ = settings->subtitleHistory();
  audioTrackHistory_= settings->audioTrackHistory();
  audioChannelHistory_= settings->audioChannelHistory();
  aspectRatioHistory_= settings->aspectRatioHistory();

  //recentGameEncodings_ = settings->gameEncodings();

  setTranslateEnabled(settings->isTranslateEnabled());

  embeddedPlayer_->setOnTop(settings->isEmbeddedPlayerOnTop());
  //embeddedCanvas_->setEnabled(settings->isPlayerLabelEnabled());

  annotationView_->setTranslateWordEnabled(settings->translateSelection());
  annotationView_->setItemCountLimited(settings->isAnnotationBandwidthLimited());
  annotationView_->setRenderHint(settings->annotationEffect());

  AnnotationSettings::globalSettings()->setScale(settings->annotationScale());
  AnnotationSettings::globalSettings()->setFullscreenScale(settings->annotationFullscreenScale());
  AnnotationSettings::globalSettings()->setOpacityFactor(settings->annotationOpacityFactor());
  AnnotationSettings::globalSettings()->setBackgroundOpacityFactor(settings->annotationBackgroundOpacityFactor());
  AnnotationSettings::globalSettings()->setOffset(settings->annotationOffset());
  AnnotationSettings::globalSettings()->setAvatarVisible(settings->isAnnotationAvatarVisible());
  AnnotationSettings::globalSettings()->setMetaVisible(settings->isAnnotationMetaVisible());
  AnnotationSettings::globalSettings()->setPreferFloat(settings->preferFloatAnnotation());
  AnnotationSettings::globalSettings()->setPreferTraditionalChinese(settings->preferTraditionalChinese());
  AnnotationSettings::globalSettings()->setOutlineColor(settings->annotationOutlineColor());
  AnnotationSettings::globalSettings()->setHighlightColor(settings->annotationHighlightColor());
  AnnotationSettings::globalSettings()->setSubtitleColor(settings->subtitleOutlineColor());
  AnnotationSettings::globalSettings()->setPositionResolution(settings->annotationPositionResolution());
  AnnotationSettings::globalSettings()->setSpeedup(settings->annotationSpeedup());
  AnnotationSettings::globalSettings()->setSubtitleOnTop(settings->isSubtitleOnTop());

  libraryView_->setAutoRun(settings->showLibrary());
  libraryView_->setViewIndex(settings->libraryView());

  dataServer_->setPreferCache(settings->preferLocalDatabase());

  annotationFilter_->setEnabled(settings->isAnnotationFilterEnabled());
  annotationFilter_->setBlockedTexts(settings->blockedKeywords());
  annotationFilter_->setBlockedUserAliases(settings->blockedUserNames());
  //annotationFilter_->setAnnotationCountHint(settings->annotationCountHint());
  {
    QSet<int> l = settings->annotationLanguages();
    annotationFilter_->setLanguages(l);

    toggleAnnotationLanguageToAnyAct_->setChecked(l.isEmpty());
    //toggleAnnotationLanguageToUnknownAct_->setChecked(l & Traits::UnknownLanguageBit);
    toggleAnnotationLanguageToEnglishAct_->setChecked(l.contains(Traits::English));
    toggleAnnotationLanguageToJapaneseAct_->setChecked(l.contains(Traits::Japanese));
    toggleAnnotationLanguageToChineseAct_->setChecked(l.contains(Traits::TraditionalChinese));
    toggleAnnotationLanguageToKoreanAct_->setChecked(l.contains(Traits::Korean));
    toggleAnnotationLanguageToFrenchAct_->setChecked(l.contains(Traits::French));
    toggleAnnotationLanguageToGermanAct_->setChecked(l.contains(Traits::German));
    toggleAnnotationLanguageToItalianAct_->setChecked(l.contains(Traits::Italian));
    toggleAnnotationLanguageToSpanishAct_->setChecked(l.contains(Traits::Spanish));
    toggleAnnotationLanguageToPortugueseAct_->setChecked(l.contains(Traits::Portuguese));
    toggleAnnotationLanguageToRussianAct_->setChecked(l.contains(Traits::Russian));
    invalidateAnnotationLanguages();
  }

  toggleSaveAnnotationFileAct_->setChecked(settings->isAnnotationFileSaved());

  player_->setBufferSaved(settings->isBufferedMediaSaved());

  {
    MrlResolverSettings *mrs = MrlResolverSettings::globalSettings();
    QString username, password;
    boost::tie(username, password) = ac->nicovideoAccount();
    if (!username.isEmpty() && !password.isEmpty())
      mrs->setNicovideoAccount(username, password);

    boost::tie(username, password) = ac->bilibiliAccount();
    if (!username.isEmpty() && !password.isEmpty())
      mrs->setBilibiliAccount(username, password);
  }

  nothingAfterFinishedAct_->setChecked(true);

  // Set browsed file.
  QString browsedFile;
  foreach (const QString &url, recentFiles_)
    if (isRemoteMrl(url)) {
      //browsedFile = url;
      break;
    } else if (!url.endsWith(".exe", Qt::CaseInsensitive) && QFile::exists(url)) {
      browsedFile = url;
      break;
    }
  if (browsedFile.isEmpty() && QFile::exists(G_PATH_DOWNLOADS)) {
    QDir dir(G_PATH_DOWNLOADS);
    QStringList l = dir.entryList(Player::supportedVideoFilters(), QDir::Files, QDir::Time);
    if (!l.isEmpty())
      browsedFile = dir.absolutePath() + "/" + l.last();
  }
  DOUT("browsedFile =" << browsedFile);
  if (!browsedFile.isEmpty()) {
    if (isRemoteMrl(browsedFile))
      setBrowsedUrl(browsedFile);
    else
      setBrowsedFile(browsedFile);
  }

#ifdef Q_OS_WIN
  //connect(annotationView_, SIGNAL(posChanged()), SLOT(ensureWindowOnTop()));
#endif // Q_OS_WIN

#ifdef Q_OS_MAC
  // Forward reseting player on Mac OS X
  if (!player_->isValid())
    resetPlayer();
#endif // Q_OS_MAC

  // Initial focus
  mainPlayer_->inputComboBox()->setFocus();

  // System tray icon
  if (tray_)
    tray_->show(); // FIXME: not working on mac?

  // Monitor clipboard
  clipboardMonitor_->setEnabled(true);

  //statusBar()->showMessage(tr("Ready"));

  setAutoPlayNext();

  gameLibrary_->load();
  mediaLibrary_->load();

  // Enable keyboard shortcuts
  //updateContextMenu();

  //grabGesture(Qt::PanGesture);
  grabGesture(Qt::SwipeGesture);
  grabGesture(Qt::PinchGesture);

  DOUT("create event filters");
  installEventFilters();

  installLogger();

  if (unique)
    appServer_->start();
}

void
MainWindow::installEventFilters()
{ installEventFilter(clickListener_); }

void
MainWindow::createComponents(bool unique)
{
  // Systemt tray icon
  if (unique && QSystemTrayIcon::isSystemTrayAvailable()) {
    tray_ = new Tray(this, this);
    connect(this, SIGNAL(trayToolTip(QString)), tray_, SLOT(setToolTip(QString)), Qt::QueuedConnection);
    connect(this, SIGNAL(progressMessageChanged(QString)), tray_, SLOT(setToolTip(QString)), Qt::QueuedConnection);
    connect(this, SIGNAL(trayMessage(QString,QString)), tray_, SLOT(showMessage(QString,QString)), Qt::QueuedConnection);
  }

  fadeAni_ = new FadeAnimation(this, "windowOpacity", this);

  // Utilities
  grabber_ = new Grabber(this);
  AC_CONNECT_MESSAGE(grabber_, this, Qt::AutoConnection);
  AC_CONNECT_WARNING(grabber_, this, Qt::AutoConnection);

  // Server agents
  server_ = new AnnotationServerAgent(this);

  // Player
  player_ = new Player(this);

  // Signal hub
  hub_ = new SignalHub(player_, this);

  // Logger
  logger_ = new EventLogger(this, player_, hub_, this);

  // Caches
  cache_ = new AnnotationDatabase(this);
  if (unique && !cache_->open(G_PATH_CACHEDB))
    cache_->clear();
  queue_ = new AnnotationDatabase(this);
  if (unique && !queue_->open(G_PATH_QUEUEDB))
    queue_->clear();

  // Library
  gameLibrary_ = new GameLibrary(G_PATH_GAMEDB, this);

  mediaLibrary_ = new MediaLibrary(G_PATH_MEDIADB, this);
  libraryView_ = new MainLibraryView(mediaLibrary_, this);

  // Data server
  dataServer_ = new DataServer(hub_, server_, cache_, queue_, this);

  // Data manager
  dataManager_ = new DataManager(this);

  // Clipboard monitor
  clipboardMonitor_ = new ClipboardMonitor(this);
  clipboardMonitor_->setEnabled(false);
  AC_CONNECT_MESSAGE(clipboardMonitor_, this, Qt::AutoConnection);

  // Mrl resolver
  mrlResolver_ = new MrlResolverManager(this);

  // Players and graphic views
  videoView_ = new VideoView(this);
  osdWindow_ = new OsdWindow(this);
  //windows_.append(osdWindow_);

  annotationView_ = new AnnotationGraphicsView(hub_, dataManager_, player_, videoView_, osdWindow_);
  annotationView_->setFullScreenView(osdWindow_);

  mainConsole_ = new MainConsole(annotationView_);
  mainConsole_->setAutoClearInterval(MainConsoleTimeout);

  miniConsole_ = new MiniConsole(annotationView_);
  miniConsole_->setAutoClearInterval(MiniConsoleTimeout);

  // MeCab
  mecab_ = new MeCabParser(this);

  // Rubber bands
  attractRubberBand_ = new QxCircularRubberBand(QRubberBand::Line, annotationView_);
  expelRubberBand_ = new QxCircularRubberBand(QRubberBand::Line, annotationView_);

  pauseRubberBand_ = new QxMouseRubberBand(QRubberBand::Rectangle, annotationView_);
  resumeRubberBand_ = new QxMouseRubberBand(QRubberBand::Rectangle, annotationView_);
  removeRubberBand_ = new QxMouseRubberBand(QRubberBand::Rectangle, annotationView_);

  pauseRubberBand_->setColor(Qt::cyan);
  removeRubberBand_->setColor(Qt::red);

  attractRubberBand_->setColor(Qt::cyan);
  expelRubberBand_->setColor(Qt::red);
  attractRubberBand_->setRadius(400);
  expelRubberBand_->setRadius(400);

  clickListener_ = new MouseClickEventListener(this);
  clickListener_->setInterval(
    CLICK_TIMEOUT
//#ifdef WITH_WIN_QTWIN
//    QtWin::getDoubleClickInterval() /2 +10 // +10 to ensure non-zero
//#else
//    DBLCLICK_TIMEOUT /2 +10 // +10 to ensure non-zero
//#endif // Q_OS_WIN
  );

//#ifdef WITH_WIN_DWM
//  {
//    QWidget *w = annotationView_->editor();
//    w->setParent(this, w->windowFlags()); // hot fix for dwm displaying issue
//    AcUi::globalInstance()->setWindowDwmEnabled(w->winId(), false);
//    AcUi::globalInstance()->setWindowDwmEnabled(w->winId(), true);
//  }
//#endif // WITH_WIN_DWM
//#ifdef Q_OS_MAC // remove background in annotation view
//  annotationView_->setStyleSheet(
//    SS_BEGIN(QWidget)
//      SS_TRANSPARENT
//    SS_END
//  );
//#endif // Q_OS_MAC
  annotationFilter_ = new AnnotationFilter(dataManager_, this);
  annotationView_->setFilter(annotationFilter_);

  //QLayout *layout = new QStackedLayout(osdWindow_);
  //layout->addWidget(annotationView_);
  //osdWindow_->setLayout(layout);
  osdWindow_->setEventForwarder(annotationView_);

  embeddedPlayer_ = new EmbeddedPlayerUi(hub_, player_, server_, dataManager_);
  embeddedCanvas_ = embeddedPlayer_->canvas();

//#ifdef Q_OS_MAC // remove background in Osd player
//  embeddedPlayer_->setStyleSheet(
//    SS_BEGIN(QWidget)
//      SS_TRANSPARENT
//    SS_END
//  );
//#endif // Q_OS_MAC

  // Show OSD window
  osdWindow_->showInOsdMode(); // this must go after all children of osdWindow_ are created
  embeddedPlayer_->setParent(osdWindow_); // Set parent after osdWindow_ become visible.
  embeddedPlayer_->setAutoHideEnabled();
  embeddedPlayer_->setContainerWidget(this);

  annotationView_->updateGeometry();
  annotationView_->show();

  connect(annotationView_, SIGNAL(sizeChanged(QSize)), mainConsole_, SLOT(resize(QSize)));
  connect(annotationView_, SIGNAL(sizeChanged(QSize)), miniConsole_, SLOT(resize(QSize)));
  mainConsole_->show();
  miniConsole_->show();

  mainPlayer_ = new MainPlayerUi(hub_, player_, server_, this);
  miniPlayer_ = new MiniPlayerUi(hub_, player_, server_, this);

  windows_.append(miniPlayer_);

//#ifdef Q_OS_MAC
//  videoView_->showView();
//  player_->setEmbeddedWindow(videoView_->view());
//  QTimer::singleShot(0, videoView_, SLOT(hideView()));
//#else
//  player_->embed(videoView_);
//#endif // Q_OS_MAC
//  player_->setFullScreen(false);

  // Translator
  translator_ = new TranslatorManager(this);

  extraTranslator_ = new TranslatorManager(this);
  connect(translator_, SIGNAL(servicesChanged(ulong)), extraTranslator_, SLOT(setServices(ulong)));
  extraTranslator_->setSynchronized(false);

  translator_->setServices(Settings::globalSettings()->translationServices());
  if (!translator_->hasServices())
    translator_->setServices(DEFAULT_TRANSLATORS);

  // Dialogs
  //userPanel_ = new UserPanel(this);
  //userPanel_->hide(); // TODO!!!!!!

#ifdef AC_ENABLE_GAME
  messageHandler_ = new MessageHandler(this);
  syncView_ = new SyncView(messageHandler_, this);
  windows_.append(syncView_);

  syncView_->setThreadViewVisible(false);
  syncView_->disableThreadView();

  //recentThreadView_ = new ThreadView(this);
  //windows_.append(recentThreadView_);

  AC_CONNECT_MESSAGES(syncView_, this, Qt::AutoConnection);
  //AC_CONNECT_MESSAGES(recentThreadView_, this, Qt::AutoConnection);

  processFilter_ = new ProcessFilter(gameLibrary_, this);

#endif // AC_ENABLE_GAME

#ifdef WITH_LIB_DOLL
  doll_ = new Doll(this);
  connect(doll_, SIGNAL(said(QString)), SLOT(respond(QString)));
#endif // WITH_LIB_DOLL

  holdTimer_ = new QTimer(this);
  holdTimer_->setInterval(HOLD_TIMEOUT);
  holdTimer_->setSingleShot(true);
  connect(holdTimer_, SIGNAL(timeout()), SLOT(toggleMagnifierVisible()));

  windowStaysOnTopTimer_ = new QTimer(this);
  windowStaysOnTopTimer_->setInterval(G_WINDOWONTOP_TIMEOUT);
  connect(windowStaysOnTopTimer_, SIGNAL(timeout()), SLOT(setWindowOnTop()));

  saveSettingsTimer_ = new QTimer();
  saveSettingsTimer_->setInterval(30 * 1000); // save settings after 30 seconds
  saveSettingsTimer_->setSingleShot(true);
  connect(saveSettingsTimer_, SIGNAL(timeout()), SLOT(saveSettings()));

  //autoHideCursorTimer_ = new QTimer(this);
  //autoHideCursorTimer_->setInterval(G_AUTOHIDE_TIMEOUT);
  //autoHideCursorTimer_->setSingleShot(true);
  //connect(autoHideCursorTimer_, SIGNAL(timeout()), SLOT(autoHideCursor()));

  //liveTimer_ = new QTimer(this);
  //connect(liveTimer_, SIGNAL(timeout()), SLOT(updateLiveAnnotations()));

  loadSubtitlesTimer_ = new QxCountdownTimer;
  loadSubtitlesTimer_->setInterval(3000); // 3 seconds

  resumePlayTimer_ = new QxCountdownTimer;
  resumePlayTimer_->setInterval(3000); // 3 seconds

  resumeSubtitleTimer_ = new QxCountdownTimer;
  resumeSubtitleTimer_->setInterval(3000); // 3 seconds

  resumeAudioTrackTimer_ = new QxCountdownTimer;
  resumeAudioTrackTimer_->setInterval(3000); // 3 seconds

  resumeAudioChannelTimer_ = new QxCountdownTimer;
  resumeAudioChannelTimer_->setInterval(3000); // 3 seconds

  resumeAspectRatioTimer_ = new QxCountdownTimer;
  resumeAspectRatioTimer_->setInterval(3000); // 3 seconds

  navigationTimer_ = new QTimer(this);
  navigationTimer_->setInterval(5000); // 5 seconds
  navigationTimer_->setSingleShot(true);
  connect(navigationTimer_, SIGNAL(timeout()), SLOT(enableNavigation()));

  browserDelegate_ = new AcBrowser(this);
  downloaderDelegate_ = new AcDownloader(this);
  translatorDelegate_ = new AcTranslator(this);
  updaterDelegate_ = new AcUpdater(this);
  appServer_ = new AcPlayerServer(this);

  annotationDownloader_ = new AnnotationDownloader(this);
  annotationDownloader_->setDownloadsLocation(QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));
}

void
MainWindow::installPlayerLogger()
{
  connect(player_, SIGNAL(buffering()), logger_, SLOT(startLogUntilPlaying()), Qt::QueuedConnection);
  connect(player_, SIGNAL(stopped()), logger_, SLOT(stopLogUntilPlaying()), Qt::QueuedConnection);
  //connect(player_, SIGNAL(titleIdChanged(int)), logger_, SLOT(logTitleChanged()));
  connect(player_, SIGNAL(mediaChanged()), logger_, SLOT(logMediaChanged()), Qt::QueuedConnection);
  connect(player_, SIGNAL(mediaClosed()), logger_, SLOT(logMediaClosed()), Qt::QueuedConnection);
  connect(player_, SIGNAL(volumeChanged()), logger_, SLOT(logVolumeChanged()), Qt::QueuedConnection);
  connect(player_, SIGNAL(subtitleChanged()), logger_, SLOT(logSubtitleChanged()), Qt::QueuedConnection);
  connect(player_, SIGNAL(audioTrackChanged()), logger_, SLOT(logAudioTrackChanged()), Qt::QueuedConnection);
  connect(player_, SIGNAL(opening()), logger_, SLOT(logOpening()), Qt::QueuedConnection);
  connect(player_, SIGNAL(buffering()), logger_, SLOT(logBuffering()), Qt::QueuedConnection);
  connect(player_, SIGNAL(playing()), logger_, SLOT(logPlaying()), Qt::QueuedConnection);
  connect(player_, SIGNAL(paused()), logger_, SLOT(logPaused()), Qt::QueuedConnection);
  connect(player_, SIGNAL(stopped()), logger_, SLOT(logStopped()), Qt::QueuedConnection);
  connect(player_, SIGNAL(errorEncountered()), logger_, SLOT(logPlayerError()), Qt::QueuedConnection);
  connect(player_, SIGNAL(errorEncountered()), logger_, SLOT(stopLogUntilPlaying()), Qt::QueuedConnection);
  connect(player_, SIGNAL(trackNumberChanged(int)), logger_, SLOT(logTrackNumberChanged(int)), Qt::QueuedConnection);
  connect(player_, SIGNAL(rateChanged(qreal)), logger_, SLOT(logPlayRateChanged(qreal)), Qt::QueuedConnection);
  connect(player_, SIGNAL(aspectRatioChanged(QString)), logger_, SLOT(logAspectRatioChanged(QString)), Qt::QueuedConnection);
  connect(player_, SIGNAL(contrastChanged(qreal)), logger_, SLOT(logContrastChanged(qreal)), Qt::QueuedConnection);
  connect(player_, SIGNAL(brightnessChanged(qreal)), logger_, SLOT(logBrightnessChanged(qreal)), Qt::QueuedConnection);
  connect(player_, SIGNAL(hueChanged(int)), logger_, SLOT(logHueChanged(int)), Qt::QueuedConnection);
  connect(player_, SIGNAL(saturationChanged(qreal)), logger_, SLOT(logSaturationChanged(qreal)), Qt::QueuedConnection);
  connect(player_, SIGNAL(gammaChanged(qreal)), logger_, SLOT(logGammaChanged(qreal)), Qt::QueuedConnection);
  connect(player_, SIGNAL(audioChannelChanged(int)), logger_, SLOT(logAudioChannelChanged(int)), Qt::QueuedConnection);
  connect(player_, SIGNAL(audioDelayChanged(qint64)), logger_, SLOT(logAudioDelayChanged(qint64)), Qt::QueuedConnection);
}

void
MainWindow::installLogger()
{
  connect(cache_, SIGNAL(cleared()), logger_, SLOT(logCacheCleared()), Qt::QueuedConnection);

  connect(server_, SIGNAL(connectedChanged(bool)), logger_, SLOT(logInternetConnectionChanged(bool)), Qt::QueuedConnection);
  connect(server_, SIGNAL(loginRequested(QString)), logger_, SLOT(logLoginRequested(QString)), Qt::QueuedConnection);
  connect(server_, SIGNAL(loginSucceeded(QString)), logger_, SLOT(logLoginSucceeded(QString)), Qt::QueuedConnection);
  connect(server_, SIGNAL(loginFailed(QString)), logger_, SLOT(logLoginFailed(QString)), Qt::QueuedConnection);
  connect(server_, SIGNAL(logoutRequested()), logger_, SLOT(logLogoutRequested()), Qt::QueuedConnection);
  connect(server_, SIGNAL(logoutFinished()), logger_, SLOT(logLogoutFinished()), Qt::QueuedConnection);
  connect(server_, SIGNAL(unknownError()), logger_, SLOT(logServerAgentUnknownError()), Qt::QueuedConnection);
  connect(server_, SIGNAL(connectionError()), logger_, SLOT(logServerAgentConnectionError()), Qt::QueuedConnection);
  connect(server_, SIGNAL(serverError()), logger_, SLOT(logServerAgentServerError()), Qt::QueuedConnection);
  connect(server_, SIGNAL(error404()), logger_, SLOT(logServerAgentError404()), Qt::QueuedConnection);

  connect(AnnotationSettings::globalSettings(), SIGNAL(scaleChanged(qreal)), logger_, SLOT(logAnnotationScaleChanged(qreal)));
  connect(AnnotationSettings::globalSettings(), SIGNAL(fullscreenScaleChanged(qreal)), logger_, SLOT(logAnnotationFullscreenScaleChanged(qreal)));
  connect(AnnotationSettings::globalSettings(), SIGNAL(opacityFactorChanged(int)), logger_, SLOT(logAnnotationOpacityFactorChanged(int)));
  connect(AnnotationSettings::globalSettings(), SIGNAL(backgroundOpacityFactorChanged(int)), logger_, SLOT(logAnnotationBackgroundOpacityFactorChanged(int)));
  connect(AnnotationSettings::globalSettings(), SIGNAL(speedupChanged(qreal)), logger_, SLOT(logAnnotationSpeedupChanged(qreal)));
  connect(AnnotationSettings::globalSettings(), SIGNAL(rotationChanged(qreal)), logger_, SLOT(logAnnotationRotationChanged(qreal)));
  connect(AnnotationSettings::globalSettings(), SIGNAL(offsetChanged(int)), logger_, SLOT(logAnnotationOffsetChanged(int)));
  connect(AnnotationSettings::globalSettings(), SIGNAL(preferFloatChanged(bool)), logger_, SLOT(logPreferFloatAnnotationChanged(bool)));
  connect(AnnotationSettings::globalSettings(), SIGNAL(subtitleOnTopChanged(bool)), logger_, SLOT(logSubtitleOnTopChanged(bool)));

  connect(annotationDownloader_, SIGNAL(fileSaved(QString)), logger_, SLOT(logFileSaved(QString)), Qt::QueuedConnection);

  connect(annotationView_, SIGNAL(selectedUserIds(QList<qint64>)), logger_, SLOT(logSelectedUserIds(QList<qint64>)));
  connect(annotationView_, SIGNAL(trackedWindowDestroyed()), logger_, SLOT(logTrackedWindowDestroyed()));
  connect(annotationView_, SIGNAL(hoveredItemPausedChanged(bool)), logger_, SLOT(logPauseHoveredAnnotations(bool)));
  connect(annotationView_, SIGNAL(hoveredItemResumedChanged(bool)), logger_, SLOT(logResumeHoveredAnnotations(bool)));
  connect(annotationView_, SIGNAL(hoveredItemRemovedChanged(bool)), logger_, SLOT(logRemoveHoveredAnnotations(bool)));
  connect(annotationView_, SIGNAL(nearbyItemExpelledChanged(bool)), logger_, SLOT(logExpelNearbyAnnotations(bool)));
  connect(annotationView_, SIGNAL(nearbyItemAttractedChanged(bool)), logger_, SLOT(logAttractNearbyAnnotations(bool)));
  connect(annotationView_, SIGNAL(itemCountLimitedChanged(bool)), logger_, SLOT(logAnnotationCountLimitedChanged(bool)));
  connect(annotationView_, SIGNAL(annotationSkipped()), logger_, SLOT(logAnnotationSkipped()));

  connect(embeddedCanvas_, SIGNAL(enabledChanged(bool)), logger_, SLOT(logCanvasEnabled(bool)));

  connect(dataServer_, SIGNAL(preferCacheChanged(bool)), logger_, SLOT(logPreferLocalDatabaseChanged(bool)), Qt::QueuedConnection);

  connect(TextCodecManager::globalInstance(), SIGNAL(encodingChanged(QString)), logger_, SLOT(logTextEncodingChanged(QString)), Qt::QueuedConnection);
}

void
MainWindow::createConnections()
{
#ifdef Q_OS_WIN
  connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), SLOT(onFocusedWidgetChanged(QWidget*, QWidget*)));
#endif // Q_OS_WIN

  // Event filters
  connect(clickListener_, SIGNAL(triggered()), SLOT(playPauseMedia()));

  // Settings
  connect(AcSettings::globalSettings(), SIGNAL(nicovideoAccountChanged(QString,QString)),
          MrlResolverSettings::globalSettings(), SLOT(setNicovideoAccount(QString,QString)));
  connect(AcSettings::globalSettings(), SIGNAL(bilibiliAccountChanged(QString,QString)),
          MrlResolverSettings::globalSettings(), SLOT(setBilibiliAccount(QString,QString)));

  // Library
  connect(libraryView_, SIGNAL(openRequested(QString)), SLOT(openSource(QString)));
  connect(libraryView_, SIGNAL(toggled()), SLOT(toggleMediaLibrary()));
  connect(libraryView_, SIGNAL(showGameRequested(QString)), SLOT(showGameWithDigest(QString)));
#ifdef AC_ENABLE_GAME
  connect(libraryView_, SIGNAL(syncGameRequested()), SLOT(openCurrentGame()));
#endif // AC_ENABLE_GAME

  // Player

  connect(this, SIGNAL(posChanged()), embeddedPlayer_, SLOT(hide()));

  //connect(this, SIGNAL(seeked()), SLOT(checkReachEnd()));
  connect(player_, SIGNAL(timeChanged()), SLOT(checkReachEnd()));
  connect(player_, SIGNAL(titleIdChanged(int)), SLOT(invalidateToken()));
  connect(player_, SIGNAL(errorEncountered()), SLOT(handlePlayerError()));
  connect(player_, SIGNAL(trackNumberChanged(int)), SLOT(invalidateMediaAndPlay()));
  connect(player_, SIGNAL(endReached()), SLOT(autoPlayNext()));
  connect(player_, SIGNAL(mediaTitleChanged(QString)), SLOT(setWindowTitle(QString)));
  connect(player_, SIGNAL(fileSaved(QString)), SLOT(signFile(QString)), Qt::QueuedConnection);
  connect(player_, SIGNAL(downloadProgress(qint64,qint64)), SLOT(updateDownloadProgress(qint64,qint64)));
  connect(player_, SIGNAL(fileSaved(QString)), AcLocationManager::globalInstance(), SLOT(createDownloadsLocation()), Qt::QueuedConnection);

  // Locations
  //connect(AcLocationManager::globalInstance(), SIGNAL(downloadsLocationChanged(QString)), annotationDownloader_, SLOT(setDownloadsLocation(QString)));
  connect(AcLocationManager::globalInstance(), SIGNAL(downloadsLocationChanged(QString)), player_, SLOT(setDownloadsLocation(QString)));

  // Resume
  connect(player_, SIGNAL(stopping()), SLOT(rememberAspectRatio()));
  connect(player_, SIGNAL(stopping()), SLOT(rememberPlayPos()));
  connect(player_, SIGNAL(stopping()), SLOT(rememberSubtitle()));
  connect(player_, SIGNAL(stopping()), SLOT(rememberAudioTrack()));
  connect(player_, SIGNAL(stopping()), SLOT(rememberAudioChannel()));
  connect(player_, SIGNAL(endReached()), SLOT(rememberAspectRatio()));
  connect(player_, SIGNAL(endReached()), SLOT(rememberPlayPos()));
  connect(player_, SIGNAL(endReached()), SLOT(rememberSubtitle()));
  connect(player_, SIGNAL(endReached()), SLOT(rememberAudioTrack()));
  connect(player_, SIGNAL(endReached()), SLOT(rememberAudioChannel()));

  if (rippleEnabled_) {
    connect(player_, SIGNAL(opening()), SLOT(disableRipple()));
    connect(player_, SIGNAL(playing()), SLOT(disableRipple()));
    connect(player_, SIGNAL(stopped()), SLOT(enableRipple()));
    connect(player_, SIGNAL(mediaClosed()), SLOT(enableRipple()));
  }

  connect(player_, SIGNAL(mediaChanged()), SLOT(saveSettingsLater()));

  connect(loadSubtitlesTimer_, SIGNAL(timeout()), SLOT(loadSubtitles()));
  connect(resumePlayTimer_, SIGNAL(timeout()), SLOT(resumePlayPos()));
  connect(resumeSubtitleTimer_, SIGNAL(timeout()), SLOT(resumeSubtitle()));
  connect(resumeAudioTrackTimer_, SIGNAL(timeout()), SLOT(resumeAudioTrack()));
  connect(resumeAudioChannelTimer_, SIGNAL(timeout()), SLOT(resumeAudioChannel()));
  connect(resumeAspectRatioTimer_, SIGNAL(timeout()), SLOT(resumeAspectRatio()));

  // Hub
  connect(hub_, SIGNAL(playModeChanged(SignalHub::PlayMode)), SLOT(updatePlayMode()));
  connect(hub_, SIGNAL(playerModeChanged(SignalHub::PlayerMode)), SLOT(updatePlayerMode()));
  connect(hub_, SIGNAL(tokenModeChanged(SignalHub::TokenMode)), SLOT(updateTokenMode()));
  connect(hub_, SIGNAL(windowModeChanged(SignalHub::WindowMode)), SLOT(updateWindowMode()));
  connect(hub_, SIGNAL(opened()),SLOT(open()));
  connect(hub_, SIGNAL(played()),SLOT(play()));
  connect(hub_, SIGNAL(paused()),SLOT(pause()));
  connect(hub_, SIGNAL(stopped()),SLOT(stop()));

  // Player UI

  #define CONNECT(_playerui) \
    connect(_playerui, SIGNAL(textEntered(QString)), SLOT(eval(QString))); \
    connect(_playerui, SIGNAL(loginRequested()), SLOT(showLogin())); \
    connect(_playerui, SIGNAL(showPositionPanelRequested()), SLOT(showSeek())); \
    connect(_playerui->inputComboBox()->lineEdit(), SIGNAL(textChanged(QString)), SLOT(syncInputLineText(QString))); \
    connect(_playerui->prefixComboBox()->lineEdit(), SIGNAL(textChanged(QString)), SLOT(syncPrefixLineText(QString))); \
    connect(_playerui->previousButton(), SIGNAL(clicked()), SLOT(previous())); \
    connect(_playerui->nextButton(), SIGNAL(clicked()), SLOT(next())); \
    connect(_playerui->toggleTraceWindowButton(), SIGNAL(clicked(bool)), SLOT(setWindowTrackingEnabled(bool))); \
    connect(_playerui->toggleTraceWindowButton(), SIGNAL(clicked(bool)), SLOT(updateTraceButtons())); \
    connect(_playerui, SIGNAL(updateMenuRequested()), SLOT(updateContextMenu())); \
    connect(this, SIGNAL(downloadProgressUpdated()), \
            _playerui, SLOT(updatePositionSlider()), Qt::QueuedConnection);

    CONNECT(mainPlayer_)
    CONNECT(miniPlayer_)
    CONNECT(embeddedPlayer_)
  #undef CONNECT

  connect(embeddedPlayer_, SIGNAL(updateMenuRequested()), SLOT(updateContextMenu()));
  connect(embeddedPlayer_->progressButton(), SIGNAL(clicked()), SLOT(clickProgressButton()));
  connect(this, SIGNAL(progressMessageChanged(QString)), embeddedPlayer_, SLOT(setProgressMessage(QString)), Qt::QueuedConnection);

  connect(this, SIGNAL(openProcessRequested()), SLOT(openProcess()), Qt::QueuedConnection);

  // invalidateMediaAndPlay
  connect(this, SIGNAL(playMediaRequested()), SLOT(playMedia()), Qt::QueuedConnection);
  connect(this, SIGNAL(removeAnnotationsFromAnnotationViewRequested()), annotationView_, SLOT(removeAnnotations()), Qt::QueuedConnection);

  // Queued connections
  connect(this, SIGNAL(response(QString)), SLOT(respond(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(said(QString,QString)), SLOT(say(QString,QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(showTextRequested(QString)), SLOT(showText(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(windowTitleSuffixToChange(QString)), SLOT(setWindowTitleSuffix(QString)), Qt::QueuedConnection);

 #ifndef Q_OS_LINUX
  connect(this, SIGNAL(windowMaximized()), SLOT(maximizedToFullScreen()), Qt::QueuedConnection);
 #endif // !Q_OS_LINUX
  connect(this, SIGNAL(openAnnotationUrlRequested(QString)), SLOT(openAnnotationUrl(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(openAnnotationFileRequested(QString)), SLOT(openAnnotationFile(QString)), Qt::QueuedConnection);

  // - Messages -
  AC_CONNECT_MESSAGES(this, this, Qt::QueuedConnection);
  connect(this, SIGNAL(messageOnce(QString)), SLOT(showMessageOnce(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(errorMessageOnce(QString)), SLOT(showErrorOnce(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(warningOnce(QString)), SLOT(warnOnce(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(notificationOnce(QString)), SLOT(notifyOnce(QString)), Qt::QueuedConnection);

  AC_CONNECT_MESSAGES(DownloaderController::globalController(), this, Qt::QueuedConnection);

  AC_CONNECT_MESSAGE(hub_, this, Qt::QueuedConnection);

  AC_CONNECT_MESSAGE(player_, this, Qt::QueuedConnection);
  AC_CONNECT_ERROR(player_, this, Qt::QueuedConnection);
  AC_CONNECT_WARNING(player_, this, Qt::QueuedConnection);

  AC_CONNECT_MESSAGE(annotationView_, this, Qt::AutoConnection);
  AC_CONNECT_WARNING(annotationView_, this, Qt::AutoConnection);

  AC_CONNECT_MESSAGE(dataServer_, this, Qt::QueuedConnection);
  AC_CONNECT_WARNING(dataServer_, this, Qt::QueuedConnection);

  AC_CONNECT_MESSAGE(annotationDownloader_, this, Qt::QueuedConnection);
  AC_CONNECT_WARNING(annotationDownloader_, this, Qt::QueuedConnection);
  AC_CONNECT_ERROR(annotationDownloader_, this, Qt::QueuedConnection);

  connect(translator_, SIGNAL(errorMessage(QString)), SLOT(warn(QString)), Qt::QueuedConnection);
  connect(extraTranslator_, SIGNAL(errorMessage(QString)), SLOT(warn(QString)), Qt::QueuedConnection);

  // Annotation graphics view
  connect(this, SIGNAL(addAnnotationsRequested(AnnotationList)),
          annotationView_, SLOT(addAnnotations(AnnotationList)),
          Qt::QueuedConnection);
  connect(this, SIGNAL(addAndShowAnnotationRequested(Annotation)),
          annotationView_, SLOT(addAndShowAnnotation(Annotation)),
          Qt::QueuedConnection);
  connect(this, SIGNAL(setAnnotationsRequested(AnnotationList)),
          annotationView_, SLOT(setAnnotations(AnnotationList)),
          Qt::QueuedConnection);
  connect(this, SIGNAL(appendAnnotationsRequested(AnnotationList)),
          annotationView_, SLOT(appendAnnotations(AnnotationList)),
          Qt::QueuedConnection);
  connect(this, SIGNAL(showAnnotationRequested(Annotation)),
          annotationView_, SLOT(showAnnotation(Annotation)),
          Qt::QueuedConnection);
  connect(this, SIGNAL(showAnnotationOnceRequested(Annotation)),
          annotationView_, SLOT(showAnnotationOnce(Annotation)),
          Qt::QueuedConnection);

  connect(annotationView_, SIGNAL(sizeChanged(QSize)), mainConsole_, SLOT(setAreaSize(QSize)));

  connect(annotationView_, SIGNAL(searchRequested(int,QString)), SLOT(searchWithEngine(int,QString)));
  connect(annotationView_, SIGNAL(traditionalChineseRequested(QString)), SLOT(showTraditionalChinese(QString)));
  connect(annotationView_, SIGNAL(translateTextRequested(QString)), SLOT(translateUsingTranslator(QString)));
  connect(annotationView_, SIGNAL(translateWordRequested(QString)), SLOT(translateUsingTranslator(QString)));

  connect(annotationView_, SIGNAL(annotationTextSubmitted(QString)), SLOT(submitText(QString)));

  // - Rubber band -
  connect(pauseRubberBand_, SIGNAL(selected(QRect)), SLOT(pauseAnnotationsAt(QRect)));
  connect(resumeRubberBand_, SIGNAL(selected(QRect)), SLOT(resumeAnnotationsAt(QRect)));
  connect(removeRubberBand_, SIGNAL(selected(QRect)), SLOT(removeAnnotationsAt(QRect)));

  // Cast
  connect(annotationView_, SIGNAL(userBlessedWithId(qint64)), SLOT(blessUserWithId(qint64)));
  connect(annotationView_, SIGNAL(userCursedWithId(qint64)), SLOT(curseUserWithId(qint64)));
  connect(annotationView_, SIGNAL(userBlockedWithId(qint64)), SLOT(blockUserWithId(qint64)));

  connect(annotationView_, SIGNAL(userBlockedWithAlias(QString)), annotationFilter_, SLOT(addBlockedUserAlias(QString)));

  connect(annotationView_, SIGNAL(annotationBlessedWithId(qint64)), SLOT(blessAnnotationWithId(qint64)));
  connect(annotationView_, SIGNAL(annotationCursedWithId(qint64)), SLOT(curseAnnotationWithId(qint64)));
  connect(annotationView_, SIGNAL(annotationBlockedWithId(qint64)), SLOT(blockAnnotationWithId(qint64)));

  connect(annotationView_, SIGNAL(annotationBlockedWithText(QString)), annotationFilter_, SLOT(addBlockedText(QString)));

  connect(annotationView_, SIGNAL(selectedUserId(qint64)), this, SLOT(showUserAnalytics(qint64)));

  connect(annotationView_, SIGNAL(resumed()), embeddedPlayer_->canvas(), SLOT(clearUserIds()));
  connect(annotationView_, SIGNAL(selectedUserIds(QList<qint64>)), embeddedPlayer_->canvas(), SLOT(setUserIds(QList<qint64>)));

  connect(embeddedPlayer_, SIGNAL(visibleChanged(bool)), SLOT(updateAnnotationMetaVisible()));
  connect(player_, SIGNAL(playing()), SLOT(updateAnnotationMetaVisible()));
  connect(player_, SIGNAL(paused()), SLOT(updateAnnotationMetaVisible()));
  connect(player_, SIGNAL(stopped()), SLOT(updateAnnotationMetaVisible()));

  // Data manager
  //connect(dataManager_, SIGNAL(aliasesChanged(AliasList)), SLOT(openAnnotationUrlFromAliases(AliasList)));

  connect(dataManager_, SIGNAL(aliasRemoved(Alias)), SLOT(removeAliasAnnotationFile(Alias)));
  connect(dataManager_, SIGNAL(aliasRemovedWithId(qint64)), dataServer_, SLOT(deleteAliasWithId(qint64)));
  connect(dataManager_, SIGNAL(annotationRemovedWithId(qint64)), dataServer_, SLOT(deleteAnnotationWithId(qint64)));
  connect(dataManager_, SIGNAL(annotationTextUpdatedWithId(QString,qint64)), dataServer_, SLOT(updateAnnotationTextWithId(QString,qint64)));

  connect(annotationView_, SIGNAL(annotationAdded(Annotation)), dataManager_, SLOT(addAnnotation(Annotation)));
  connect(annotationView_, SIGNAL(annotationsRemoved()), dataManager_, SLOT(removeAnnotations()));

  connect(annotationView_, SIGNAL(annotationsRemoved()), SLOT(clearAnnotationFiles()));
  connect(annotationView_, SIGNAL(annotationsRemoved()), SLOT(clearAnnotationUrls()));

  connect(annotationView_, SIGNAL(annotationDeletedWithId(qint64)), dataManager_, SLOT(removeAnnotationWithId(qint64)));

  connect(annotationView_, SIGNAL(annotationTextUpdatedWithId(QString,qint64)), dataManager_, SLOT(updateAnnotationTextWithId(QString,qint64)));

  // Forward drag/drop event
  //connect(tokenView_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  //connect(tokenView_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  //connect(tokenView_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  //connect(tokenView_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));

#ifdef AC_ENABLE_GAME
  //connect(syncView_->tokenView(), SIGNAL(aliasSubmitted(Alias)), SLOT(submitAlias(Alias)));

  //connect(syncView_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  //connect(syncView_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  //connect(syncView_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  //connect(syncView_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));
#endif // AC_ENABLE_GAME


  //connect(annotationBrowser_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  //connect(annotationBrowser_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  //connect(annotationBrowser_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  //connect(annotationBrowser_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));

  connect(miniPlayer_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  connect(miniPlayer_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  connect(miniPlayer_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  connect(miniPlayer_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));

  //connect(miniPlayer_, SIGNAL(togglePlayModeRequested()), hub_, SLOT(toggleFullScreenWindowMode()));
  //connect(miniPlayer_, SIGNAL(toggleMiniModeRequested()), hub_, SLOT(toggleMiniPlayerMode()));

  PlayerPanel *players[] = { mainPlayer_, miniPlayer_, embeddedPlayer_ };
  BOOST_FOREACH(PlayerPanel *p, players) {
    connect(p, SIGNAL(toggleLibraryRequested()), SLOT(toggleLibrary()));
  }

  // Live mode
  //connect(player_, SIGNAL(mediaChanged()), this, SLOT(stopLiveMode()));
  //connect(player_, SIGNAL(mediaClosed()), this, SLOT(stopLiveMode()));
  //connect(player_, SIGNAL(paused()), this, SLOT(stopLiveMode()));
  //connect(player_, SIGNAL(stopped()), this, SLOT(stopLiveMode()));
  //connect(this, SIGNAL(seeked()), this, SLOT(stopLiveMode()));

  // Sync mode
  connect(player_, SIGNAL(mediaChanged()), hub_, SLOT(stopSyncPlayMode()));
  connect(player_, SIGNAL(mediaClosed()), hub_, SLOT(stopSyncPlayMode()));
  connect(player_, SIGNAL(paused()), hub_, SLOT(stopSyncPlayMode()));
  connect(player_, SIGNAL(stopped()), hub_, SLOT(stopSyncPlayMode()));
  connect(this, SIGNAL(seeked()), hub_, SLOT(stopSyncPlayMode()));

  // Tracked window
  connect(annotationView_, SIGNAL(trackedWindowChanged(WId)), embeddedPlayer_, SLOT(setContainerWindow(WId)));

#ifdef Q_OS_WIN
  connect(annotationView_, SIGNAL(trackedWindowChanged(WId)), SLOT(setEmbeddedWindow(WId)));
#endif // Q_OS_WIN
  connect(annotationView_, SIGNAL(trackedWindowChanged(WId)), SLOT(updateTraceButtons()));

  connect(annotationView_, SIGNAL(trackedWindowChanged(WId)), SLOT(invalidatePlayerMode()));
  connect(annotationView_, SIGNAL(trackedWindowChanged(WId)), SLOT(updateOsdWindowOnTop()));
  connect(annotationView_, SIGNAL(trackedWindowDestroyed()), hub_, SLOT(stop()));

  connect(annotationView_, SIGNAL(fullScreenModeChanged(bool)), embeddedPlayer_, SLOT(setFullScreenMode(bool)));

  // Synchronize text edit
  //#define SYNC(_playerui1, _playerui2)
  //  connect(_playerui1->lineEdit(), SIGNAL(textChanged(QString)), _playerui2->lineEdit(), SLOT(setText(QString)));
  //  connect(_playerui2->lineEdit(), SIGNAL(textChanged(QString)), _playerui1->lineEdit(), SLOT(setText(QString)));
  //SYNC(mainPlayer_, miniPlayer_)
  //SYNC(mainPlayer_, embeddedPlayer_)
  //SYNC(miniPlayer_, embeddedPlayer_)
  //#undef SYNC

  //connect(server_, SIGNAL(annotationsReceived(AnnotationList)),
  //        annotationView_, SLOT(addAnnotations(AnnotationList)));
  //connect(server_, SIGNAL(tokensReceived(TokenList)),
  //        this, SLOT(processTokens(TokenList)));

  connect(this, SIGNAL(playingFinished()), SLOT(promptShutdown()));
  connect(this, SIGNAL(playingFinished()), SLOT(promptSleep()));

#ifdef Q_OS_MAC
  connect(player_, SIGNAL(playing()), SLOT(showVideoViewIfAvailable()));
  connect(player_, SIGNAL(stopped()), videoView_, SLOT(hideView()));
#endif // Q_OS_MAC

#ifdef Q_OS_WIN
  //if (!QtWin::isWindowsVistaOrLater())
#endif // Q_OS_WIN
  {
    connect(player_, SIGNAL(playing()), SLOT(disableWindowTransparency()));
    connect(player_, SIGNAL(stopped()), SLOT(enableWindowTransparency()));
  }

  // Clipboard
  connect(clipboardMonitor_, SIGNAL(annotationUrlEntered(QString)), SLOT(enterAnnotationUrl(QString)));
  connect(clipboardMonitor_, SIGNAL(mediaUrlEntered(QString)), SLOT(enterMediaUrl(QString)));

  // Language:
  //connect(Localizer::globalInstance(), SIGNAL(localeChanged()), SLOT(invalidateAppLanguage()));

  // Translator:
  connect(translator_, SIGNAL(translatedByRomaji(QString)), SLOT(showRomajiTranslation(QString)));
  connect(translator_, SIGNAL(translatedByMicrosoft(QString)), SLOT(showMicrosoftTranslation(QString)));
  connect(translator_, SIGNAL(translatedByGoogle(QString)), SLOT(showGoogleTranslation(QString)));
  connect(translator_, SIGNAL(translatedByYahoo(QString)), SLOT(showYahooTranslation(QString)));
  connect(translator_, SIGNAL(translatedByFresheye(QString)), SLOT(showFresheyeTranslation(QString)));
  connect(translator_, SIGNAL(translatedByOcn(QString)), SLOT(showOcnTranslation(QString)));
  connect(translator_, SIGNAL(translatedByExcite(QString)), SLOT(showExciteTranslation(QString)));
  connect(translator_, SIGNAL(translatedBySdl(QString)), SLOT(showSdlTranslation(QString)));
  connect(translator_, SIGNAL(translatedBySystran(QString)), SLOT(showSystranTranslation(QString)));
  connect(translator_, SIGNAL(translatedByNifty(QString)), SLOT(showNiftyTranslation(QString)));
  connect(translator_, SIGNAL(translatedByInfoseek(QString)), SLOT(showInfoseekTranslation(QString)));

  connect(extraTranslator_, SIGNAL(translatedByRomaji(QString)), SLOT(showRomajiAdditionalTranslation(QString)));
  connect(extraTranslator_, SIGNAL(translatedByMicrosoft(QString)), SLOT(showMicrosoftAdditionalTranslation(QString)));
  connect(extraTranslator_, SIGNAL(translatedByGoogle(QString)), SLOT(showGoogleAdditionalTranslation(QString)));
  connect(extraTranslator_, SIGNAL(translatedByYahoo(QString)), SLOT(showYahooAdditionalTranslation(QString)));
  connect(extraTranslator_, SIGNAL(translatedByFresheye(QString)), SLOT(showFresheyeAdditionalTranslation(QString)));
  connect(extraTranslator_, SIGNAL(translatedByOcn(QString)), SLOT(showOcnAdditionalTranslation(QString)));
  connect(extraTranslator_, SIGNAL(translatedByExcite(QString)), SLOT(showExciteAdditionalTranslation(QString)));
  connect(extraTranslator_, SIGNAL(translatedBySdl(QString)), SLOT(showSdlAdditionalTranslation(QString)));
  connect(extraTranslator_, SIGNAL(translatedBySystran(QString)), SLOT(showSystranAdditionalTranslation(QString)));
  connect(extraTranslator_, SIGNAL(translatedByNifty(QString)), SLOT(showNiftyAdditionalTranslation(QString)));
  connect(extraTranslator_, SIGNAL(translatedByInfoseek(QString)), SLOT(showInfoseekAdditionalTranslation(QString)));

#ifdef AC_ENABLE_GAME
  connect(this, SIGNAL(windowPicked(WId)), annotationView_, SLOT(setTrackedWindow(WId)));
  connect(this, SIGNAL(windowPicked(WId)), SLOT(setWindowOnTop()));
  //connect(this, SIGNAL(windowPicked(WId)), osdWindow_, SLOT(setWindowOnTop()));

  connect(this, SIGNAL(attached(ProcessInfo)), syncView_->processView(), SIGNAL(attached(ProcessInfo)));
  connect(this, SIGNAL(detached(ProcessInfo)), syncView_->processView(), SIGNAL(detached(ProcessInfo)));

  //connect(syncView_->processView(), SIGNAL(attached(ProcessInfo)), recentThreadView_, SLOT(setProcessNameFromProcessInfo(ProcessInfo)));
  //connect(syncView_->processView(), SIGNAL(detached(ProcessInfo)), recentThreadView_, SLOT(clearProcessName()));

  connect(syncView_->processView(), SIGNAL(attached(ProcessInfo)), SLOT(synchronizeProcess(ProcessInfo)));

  AC_CONNECT_MESSAGE(syncView_, this, Qt::AutoConnection);
  AC_CONNECT_WARNING(syncView_, this, Qt::AutoConnection);

  //connect(syncView_, SIGNAL(threadsSelected(TextThreadList,ProcessInfo)), SLOT(backlogDialog()));
  connect(syncView_, SIGNAL(threadsSelected(TextThreadList,ProcessInfo)), SLOT(subscribeThreads(TextThreadList,ProcessInfo)));

  connect(this, SIGNAL(subscribeThreadsRequested(TextThreadList,ProcessInfo)), SLOT(subscribeThreads(TextThreadList,ProcessInfo)), Qt::QueuedConnection);
  connect(this, SIGNAL(subscribeThreadsRequested(TextThreadList,ProcessInfo)), syncView_->threadView(), SLOT(setThreads(TextThreadList)), Qt::QueuedConnection);
  connect(this, SIGNAL(hideSyncViewRequested()), syncView_, SLOT(hide()), Qt::QueuedConnection);

  //connect(recentThreadView_, SIGNAL(channelSelected(ulong,QString)), SLOT(updateChannel(ulong,QString)));
  //connect(recentThreadView_, SIGNAL(channelSelected(ulong,QString)), recentThreadView_, SLOT(hide()));
  connect(messageHandler_, SIGNAL(contextChanged(qint64,qint64)), annotationView_, SLOT(showAnnotationsInContext(qint64,qint64)));
  connect(messageHandler_, SIGNAL(textChanged(QString,int)), SLOT(translateGameText(QString,int)));
  connect(messageHandler_, SIGNAL(textChanged(QString,int)), SLOT(showGameText(QString,int)), Qt::QueuedConnection);
  connect(messageHandler_, SIGNAL(textChanged(QString,int)), SLOT(translateWithAtlas(QString,int)), Qt::QueuedConnection);
#endif // AC_ENABLE_GAME
  //connect(player_, SIGNAL(opening()), SLOT(backlogDialog()));
  // MRL resolver
  connect(mrlResolver_, SIGNAL(mediaResolved(MediaInfo,QNetworkCookieJar*)), SLOT(openRemoteMedia(MediaInfo,QNetworkCookieJar*)));
  connect(mrlResolver_, SIGNAL(subtitleResolved(QString,QString,QString)), SLOT(importAnnotationsFromUrl(QString,QString)));

  AC_CONNECT_MESSAGE(mrlResolver_, this, Qt::QueuedConnection);
  AC_CONNECT_ERROR(mrlResolver_, this, Qt::QueuedConnection);

#ifdef Q_OS_WIN
  connect(AcUi::globalInstance(), SIGNAL(aeroEnabledChanged(bool)), AcSettings::globalSettings(), SLOT(setAeroEnabled(bool)));
#endif // Q_OS_WIN

  // Annotation codec
  {
    AnnotationCodecManager *acm = AnnotationCodecManager::globalInstance();
    connect(acm, SIGNAL(fetched(AnnotationList,QString,QString)), SLOT(addRemoteAnnotations(AnnotationList,QString,QString)));

    AC_CONNECT_MESSAGE(acm, this, Qt::QueuedConnection);
    connect(acm, SIGNAL(errorMessage(QString)), SLOT(warn(QString)), Qt::QueuedConnection);
  }

  // Streaming service
  //{
  //  StreamService *ss = StreamService::globalInstance();
  //  connect(ss, SIGNAL(message(QString)), SLOT(showMessage(QString)));
  //  connect(ss, SIGNAL(errorMessage(QString)), SLOT(warn(QString)));
  //  connect(ss, SIGNAL(streamReady(QString)), SLOT(openStreamUrl(QString)));
  //}

  // - Close
  //connect(this, SIGNAL(closing()), db_, SLOT(dispose));

  // - IPC -
  connect(appServer_, SIGNAL(arguments(QStringList)), SLOT(openSources(QStringList)), Qt::QueuedConnection);
}

void
MainWindow::openSources(const QStringList &l)
{
  playlist_ = l;
  if (!playlist_.isEmpty())
    openSource(playlist_.first());
}

//bool
//MainWindow::isValid() const
//{ return player_ && player_->isValid();}

void
MainWindow::createActions()
{
  connect(openAct_ = new QAction(QIcon(RC_IMAGE_OPEN), tr("Open"), this),
          SIGNAL(triggered()), hub_, SLOT(open()));
  connect(playAct_ = new QAction(QIcon(RC_IMAGE_PLAY), tr("Play"), this),
          SIGNAL(triggered()), hub_, SLOT(play()));
          playAct_->setShortcut(QKeySequence("SPACE"));
  connect(pauseAct_ = new QAction(QIcon(RC_IMAGE_PAUSE), tr("Pause"), this),
          SIGNAL(triggered()), hub_, SLOT(pause()));
          pauseAct_->setShortcut(QKeySequence("SPACE"));
  connect(stopAct_ = new QAction(QIcon(RC_IMAGE_STOP), tr("Stop"), this),
          SIGNAL(triggered()), hub_, SLOT(stop()));
  connect(promptStopAct_ = new QAction(QIcon(RC_IMAGE_STOP), tr("Stop"), this),
          SIGNAL(triggered()), SLOT(promptStop()));
  connect(replayAct_ = new QAction(QIcon(RC_IMAGE_REPLAY), tr("Replay"), this),
          SIGNAL(triggered()), SLOT(replay()));
  connect(previousAct_ = new QAction(QIcon(RC_IMAGE_PREVIOUS), tr("Previous"), this),
          SIGNAL(triggered()), SLOT(previous()));
  connect(nextAct_ = new QAction(QIcon(RC_IMAGE_NEXT), tr("Next"), this),
          SIGNAL(triggered()), SLOT(next()));
  connect(nextFrameAct_ = new QAction(QIcon(RC_IMAGE_NEXTFRAME), tr("Next Frame"), this),
          SIGNAL(triggered()), player_, SLOT(nextFrame()));
  connect(snapshotAct_ = new QAction(tr("Snapshot"), this),
          SIGNAL(triggered()), SLOT(snapshot()));
  connect(snapshotAllAct_ = new QAction(tr("Snapshot All"), this),
          SIGNAL(triggered()), SLOT(snapshotAll()));

  connect(loginAct_ = new QAction(tr("Login"), this),
          SIGNAL(triggered()), SLOT(showLogin()));
  connect(logoutAct_ = new QAction(tr("Logout"), this),
          SIGNAL(triggered()), SLOT(logout()));

  connect(updateAnnotationsAct_ = new QAction(tr("Update Annotations"), this),
          SIGNAL(triggered()), SLOT(updateAnnotations()));
          updateAnnotationsAct_->setShortcut(QKeySequence("CTRL+U"));
          addAction(updateAnnotationsAct_);

  connect(clearRecentAct_ = new QAction(tr("Remove All"), this),
          SIGNAL(triggered()), SLOT(clearRecent()));
  connect(validateRecentAct_ = new QAction(tr("Remove Non-existed"), this),
          SIGNAL(triggered()), SLOT(updateRecent()));

  connect(downloadCurrentUrlAct_ = new QAction(QIcon(ACRC_IMAGE_DOWNLOADER), TR(T_MENUTEXT_DOWNLOADCURRENT), this),
          SIGNAL(triggered()), SLOT(downloadCurrentUrl()));
          downloadCurrentUrlAct_->setShortcut(QKeySequence("CTRL+D"));
          addAction(downloadCurrentUrlAct_);

  connect(openProxyBrowserAct_ = new QAction(QIcon(ACRC_IMAGE_BROWSER), TR(T_MENUTEXT_PROXYVIEW), this),
          SIGNAL(triggered()), SLOT(openProxyBrowser()));
          openProxyBrowserAct_->setShortcut(QKeySequence("CTRL+B"));
          addAction(openProxyBrowserAct_);

  connect(showTranslatorAct_ = new QAction(QIcon(ACRC_IMAGE_TRANSLATOR), tr("Translator"), this),
          SIGNAL(triggered()), SLOT(showTranslator()));

  connect(showDownloaderAct_ = new QAction(QIcon(ACRC_IMAGE_DOWNLOADER), TR(T_MENUTEXT_DOWNLOAD), this),
          SIGNAL(triggered()), SLOT(showDownloader()));

  connect(openInWebBrowserAct_ = new QAction(QIcon(ACRC_IMAGE_BROWSER), TR(T_MENUTEXT_OPENINWEBBROWSER), this),
          SIGNAL(triggered()), SLOT(openInWebBrowser()));

  connect(showWindowPickerAct_ = new QAction(TR(T_MENUTEXT_WINDOWPICKDIALOG), this),
          SIGNAL(triggered()), SLOT(showWindowPicker()));
  connect(showProcessAct_ = new QAction(TR(T_MENUTEXT_PROCESSPICKDIALOG), this),
          SIGNAL(triggered()), SLOT(showProcess()));

  connect(showLibraryAct_ = new QAction(TR(T_LIBRARY), this),
          SIGNAL(triggered()), this, SLOT(showLibrary()));
          showLibraryAct_->setShortcut(QKeySequence("CTRL+L"));
          addAction(showLibraryAct_);

  connect(toggleSubtitleOnTopAct_ = new QAction(tr("Show Subtitle On Top"), this),
          SIGNAL(triggered(bool)), AnnotationSettings::globalSettings(), SLOT(setSubtitleOnTop(bool)));
          toggleSubtitleOnTopAct_->setCheckable(true);
  connect(toggleSubtitleAnnotationVisibleAct_ = new QAction(tr("Show Subtitle"), this), SIGNAL(triggered(bool)),
          annotationView_, SLOT(setSubtitleVisible(bool)));
          toggleSubtitleAnnotationVisibleAct_->setCheckable(true);
  connect(toggleNonSubtitleAnnotationVisibleAct_ = new QAction(tr("Show Non-Subtitle Annotation"), this), SIGNAL(triggered(bool)),
          annotationView_, SLOT(setNonSubtitleVisible(bool)));
          toggleNonSubtitleAnnotationVisibleAct_->setCheckable(true);

  connect(toggleMagnifierVisibleAct_ = new QAction(tr("Image Filter") + " (" + TR(T_EXPERIMENTAL) + ")", this),
          SIGNAL(triggered(bool)), SLOT(setMagnifierVisible(bool)));
          toggleMagnifierVisibleAct_->setCheckable(true);
          toggleMagnifierVisibleAct_->setShortcut(QKeySequence("CTRL+E"));
          addAction(toggleMagnifierVisibleAct_);

  connect(toggleAnnotationBandwidthLimitedAct_ = new QAction(tr("Limit Bandwidth"), this),
          SIGNAL(triggered(bool)), annotationView_, SLOT(setItemCountLimited(bool)));
          toggleAnnotationBandwidthLimitedAct_->setCheckable(true);

  connect(toggleAnnotationTraditionalChineseAct_ = new QAction(QIcon(ACRC_IMAGE_TRADITIONAL_CHINESE), tr("Convert to Traditional Chinese"), this),
          SIGNAL(triggered(bool)), AnnotationSettings::globalSettings(), SLOT(setPreferTraditionalChinese(bool)));
          toggleAnnotationTraditionalChineseAct_->setCheckable(true);

  connect(toggleAnnotationAvatarVisibleAct_ = new QAction(tr("Always Show Avatar"), this),
          SIGNAL(triggered(bool)), AnnotationSettings::globalSettings(), SLOT(setAvatarVisible(bool)));
          toggleAnnotationAvatarVisibleAct_->setCheckable(true);

  connect(toggleAnnotationMetaVisibleAct_ = new QAction(tr("Always Show Timestamp"), this),
          SIGNAL(triggered(bool)), AnnotationSettings::globalSettings(), SLOT(setMetaVisible(bool)));
          toggleAnnotationMetaVisibleAct_->setCheckable(true);

  connect(togglePreferFloatAnnotationAct_ = new QAction(tr("No Drifting Annotations"), this),
          SIGNAL(triggered(bool)), AnnotationSettings::globalSettings(), SLOT(setPreferFloat(bool)));
          togglePreferFloatAnnotationAct_->setCheckable(true);

  connect(toggleWindowTrackingAct_ = new QAction(QIcon(RC_IMAGE_TRACE), tr("Trace Window"), this),
          SIGNAL(triggered(bool)), SLOT(setWindowTrackingEnabled(bool)));
          toggleWindowTrackingAct_->setCheckable(true);

  connect(setAudioDelayAct_ = new QAction(tr("Audio Delay"), this),
          SIGNAL(triggered()), SLOT(showAudioDelayDialog()));

  connect(openAnnotationFileAct_ = new QAction(tr("Open Annotation Files"), this),
          SIGNAL(triggered()), SLOT(openAnnotationFile()));

  connect(openAnnotationUrlAct_ = new QAction(tr("Import Annotations from URL"), this),
          SIGNAL(triggered()), SLOT(openAnnotationUrl()));
          openAnnotationUrlAct_->setShortcuts(QKeySequence::Italic);

  connect(downloadAnnotationsAct_ = new QAction(tr("Download Annotations (XML/JSON)"), this),
          SIGNAL(triggered()), SLOT(downloadAnnotations()));

  toggleSaveAnnotationFileAct_ = new QAction(tr("Save Annotation File"), this);
          toggleSaveAnnotationFileAct_->setCheckable(true);

  connect(aboutAct_ = new QAction(tr("About"), this),
          SIGNAL(triggered()), SLOT(about()));
  connect(updateAct_ = new QAction(tr("Check for Update"), this),
          SIGNAL(triggered()), SLOT(update()));

  connect(preferencesAct_ = new QAction(tr("Preferences"), this),
          SIGNAL(triggered()), SLOT(showPreferences()));
#ifndef Q_OS_MAC
          preferencesAct_->setShortcut(QKeySequence("CTRL+,"));
          addAction(preferencesAct_);
#endif // !Q_OS_MAC
#ifdef Q_OS_WIN
          new QShortcut(QKeySequence("ALT+O"), this, SLOT(showPreferences()));
          preferencesAct_->setText(preferencesAct_->text() + " [ALT+O]");
#endif // Q_OS_WIN

  connect(helpAct_ = new QAction(tr("Help"), this), SIGNAL(triggered()), SLOT(help()));
          helpAct_->setShortcuts(QKeySequence::HelpContents);
#ifndef Q_OS_MAC
          addAction(helpAct_);
#endif // Q_OS_MAC

  connect(quitAct_ = new QAction(tr("Quit"), this),
          SIGNAL(triggered()), SLOT(close()));
          quitAct_->setShortcuts(QKeySequence::Quit);

  connect(toggleRomajiTranslatorAct_ = new QAction(tr("Romaji Translator"), this),
          SIGNAL(triggered(bool)), SLOT(toggleRomajiTranslator(bool)));
          toggleRomajiTranslatorAct_->setCheckable(true);
  connect(toggleYahooTranslatorAct_ = new QAction(tr("Yahoo! Honyaku"), this),
          SIGNAL(triggered(bool)), SLOT(toggleYahooTranslator(bool)));
          toggleYahooTranslatorAct_->setCheckable(true);
  connect(toggleFresheyeTranslatorAct_ = new QAction(tr("freshEYE Honyaku") + " (en,zh)", this),
          SIGNAL(triggered(bool)), SLOT(toggleFresheyeTranslator(bool)));
          toggleFresheyeTranslatorAct_->setCheckable(true);
  connect(toggleOcnTranslatorAct_ = new QAction(tr("OCN Honyaku") + " (en,zh,ko) - " + TR(T_DEFAULT), this),
          SIGNAL(triggered(bool)), SLOT(toggleOcnTranslator(bool)));
          toggleOcnTranslatorAct_->setCheckable(true);
  connect(toggleInfoseekTranslatorAct_ = new QAction(tr("Infoseek Honyaku"), this),
          SIGNAL(triggered(bool)), SLOT(toggleInfoseekTranslator(bool)));
          toggleInfoseekTranslatorAct_->setCheckable(true);
  connect(toggleGoogleTranslatorAct_ = new QAction(tr("Google Translator"), this),
          SIGNAL(triggered(bool)), SLOT(toggleGoogleTranslator(bool)));
          toggleGoogleTranslatorAct_->setCheckable(true);
  connect(toggleMicrosoftTranslatorAct_ = new QAction(tr("Microsoft Translator"), this),
          SIGNAL(triggered(bool)), SLOT(toggleMicrosoftTranslator(bool)));
          toggleMicrosoftTranslatorAct_->setCheckable(true);
  connect(toggleNiftyTranslatorAct_ = new QAction(tr("@nifty honyaku") + " (en)", this),
          SIGNAL(triggered(bool)), SLOT(toggleNiftyTranslator(bool)));
          toggleNiftyTranslatorAct_->setCheckable(true);
  connect(toggleExciteTranslatorAct_ = new QAction(tr("Excite Honyaku") + " (en)", this),
          SIGNAL(triggered(bool)), SLOT(toggleExciteTranslator(bool)));
          toggleExciteTranslatorAct_->setCheckable(true);
  connect(toggleSdlTranslatorAct_ = new QAction(tr("SDL Translator") + " (en)", this),
          SIGNAL(triggered(bool)), SLOT(toggleSdlTranslator(bool)));
          toggleSdlTranslatorAct_->setCheckable(true);
  connect(toggleSystranTranslatorAct_ = new QAction(tr("SYSTRAN Translator") + " (en)", this),
          SIGNAL(triggered(bool)), SLOT(toggleSystranTranslator(bool)));
          toggleSystranTranslatorAct_->setCheckable(true);

  connect(setStereoChannelAct_ = new QAction(tr("Stereo"), this),
          SIGNAL(triggered(bool)), SLOT(setStereoChannel()));
          setStereoChannelAct_->setCheckable(true);
  connect(setReverseStereoChannelAct_ = new QAction(tr("Reverse"), this),
          SIGNAL(triggered(bool)), SLOT(setReverseStereoChannel()));
          setReverseStereoChannelAct_->setCheckable(true);
  connect(setLeftChannelAct_ = new QAction(tr("Left"), this),
          SIGNAL(triggered(bool)), SLOT(setLeftChannel()));
          setLeftChannelAct_->setCheckable(true);
  connect(setRightChannelAct_ = new QAction(tr("Right"), this),
          SIGNAL(triggered(bool)), SLOT(setRightChannel()));
          setRightChannelAct_->setCheckable(true);
  connect(setDolbysChannelAct_ = new QAction(tr("Dolby's"), this),
          SIGNAL(triggered(bool)), SLOT(setDolbysChannel()));
          setDolbysChannelAct_->setCheckable(true);

  connect(toggleMenuThemeEnabledAct_ = new QAction(TR(T_MENUTEXT_MENUTHEME) + " (DEBUG)", this),
          SIGNAL(triggered(bool)), AcUi::globalInstance(), SLOT(setMenuEnabled(bool)));
          toggleMenuThemeEnabledAct_->setCheckable(true);
  connect(toggleSubmitAct_ = new QAction(TR(T_MENUTEXT_AUTOSUBMIT) + " (DEBUG)", this),
          SIGNAL(triggered(bool)), SLOT(setSubmit(bool)));
          toggleSubmitAct_->setCheckable(true);

#ifdef AC_ENABLE_GAME
  toggleGameTextVisibleAct_ = new QAction(tr("Show Galgame Text"), this);
          toggleGameTextVisibleAct_->setCheckable(true);
          toggleGameTextVisibleAct_->setChecked(Settings::globalSettings()->isGameTextVisible());

  toggleAppLocaleEnabledAct_ = new QAction(tr("Load Galgame using AppLocale"), this);
          toggleAppLocaleEnabledAct_->setCheckable(true);
          toggleAppLocaleEnabledAct_->setChecked(Settings::globalSettings()->isAppLocaleEnabled());

  toggleGameTextColorfulAct_ = new QAction(tr("Colorful"), this);
          toggleGameTextColorfulAct_->setCheckable(true);
          toggleGameTextColorfulAct_->setChecked(Settings::globalSettings()->isGameTextColorful());

  toggleGameTextResizableAct_ = new QAction(tr("Resize"), this);
          toggleGameTextResizableAct_->setCheckable(true);
          toggleGameTextResizableAct_->setChecked(Settings::globalSettings()->isGameTextResizable());

  connect(openCurrentGameAct_ = new QAction(tr("Sync with Running Galgame"), this),
          SIGNAL(triggered()), SLOT(openCurrentGame()));
#endif // AC_ENABLE_GAME

  connect(setAppLanguageToEnglishAct_ = new QAction(QIcon(RC_IMAGE_ENGLISH), TR(T_ENGLISH), this),
          SIGNAL(triggered(bool)), SLOT(setAppLanguageToEnglish()));
          setAppLanguageToEnglishAct_->setCheckable(true);
  connect(setAppLanguageToJapaneseAct_ = new QAction(QIcon(RC_IMAGE_JAPANESE), TR(T_JAPANESE), this),
          SIGNAL(triggered(bool)), SLOT(setAppLanguageToJapanese()));
          setAppLanguageToJapaneseAct_->setCheckable(true);
  connect(setAppLanguageToTraditionalChineseAct_ = new QAction(QIcon(ACRC_IMAGE_TRADITIONAL_CHINESE), TR(T_CHINESE), this),
          SIGNAL(triggered(bool)), SLOT(setAppLanguageToTraditionalChinese()));
          setAppLanguageToTraditionalChineseAct_->setCheckable(true);
  connect(setAppLanguageToSimplifiedChineseAct_ = new QAction(QIcon(ACRC_IMAGE_SIMPLIFIED_CHINESE), TR(T_SIMPLIFIEDCHINESE), this),
          SIGNAL(triggered(bool)), SLOT(setAppLanguageToSimplifiedChinese()));
          setAppLanguageToSimplifiedChineseAct_->setCheckable(true);

  connect(setUserLanguageToEnglishAct_ = new QAction(QIcon(ACRC_IMAGE_ENGLISH), TR(T_ENGLISH), this),
          SIGNAL(triggered(bool)), SLOT(setUserLanguageToEnglish()));
          setUserLanguageToEnglishAct_->setCheckable(true);
  connect(setUserLanguageToJapaneseAct_ = new QAction(QIcon(ACRC_IMAGE_JAPANESE), TR(T_JAPANESE), this),
          SIGNAL(triggered(bool)), SLOT(setUserLanguageToJapanese()));
          setUserLanguageToJapaneseAct_->setCheckable(true);
  connect(setUserLanguageToKoreanAct_ = new QAction(QIcon(ACRC_IMAGE_KOREAN), TR(T_KOREAN), this),
          SIGNAL(triggered(bool)), SLOT(setUserLanguageToKorean()));
          setUserLanguageToKoreanAct_->setCheckable(true);
  connect(setUserLanguageToFrenchAct_ = new QAction(QIcon(ACRC_IMAGE_FRENCH), TR(T_FRENCH), this),
          SIGNAL(triggered(bool)), SLOT(setUserLanguageToFrench()));
          setUserLanguageToFrenchAct_->setCheckable(true);
  connect(setUserLanguageToGermanAct_ = new QAction(QIcon(ACRC_IMAGE_GERMAN), TR(T_GERMAN), this),
          SIGNAL(triggered(bool)), SLOT(setUserLanguageToGerman()));
          setUserLanguageToGermanAct_->setCheckable(true);
  connect(setUserLanguageToItalianAct_ = new QAction(QIcon(ACRC_IMAGE_ITALIAN), TR(T_ITALIAN), this),
          SIGNAL(triggered(bool)), SLOT(setUserLanguageToItalian()));
          setUserLanguageToItalianAct_->setCheckable(true);
  connect(setUserLanguageToSpanishAct_ = new QAction(QIcon(ACRC_IMAGE_SPANISH), TR(T_SPANISH), this),
          SIGNAL(triggered(bool)), SLOT(setUserLanguageToSpanish()));
          setUserLanguageToSpanishAct_->setCheckable(true);
  connect(setUserLanguageToPortugueseAct_ = new QAction(QIcon(ACRC_IMAGE_PORTUGUESE), TR(T_PORTUGUESE), this),
          SIGNAL(triggered(bool)), SLOT(setUserLanguageToPortuguese()));
          setUserLanguageToPortugueseAct_->setCheckable(true);
  connect(setUserLanguageToRussianAct_ = new QAction(QIcon(ACRC_IMAGE_RUSSIAN), TR(T_RUSSIAN), this),
          SIGNAL(triggered(bool)), SLOT(setUserLanguageToRussian()));
          setUserLanguageToRussianAct_->setCheckable(true);
  connect(setUserLanguageToTraditionalChineseAct_ = new QAction(QIcon(ACRC_IMAGE_TRADITIONAL_CHINESE), TR(T_CHINESE), this),
          SIGNAL(triggered(bool)), SLOT(setUserLanguageToTraditionalChinese()));
          setUserLanguageToTraditionalChineseAct_->setCheckable(true);
  connect(setUserLanguageToSimplifiedChineseAct_ = new QAction(QIcon(ACRC_IMAGE_SIMPLIFIED_CHINESE), TR(T_SIMPLIFIEDCHINESE), this),
          SIGNAL(triggered(bool)), SLOT(setUserLanguageToSimplifiedChinese()));
          setUserLanguageToSimplifiedChineseAct_->setCheckable(true);

  connect(toggleTranslateSelectionAct_ = new QAction(tr("Translate Double-Clicked Game Text"), this),
          SIGNAL(triggered(bool)), annotationView_, SLOT(setTranslateWordEnabled(bool)));
          toggleTranslateSelectionAct_->setCheckable(true);
  connect(toggleTranslateAct_ = new QAction(tr("Show Online Translation"), this),
          SIGNAL(triggered(bool)), SLOT(setTranslateEnabled(bool)));
          toggleTranslateAct_->setCheckable(true);
#ifdef WITH_WIN_ATLAS
  toggleAtlasEnabledAct_ = new QAction(tr("Show Offline ATLAS Translation"), this);
          toggleAtlasEnabledAct_->setCheckable(true);
          toggleAtlasEnabledAct_->setChecked(Settings::globalSettings()->isAtlasEnabled());
#endif // WITH_WIN_ATLAS

  // TODO clean up actions
  // *** CHECKPOINT ***

#define MAKE_ACTION(_action, _styleid, _receiver, _slot) \
  connect(_action = new QAction(QIcon(RC_IMAGE_##_styleid), TR(T_MENUTEXT_##_styleid), this), \
          SIGNAL(triggered()), _receiver, _slot);
#define MAKE_TOGGLE(_action, _styleid, _receiver, _slot) \
  connect(_action = new QAction(QIcon(RC_IMAGE_##_styleid), TR(T_MENUTEXT_##_styleid), this), \
          SIGNAL(triggered(bool)), _receiver, _slot); \
  _action->setCheckable(true);

  MAKE_ACTION(openFileAct_,     OPENFILE,       this,           SLOT(openFile()))
  MAKE_ACTION(openUrlAct_,      OPENURL,        this,           SLOT(openUrl()))
  MAKE_ACTION(openDeviceAct_,   OPENDEVICE,     this,           SLOT(openDevice()))
  MAKE_ACTION(copyCurrentUrlAct_,  COPYCURRENTURL,     this,    SLOT(copyCurrentUrl()))
  MAKE_ACTION(copyCurrentTitleAct_,  COPYCURRENTTITLE, this,    SLOT(copyCurrentTitle()))
  MAKE_ACTION(openVideoDeviceAct_, OPENVIDEODEVICE,  this,      SLOT(openVideoDevice()))
  MAKE_ACTION(openAudioDeviceAct_, OPENAUDIODEVICE,  this,      SLOT(openAudioDevice()))
  MAKE_ACTION(openSubtitleAct_, OPENSUBTITLE,   this,           SLOT(openSubtitle()))
  MAKE_ACTION(checkInternetConnectionAct_,      CHECKINTERNET,   this, SLOT(checkInternetConnection()))
  MAKE_ACTION(deleteCachesAct_, DELETECACHE,    this,           SLOT(deleteCaches()))
  MAKE_ACTION(newWindowAct_,    NEWWINDOW,      this,           SLOT(newWindow()))
  MAKE_ACTION(resumeAnnotationAct_, RESUMEANNOT,this,           SLOT(resumeAnnotations()))
  MAKE_ACTION(resetAnnotationOffsetAct_, RESETANNOTOFFSET, AnnotationSettings::globalSettings(),  SLOT(resetOffset()))
  MAKE_ACTION(increaseAnnotationOffsetAct_, INCREASEANNOTOFFSET, this,  SLOT(increaseAnnotationOffset()))
  MAKE_ACTION(decreaseAnnotationOffsetAct_, DECREASEANNOTOFFSET, this,  SLOT(decreaseAnnotationOffset()))
  MAKE_ACTION(resetAnnotationScaleAct_, RESETANNOTSCALE, this,  SLOT(resetAnnotationScale()))
  MAKE_ACTION(increaseAnnotationScaleAct_, INCREASEANNOTSCALE, this,  SLOT(annotationScaleUp()))
  MAKE_ACTION(decreaseAnnotationScaleAct_, DECREASEANNOTSCALE, this,  SLOT(annotationScaleDown()))
  MAKE_ACTION(resetAnnotationRotationAct_, RESETANNOTROTATION, AnnotationSettings::globalSettings(),  SLOT(resetRotation()))
  MAKE_ACTION(increaseAnnotationRotationAct_, INCREASEANNOTROTATION, this,  SLOT(annotationRotateUp()))
  MAKE_ACTION(decreaseAnnotationRotationAct_, DECREASEANNOTROTATION, this,  SLOT(annotationRotateDown()))
  MAKE_ACTION(saveMediaAct_, SAVEMEDIA, this, SLOT(saveMedia()))
  MAKE_ACTION(hueUpAct_, HUEUP, this, SLOT(hueUp()))
  MAKE_ACTION(hueDownAct_, HUEDOWN, this, SLOT(hueDown()))
  MAKE_ACTION(hueResetAct_, RESET, this, SLOT(hueReset()))
  MAKE_ACTION(contrastUpAct_, CONTRASTUP, this, SLOT(contrastUp()))
  MAKE_ACTION(contrastDownAct_, CONTRASTDOWN, this, SLOT(contrastDown()))
  MAKE_ACTION(contrastResetAct_, RESET, this, SLOT(contrastReset()))
  MAKE_ACTION(brightnessUpAct_, BRIGHTNESSUP, this, SLOT(brightnessUp()))
  MAKE_ACTION(brightnessDownAct_, BRIGHTNESSDOWN, this, SLOT(brightnessDown()))
  MAKE_ACTION(brightnessResetAct_, RESET, this, SLOT(brightnessReset()))
  MAKE_ACTION(gammaUpAct_, GAMMAUP, this, SLOT(gammaUp()))
  MAKE_ACTION(gammaDownAct_, GAMMADOWN, this, SLOT(gammaDown()))
  MAKE_ACTION(gammaResetAct_, RESET, this, SLOT(gammaReset()))
  MAKE_ACTION(saturationUpAct_, SATURATIONUP, this, SLOT(saturationUp()))
  MAKE_ACTION(saturationDownAct_, SATURATIONDOWN, this, SLOT(saturationDown()))
  MAKE_ACTION(saturationResetAct_, RESET, this, SLOT(saturationReset()))

  MAKE_ACTION(showSeekAct_,  SEEKDIALOG,  this,         SLOT(showSeek()))
  MAKE_ACTION(showAnnotationEditorAct_, ANNOTATIONEDITOR, this, SLOT(showAnnotationEditor()))
  MAKE_ACTION(showBlacklistAct_, BLACKLIST,   this,    SLOT(showBlacklist()))
  MAKE_ACTION(showMediaInfoAct_, MEDIAINFO, this, SLOT(showMediaInfo()))
  MAKE_ACTION(showAnnotationSourceAct_, TOKENVIEW, this, SLOT(showAnnotationSource()))
  MAKE_ACTION(showAnnotationBrowserAct_, ANNOTATIONBROWSER, this, SLOT(showAnnotationBrowser()))
  MAKE_ACTION(showUserAct_, USER,          this,         SLOT(showUser()))
  MAKE_ACTION(showAnnotationAnalyticsAct_, ANNOTANALYTICS, this,  SLOT(showAnnotationAnalytics()))
  MAKE_ACTION(showConsoleAct_, CONSOLE,   this,    SLOT(showConsole()));
  MAKE_ACTION(showBacklogAct_, BACKLOG,   this,    SLOT(showBacklog()))
#ifdef AC_ENABLE_GAME
  MAKE_ACTION(showSyncGameAct_, SIGNALVIEW, this, SLOT(showSyncGame()))
  MAKE_ACTION(showGamePreferencesAct_, RECENTMESSAGES, this, SLOT(showGamePreferences()))
#endif // AC_ENABLE_GAME

  MAKE_TOGGLE(toggleAutoPlayNextAct_, AUTOPLAYNEXT, this,  SLOT(setAutoPlayNext()))
  MAKE_TOGGLE(toggleRepeatCurrentAct_, AUTOPLAYCURRENT, this,  SLOT(setRepeatCurrent()))
  MAKE_TOGGLE(toggleNoRepeatAct_, NOAUTOPLAY, this,  SLOT(setNoRepeat()))
  MAKE_TOGGLE(actualSizeAct_, ACTUALSIZE, this,  SLOT(actualSize()))
  MAKE_TOGGLE(togglePreferLocalDatabaseAct_, PREFERLOCALDB, dataServer_,  SLOT(setPreferCache(bool)))
  MAKE_TOGGLE(toggleSaveMediaAct_, AUTOSAVEMEDIA, this, SLOT(setSaveMedia(bool)))
  MAKE_TOGGLE(nothingAfterFinishedAct_, NOTHINGAFTERFINISHED, this, SLOT(nothingAfterFinished()))
  MAKE_TOGGLE(sleepAfterFinishedAct_, SLEEPAFTERFINISHED, this, SLOT(sleepAfterFinished()))
  MAKE_TOGGLE(shutdownAfterFinishedAct_, SHUTDOWNAFTERFINISHED, this, SLOT(shutdownAfterFinished()))
  //MAKE_TOGGLE(toggleMultipleWindowsEnabledAct_, MULTIWINDOW, this, SLOT(setMultipleWindowsEnabled(bool)))
  MAKE_TOGGLE(toggleClipboardMonitorEnabledAct_, MONITORCLIPBOARD, clipboardMonitor_, SLOT(setEnabled(bool)))
  //MAKE_TOGGLE(toggleNetworkProxyDialogVisibleAct_, NETWORKPROXY, this, SLOT(setNetworkProxyDialogVisible(bool)))
  MAKE_TOGGLE(toggleAeroEnabledAct_, ENABLEAERO, AcUi::globalInstance(), SLOT(setAeroEnabled(bool)))
  MAKE_TOGGLE(toggleFullScreenModeAct_, FULLSCREEN, hub_,       SLOT(setFullScreenWindowMode(bool)))
  MAKE_TOGGLE(toggleMenuBarVisibleAct_, SHOWMENUBAR, menuBar(), SLOT(setVisible(bool)))
  //MAKE_TOGGLE(toggleAnnotationCountDialogVisibleAct_, ANNOTATIONLIMIT, this, SLOT(setAnnotationCountDialogVisible(bool)))
  MAKE_TOGGLE(toggleEmbeddedModeAct_, EMBED,    hub_,           SLOT(setEmbeddedPlayerMode(bool)))
  MAKE_TOGGLE(toggleMiniModeAct_, MINI,         hub_,           SLOT(setMiniPlayerMode(bool)))
  MAKE_TOGGLE(toggleLiveModeAct_, LIVE,         hub_,           SLOT(setLiveTokenMode(bool)))
  MAKE_TOGGLE(toggleSyncModeAct_, SYNC,         hub_,           SLOT(setSyncPlayMode(bool)))
  MAKE_TOGGLE(toggleAnnotationVisibleAct_, SHOWANNOT, annotationView_, SLOT(setItemVisible(bool)))
  MAKE_TOGGLE(toggleSubtitleVisibleAct_, SHOWSUBTITLE, player_, SLOT(setSubtitleVisible(bool)))
  MAKE_TOGGLE(toggleWindowOnTopAct_, WINDOWSTAYSONTOP, this, SLOT(setWindowOnTop(bool)))
  MAKE_TOGGLE(toggleUserAnonymousAct_,  ANONYMOUS,       this,         SLOT(setUserAnonymous(bool)))
  MAKE_TOGGLE(toggleAnnotationFilterEnabledAct_, ENABLEBLACKLIST,   annotationFilter_,    SLOT(setEnabled(bool)))
  //MAKE_TOGGLE(toggleSiteAccountViewVisibleAct_, SITEACCOUNT, this, SLOT(setSiteAccountViewVisible(bool)))
  //MAKE_TOGGLE(toggleCommentViewVisibleAct_, COMMENTVIEW, this, SLOT(setCommentViewVisible(bool)))
  MAKE_ACTION(openHomePageAct_, HOMEPAGE, this, SLOT(openHomePage()))
  MAKE_TOGGLE(togglePlayerLabelEnabled_, LABELPLAYER,   embeddedCanvas_,  SLOT(setEnabled(bool)))
  MAKE_TOGGLE(toggleEmbeddedPlayerOnTopAct_, EMBEDONTOP,   embeddedPlayer_,  SLOT(setOnTop(bool)))
  MAKE_ACTION(clearCacheAct_,   CLEARCACHE,     cache_,         SLOT(clear()))
  MAKE_ACTION(connectAct_,      CONNECT,        server_,        SLOT(updateConnected()))
  MAKE_ACTION(disconnectAct_,   DISCONNECT,     server_,        SLOT(disconnect()))
  MAKE_TOGGLE(setSubtitleColorToDefaultAct_, DEFAULTCOLOR, this, SLOT(setSubtitleColorToDefault()))
  MAKE_TOGGLE(setSubtitleColorToWhiteAct_, WHITECOLOR, this, SLOT(setSubtitleColorToWhite()))
  MAKE_TOGGLE(setSubtitleColorToCyanAct_, CYANCOLOR, this, SLOT(setSubtitleColorToCyan()))
  MAKE_TOGGLE(setSubtitleColorToBlueAct_, BLUECOLOR, this, SLOT(setSubtitleColorToBlue()))
  MAKE_TOGGLE(setSubtitleColorToOrangeAct_, ORANGECOLOR, this, SLOT(setSubtitleColorToOrange()))
  MAKE_TOGGLE(setSubtitleColorToPurpleAct_, PURPLECOLOR, this, SLOT(setSubtitleColorToPurple()))
  MAKE_TOGGLE(setSubtitleColorToBlackAct_, BLACKCOLOR, this, SLOT(setSubtitleColorToBlack()))
  MAKE_TOGGLE(setSubtitleColorToRedAct_, REDCOLOR, this, SLOT(setSubtitleColorToRed()))
  MAKE_TOGGLE(setAnnotationEffectToDefaultAct_, AUTO, this, SLOT(setAnnotationEffectToDefault()))
  MAKE_TOGGLE(setAnnotationEffectToTransparentAct_, TRANSPARENT, this, SLOT(setAnnotationEffectToTransparent()))
  MAKE_TOGGLE(setAnnotationEffectToShadowAct_, SHADOW, this, SLOT(setAnnotationEffectToShadow()))
  MAKE_TOGGLE(setAnnotationEffectToBlurAct_, BLUR, this, SLOT(setAnnotationEffectToBlur()))
  MAKE_TOGGLE(setDefaultAspectRatioAct_, DEFAULT, this, SLOT(setDefaultAspectRatio()))
  MAKE_TOGGLE(setStandardAspectRatioAct_, ASPECTRATIOSTANDARD, this, SLOT(setStandardAspectRatio()))
  MAKE_TOGGLE(setWideScreenAspectRatioAct_, ASPECTRATIOWIDESCREEN, this, SLOT(setWideScreenAspectRatio()))

  MAKE_ACTION(forward5sAct_,    FORWARD5S,      this,   SLOT(forward5s()))
  MAKE_ACTION(backward5sAct_,   BACKWARD5S,     this,   SLOT(backward5s()))
  MAKE_ACTION(forward10sAct_,   FORWARD10S,     this,   SLOT(forward10s()))
  MAKE_ACTION(backward10sAct_,  BACKWARD10S,    this,   SLOT(backward10s()))
  MAKE_ACTION(forward25sAct_,   FORWARD25S,     this,   SLOT(forward25s()))
  MAKE_ACTION(backward30sAct_,  BACKWARD30S,    this,   SLOT(backward30s()))
  MAKE_ACTION(forward60sAct_,   FORWARD60S,     this,   SLOT(forward60s()))
  MAKE_ACTION(backward60sAct_,  BACKWARD60S,    this,   SLOT(backward60s()))
  MAKE_ACTION(forward85sAct_,   FORWARD85S,     this,   SLOT(forward85s()))
  MAKE_ACTION(backward90sAct_,  BACKWARD90S,    this,   SLOT(backward90s()))
  MAKE_ACTION(forward1mAct_,    FORWARD1M,      this,   SLOT(forward1m()))
  MAKE_ACTION(backward1mAct_,   BACKWARD1M,     this,   SLOT(backward1m()))
  MAKE_ACTION(forward5mAct_,    FORWARD5M,      this,   SLOT(forward5m()))
  MAKE_ACTION(backward5mAct_,   BACKWARD5M,     this,   SLOT(backward5m()))
  MAKE_ACTION(forward10mAct_,   FORWARD10M,     this,   SLOT(forward10m()))
  MAKE_ACTION(backward10mAct_,  BACKWARD10M,    this,   SLOT(backward10m()))

  MAKE_ACTION(clearPlaylistAct_,CLEARPLAYLIST,  this,   SLOT(clearPlaylist()))
  MAKE_ACTION(nextPlaylistItemAct_,NEXT,        this,   SLOT(openNextPlaylistItem()))
  MAKE_ACTION(previousPlaylistItemAct_,PREVIOUS,this,   SLOT(openPreviousPlaylistItem()))

  MAKE_ACTION(previousSectionAct_, PREVIOUSSECTION, player_,  SLOT(setPreviousTitle()))
  MAKE_ACTION(nextSectionAct_,     NEXTSECTION,    player_,   SLOT(setNextTitle()))

  MAKE_ACTION(previousFileAct_, PREVIOUS, this,  SLOT(openPreviousFile()))
  MAKE_ACTION(nextFileAct_,     NEXT,     this,  SLOT(openNextFile()))
  MAKE_ACTION(moreFilesAct_,    MORE,     this,  SLOT(moreBrowsedFiles()))
  MAKE_ACTION(lessFilesAct_,    LESS,     this,  SLOT(lessBrowsedFiles()))
  MAKE_ACTION(openBrowsedDirectoryAct_, OPENDIRECTORY, this,  SLOT(openBrowsedDirectory()))

  MAKE_TOGGLE(toggleAnnotationLanguageToEnglishAct_,  ENGLISH, this, SLOT(invalidateAnnotationLanguages()))
  MAKE_TOGGLE(toggleAnnotationLanguageToJapaneseAct_, JAPANESE,this, SLOT(invalidateAnnotationLanguages()))
  MAKE_TOGGLE(toggleAnnotationLanguageToChineseAct_, CHINESE, this, SLOT(invalidateAnnotationLanguages()))
  MAKE_TOGGLE(toggleAnnotationLanguageToKoreanAct_, KOREAN, this, SLOT(invalidateAnnotationLanguages()))
  MAKE_TOGGLE(toggleAnnotationLanguageToFrenchAct_, FRENCH, this, SLOT(invalidateAnnotationLanguages()))
  MAKE_TOGGLE(toggleAnnotationLanguageToGermanAct_, GERMAN, this, SLOT(invalidateAnnotationLanguages()))
  MAKE_TOGGLE(toggleAnnotationLanguageToItalianAct_, ITALIAN, this, SLOT(invalidateAnnotationLanguages()))
  MAKE_TOGGLE(toggleAnnotationLanguageToSpanishAct_, SPANISH, this, SLOT(invalidateAnnotationLanguages()))
  MAKE_TOGGLE(toggleAnnotationLanguageToPortugueseAct_, PORTUGUESE, this, SLOT(invalidateAnnotationLanguages()))
  MAKE_TOGGLE(toggleAnnotationLanguageToRussianAct_, RUSSIAN, this, SLOT(invalidateAnnotationLanguages()))
  //MAKE_TOGGLE(toggleAnnotationLanguageToUnknownAct_, UNKNOWNLANGUAGE, this, SLOT(invalidateAnnotationLanguages()))
  MAKE_TOGGLE(toggleAnnotationLanguageToAnyAct_, ANYLANGUAGE, this,SLOT(invalidateAnnotationLanguages()))

  MAKE_ACTION(showMaximizedAct_,        MAXIMIZE,       this,    SLOT(showMaximized()))
  MAKE_ACTION(showMinimizedAct_,        MINIMIZE,       this,    SLOT(showMinimizedAndPause()))
  MAKE_ACTION(showNormalAct_,           RESTORE,        this,    SLOT(showNormal()))

#ifdef Q_OS_WIN
  //if (!QtWin::isWindowsVistaOrLater())
#endif // Q_OS_WIN
  {
    MAKE_TOGGLE(setThemeToDefaultAct_,    DEFAULTTHEME,   this,   SLOT(setThemeToDefault()))
    MAKE_TOGGLE(setThemeToRandomAct_,     RANDOMTHEME,    this,   SLOT(setThemeToRandom()))
    MAKE_TOGGLE(setThemeToDarkAct_,       DARKTHEME,      this,   SLOT(setThemeToDark()))
    MAKE_TOGGLE(setThemeToBlackAct_,      BLACKTHEME,     this,   SLOT(setThemeToBlack()))
    MAKE_TOGGLE(setThemeToBlueAct_,       BLUETHEME,      this,   SLOT(setThemeToBlue()))
    MAKE_TOGGLE(setThemeToBrownAct_,      BROWNTHEME,     this,   SLOT(setThemeToBrown()))
    MAKE_TOGGLE(setThemeToCyanAct_,       CYANTHEME,      this,   SLOT(setThemeToCyan()))
    MAKE_TOGGLE(setThemeToGrayAct_,       GRAYTHEME,      this,   SLOT(setThemeToGray()))
    MAKE_TOGGLE(setThemeToGreenAct_,      GREENTHEME,     this,   SLOT(setThemeToGreen()))
    MAKE_TOGGLE(setThemeToPinkAct_,       PINKTHEME,      this,   SLOT(setThemeToPink()))
    MAKE_TOGGLE(setThemeToPurpleAct_,     PURPLETHEME,    this,   SLOT(setThemeToPurple()))
    MAKE_TOGGLE(setThemeToRedAct_,        REDTHEME,       this,   SLOT(setThemeToRed()))
    MAKE_TOGGLE(setThemeToWhiteAct_,      WHITETHEME,     this,   SLOT(setThemeToWhite()))
    MAKE_TOGGLE(setThemeToYellowAct_,     YELLOWTHEME,    this,   SLOT(setThemeToYellow()))
  }

#undef MAKE_ACTION

  // Shortcuts
  {
    //toggleSeekDialogVisibleAct_->setShortcut(QKeySequence("CTRL+F"));
    //addAction(toggleSeekDialogVisibleAct_);

    nextFrameAct_->setShortcut(QKeySequence("CTRL+G"));
    addAction(nextFrameAct_);

    openFileAct_->setShortcuts(QKeySequence::Open);
    addAction(openFileAct_);

    openUrlAct_->setShortcuts(QKeySequence::New);
    addAction(openUrlAct_);

    snapshotAct_->setShortcuts(QKeySequence::Print);
    addAction(snapshotAct_);

    saveMediaAct_->setShortcuts(QKeySequence::Save);
    addAction(saveMediaAct_);

    resumeAnnotationAct_->setShortcut(QKeySequence("CTRL+R"));
    addAction(resumeAnnotationAct_);

#ifndef Q_OS_MAC
    toggleWindowOnTopAct_->setShortcuts(QKeySequence::AddTab);
    addAction(toggleWindowOnTopAct_);
#endif // !Q_OS_MAC

    //QShortcut *i = new QShortcut(QKeySequence("CTRL+I"), this);
    //connect(i, SIGNAL(activated()), SLOT(showMediaInfoView()));
    //toggleMediaInfoViewVisibleAct_->setShortcut(QKeySequence("CTRL+I"));

    //QShortcut *help = new QShortcut(QKeySequence::HelpContents, this);
    //connect(help, SIGNAL(activated()), SLOT(help()));

    //playAct_->setShortcuts(QKeySequence::Print);
    //pauseAct_->setShortcuts(QKeySequence::Print);
    //QShortcut *p = new QShortcut(QKeySequence::Print, this);
    //connect(p, SIGNAL(activated()), SLOT(playPause()));

    toggleEmbeddedModeAct_->setShortcut(QKeySequence(K_CMD "+1"));
    addAction(toggleEmbeddedModeAct_);
    toggleMiniModeAct_->setShortcut(QKeySequence(K_CMD "+2"));
    addAction(toggleMiniModeAct_);
    toggleFullScreenModeAct_->setShortcut(QKeySequence(K_CMD "+3"));
    addAction(toggleFullScreenModeAct_);
#ifdef WITH_WIN_PICKER
    toggleWindowTrackingAct_->setShortcut(QKeySequence(K_CMD "+4"));
    addAction(toggleWindowTrackingAct_);
#endif // WITH_WIN_PICKER

    QShortcut *fs = new QShortcut(QKeySequence(K_CMD "+ENTER"), this);
    connect(fs, SIGNAL(activated()), hub_, SLOT(toggleFullScreenWindowMode()));

    QShortcut *f11 = new QShortcut(QKeySequence("F11"), this);
    connect(f11, SIGNAL(activated()), hub_, SLOT(toggleFullScreenWindowMode()));

    showAnnotationEditorAct_->setShortcut(QKeySequence("CTRL+F1"));
    addAction(showAnnotationEditorAct_);

    showAnnotationBrowserAct_->setShortcut(QKeySequence("CTRL+F"));
    addAction(showAnnotationBrowserAct_);
    QShortcut *cf2 = new QShortcut(QKeySequence("CTRL+F2"), this);
    connect(cf2, SIGNAL(activated()), showAnnotationSourceAct_, SLOT(trigger()));

    showAnnotationSourceAct_->setShortcut(QKeySequence("CTRL+T"));
    addAction(showAnnotationSourceAct_);
    QShortcut *cf3 = new QShortcut(QKeySequence("CTRL+F3"), this);
    connect(cf3, SIGNAL(activated()), showAnnotationSourceAct_, SLOT(trigger()));

    showBlacklistAct_->setShortcut(QKeySequence("CTRL+F4"));
    addAction(showBlacklistAct_);

    showBacklogAct_->setShortcut(QKeySequence("CTRL+F5"));
    addAction(showBacklogAct_);

    showAnnotationAnalyticsAct_->setShortcut(QKeySequence("CTRL+F6"));
    addAction(showAnnotationAnalyticsAct_);

    QShortcut *pp = new QShortcut(QKeySequence("CTRL+SHIFT+LEFT"), this);
    connect(pp, SIGNAL(activated()), SLOT(previous()));
    QShortcut *nn = new QShortcut(QKeySequence("CTRL+SHIFT+RIGHT"), this);
    connect(nn, SIGNAL(activated()), SLOT(next()));

#ifndef Q_OS_MAC
    newWindowAct_->setShortcut(QKeySequence("CTRL+SHIFT+N"));
    addAction(newWindowAct_);
    //QShortcut *csn = new QShortcut(QKeySequence("CTRL+SHIFT+N"), this);
    //connect(csn, SIGNAL(activated()), SLOT(newWindow()));
#endif // Q_OS_MAC

    QShortcut *backward = new QShortcut(QKeySequence("CTRL+LEFT"), this);
    connect(backward, SIGNAL(activated()), SLOT(backward90s()));
    QShortcut *forward = new QShortcut(QKeySequence("CTRL+RIGHT"), this);
    connect(forward, SIGNAL(activated()), SLOT(forward85s()));

    QShortcut *esc = new QShortcut(QKeySequence("CTRL+ESC"), this);
    connect(esc, SIGNAL(activated()), SLOT(showMinimizedAndPause()));

    QShortcut *up = new QShortcut(QKeySequence("CTRL+UP"), this);
    connect(up, SIGNAL(activated()), SLOT(volumeUp()));
    QShortcut *down = new QShortcut(QKeySequence("CTRL+DOWN"), this);
    connect(down, SIGNAL(activated()), SLOT(volumeDown()));
    QShortcut *csup = new QShortcut(QKeySequence("CTRL+SHIFT+UP"), this);
    connect(csup, SIGNAL(activated()), SLOT(volumeUp()));
    QShortcut *csdown = new QShortcut(QKeySequence("CTRL+SHIFT+DOWN"), this);
    connect(csdown, SIGNAL(activated()), SLOT(volumeDown()));

    // - Video adjustment -
    saturationUpAct_->setShortcut(QKeySequence(K_ALT "+Q"));
    addAction(saturationUpAct_);
    saturationDownAct_->setShortcut(QKeySequence(K_ALT "+A"));
    addAction(saturationDownAct_);
    saturationResetAct_->setShortcut(QKeySequence(K_ALT "+Z"));
    addAction(saturationResetAct_);

    gammaUpAct_->setShortcut(QKeySequence(K_ALT "+W"));
    addAction(gammaUpAct_);
    gammaDownAct_->setShortcut(QKeySequence(K_ALT "+S"));
    addAction(gammaDownAct_);
    gammaResetAct_->setShortcut(QKeySequence(K_ALT "+X"));
    addAction(gammaResetAct_);

    hueUpAct_->setShortcut(QKeySequence(K_ALT "+E"));
    addAction(hueUpAct_);
#ifdef Q_OS_MAC
    hueDownAct_->setShortcut(QKeySequence(K_ALT "+3"));
#else
    hueDownAct_->setShortcut(QKeySequence(K_ALT "+D"));
#endif // Q_OS_MAC
    addAction(hueDownAct_);
    hueResetAct_->setShortcut(QKeySequence(K_ALT "+C"));
    addAction(hueResetAct_);

    contrastUpAct_->setShortcut(QKeySequence(K_ALT "+R"));
    addAction(contrastUpAct_);
    contrastDownAct_->setShortcut(QKeySequence(K_ALT "+F"));
    addAction(contrastDownAct_);
    contrastResetAct_->setShortcut(QKeySequence(K_ALT "+V"));
    addAction(contrastResetAct_);

    brightnessUpAct_->setShortcut(QKeySequence(K_ALT "+T"));
    addAction(brightnessUpAct_);
    brightnessDownAct_->setShortcut(QKeySequence(K_ALT "+G"));
    addAction(brightnessDownAct_);
    brightnessResetAct_->setShortcut(QKeySequence(K_ALT "+B"));
    addAction(brightnessResetAct_);

    increaseAnnotationScaleAct_->setShortcut(QKeySequence("CTRL+="));
    addAction(increaseAnnotationScaleAct_);
    decreaseAnnotationScaleAct_->setShortcut(QKeySequence("CTRL+-"));
    addAction(decreaseAnnotationScaleAct_);
    resetAnnotationScaleAct_->setShortcut(QKeySequence("CTRL+0"));
    addAction(resetAnnotationScaleAct_);

    increaseAnnotationOffsetAct_->setShortcut(QKeySequence("CTRL+ALT+="));
    addAction(increaseAnnotationOffsetAct_);
    decreaseAnnotationOffsetAct_->setShortcut(QKeySequence("CTRL+ALT+-"));
    addAction(decreaseAnnotationOffsetAct_);
    resetAnnotationOffsetAct_->setShortcut(QKeySequence("CTRL+ALT+0"));
    addAction(resetAnnotationOffsetAct_);

    increaseAnnotationRotationAct_->setShortcut(QKeySequence("CTRL+SHIFT+="));
    addAction(increaseAnnotationRotationAct_);
    decreaseAnnotationRotationAct_->setShortcut(QKeySequence("CTRL+SHIFT+-"));
    addAction(decreaseAnnotationRotationAct_);
    resetAnnotationRotationAct_->setShortcut(QKeySequence("CTRL+SHIFT+0"));
    addAction(resetAnnotationRotationAct_);

#ifdef Q_OS_MAC
    QShortcut *aq = new QShortcut(QKeySequence(K_ALT "+Q"), this);
    connect(aq, SIGNAL(activated()), SLOT(saturationUp()));
    QShortcut *aa = new QShortcut(QKeySequence(K_ALT "+A"), this);
    connect(aa, SIGNAL(activated()), SLOT(saturationDown()));
    QShortcut *az = new QShortcut(QKeySequence(K_ALT "+Z"), this);
    connect(az, SIGNAL(activated()), SLOT(saturationReset()));

    QShortcut *aw = new QShortcut(QKeySequence(K_ALT "+W"), this);
    connect(aw, SIGNAL(activated()), SLOT(gammaUp()));
    QShortcut *as = new QShortcut(QKeySequence(K_ALT "+S"), this);
    connect(as, SIGNAL(activated()), SLOT(gammaDown()));
    QShortcut *ax = new QShortcut(QKeySequence(K_ALT "+X"), this);
    connect(ax, SIGNAL(activated()), SLOT(gammaReset()));

    QShortcut *ae = new QShortcut(QKeySequence(K_ALT "+E"), this);
    connect(ae, SIGNAL(activated()), SLOT(hueUp()));
#ifdef Q_OS_MAC
    QShortcut *a3 = new QShortcut(QKeySequence(K_ALT "+3"), this);
    connect(a3, SIGNAL(activated()), SLOT(hueDown()));
#else
    QShortcut *ad = new QShortcut(QKeySequence(K_ALT "+D"), this);
    connect(ad, SIGNAL(activated()), SLOT(hueDown()));
#endif // Q_OS_MAC
    QShortcut *ac = new QShortcut(QKeySequence(K_ALT "+C"), this);
    connect(ac, SIGNAL(activated()), SLOT(hueReset()));

    QShortcut *ar = new QShortcut(QKeySequence(K_ALT "+R"), this);
    connect(ar, SIGNAL(activated()), SLOT(contrastUp()));
    QShortcut *af = new QShortcut(QKeySequence(K_ALT "+F"), this);
    connect(af, SIGNAL(activated()), SLOT(contrastDown()));
    QShortcut *av = new QShortcut(QKeySequence(K_ALT "+V"), this);
    connect(av, SIGNAL(activated()), SLOT(contrastReset()));

    QShortcut *at = new QShortcut(QKeySequence(K_ALT "+T"), this);
    connect(at, SIGNAL(activated()), SLOT(brightnessUp()));
    QShortcut *ag = new QShortcut(QKeySequence(K_ALT "+G"), this);
    connect(ag, SIGNAL(activated()), SLOT(brightnessDown()));
    QShortcut *ab = new QShortcut(QKeySequence(K_ALT "+B"), this);
    connect(ab, SIGNAL(activated()), SLOT(brightnessReset()));
#endif // Q_OS_MAC
  }
}

void
MainWindow::createSearchEngines()
{
  enum { LastEngine = SearchEngineFactory::WikiZh };
  for (int i = 0; i <= LastEngine; i++) {
    SearchEngine *e = SearchEngineFactory::globalInstance()->create(i);
    e->setParent(this);
    searchEngines_.append(e);
  }
}

void
MainWindow::createSearchMenu()
{
  createSearchEngines();
  searchMenu_ = new QMenu(this); {
    AcUi::globalInstance()->setContextMenuStyle(searchMenu_, this);
    searchMenu_->setTitle(tr("Search")  + " ...");
    searchMenu_->setToolTip(tr("Search"));
  }

  searchMenu_->addAction(copyCurrentTitleAct_);
  searchMenu_->addSeparator();

  QString t = tr("Search with %1");

  SearchEngine *e = searchEngines_[SearchEngineFactory::Google];
  searchMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithGoogle()));

  e = searchEngines_[SearchEngineFactory::GoogleImages];
  searchMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithGoogleImages()));

  e = searchEngines_[SearchEngineFactory::Bing];
  searchMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithBing()));

  searchMenu_->addSeparator();

  e = searchEngines_[SearchEngineFactory::Nicovideo];
  searchMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithNicovideo()));

  e = searchEngines_[SearchEngineFactory::Bilibili];
  searchMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithBilibili()));

  e = searchEngines_[SearchEngineFactory::Acfun];
  searchMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithAcfun()));

  e = searchEngines_[SearchEngineFactory::Youtube];
  searchMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithYoutube()));

  e = searchEngines_[SearchEngineFactory::Youku];
  searchMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithYouku()));

  searchMenu_->addSeparator();

  e = searchEngines_[SearchEngineFactory::WikiEn];
  searchMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithWikiEn()));

  e = searchEngines_[SearchEngineFactory::WikiJa];
  searchMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithWikiJa()));

  e = searchEngines_[SearchEngineFactory::WikiZh];
  searchMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithWikiZh()));
}

void
MainWindow::createMenus()
{
  AcUi *ui = AcUi::globalInstance();

  // Context menu
  contextMenu_ = new QMenu(TR(T_TITLE_PROGRAM), this); {
    ui->setContextMenuStyle(contextMenu_, true); // persistent = true
  }

  createSearchMenu();

  userMenu_ = new QMenu(TR(T_TITLE_PROGRAM), this); {
    ui->setContextMenuStyle(userMenu_, true); // persistent = true
    PlayerUi *players[] = { mainPlayer_, miniPlayer_, embeddedPlayer_ };
    BOOST_FOREACH (PlayerUi *p, players) {
      p->userButton()->setMenu(userMenu_);
      connect(p, SIGNAL(updateUserMenuRequested()), SLOT(updateUserMenu()));
    }
  }

  helpContextMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(helpContextMenu_, true); // persistent = true
    helpContextMenu_->setTitle(TR(T_MENUTEXT_HELP) + " ...");
    helpContextMenu_->setToolTip(TR(T_TOOLTIP_HELP));

    helpContextMenu_->addAction(updateAct_);
    helpContextMenu_->addAction(checkInternetConnectionAct_);
    helpContextMenu_->addAction(showConsoleAct_);
    //helpContextMenu_->addAction(helpAct_);
    helpContextMenu_->addAction(openHomePageAct_);

    helpContextMenu_->addAction(aboutAct_);
  }

  currentMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(currentMenu_, this);
    currentMenu_->setTitle(tr("Current media")  + " ...");
    currentMenu_->setToolTip(tr("Current media"));

    currentMenu_->addAction(copyCurrentUrlAct_);
    currentMenu_->addSeparator();
    //currentMenu_->addAction(openBrowsedDirectoryAct_);
    currentMenu_->addAction(openInWebBrowserAct_);
    currentMenu_->addAction(downloadCurrentUrlAct_);
    currentMenu_->addAction(saveMediaAct_);
  }

  translatorMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(translatorMenu_, this);
    translatorMenu_->setTitle(tr("Translation Service")  + " ...");
    translatorMenu_->setToolTip(tr("Translation Service"));

    translatorMenu_->addAction(toggleRomajiTranslatorAct_);
    translatorMenu_->addSeparator();
    translatorMenu_->addAction(toggleOcnTranslatorAct_);
    translatorMenu_->addAction(toggleFresheyeTranslatorAct_);
    translatorMenu_->addSeparator();
    translatorMenu_->addAction(toggleInfoseekTranslatorAct_);
    translatorMenu_->addAction(toggleYahooTranslatorAct_);
    translatorMenu_->addAction(toggleGoogleTranslatorAct_);
    translatorMenu_->addAction(toggleMicrosoftTranslatorAct_);
    translatorMenu_->addSeparator();
    translatorMenu_->addAction(toggleNiftyTranslatorAct_);
    translatorMenu_->addAction(toggleExciteTranslatorAct_);
    translatorMenu_->addAction(toggleSdlTranslatorAct_);
    translatorMenu_->addAction(toggleSystranTranslatorAct_);
  }

  annotationEffectMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(annotationEffectMenu_, true); // persistent = true

    annotationEffectMenu_->setTitle(TR(T_ANNOTATIONEFFECT) + " ...");
    annotationEffectMenu_->setToolTip(TR(T_ANNOTATIONEFFECT));

    annotationEffectMenu_->addAction(setAnnotationEffectToDefaultAct_);
    annotationEffectMenu_->addSeparator();
    annotationEffectMenu_->addAction(setAnnotationEffectToTransparentAct_);
    annotationEffectMenu_->addAction(setAnnotationEffectToShadowAct_);
    annotationEffectMenu_->addAction(setAnnotationEffectToBlurAct_);
  }

  appLanguageMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(appLanguageMenu_, true); // persistent = true

    appLanguageMenu_->setTitle("Language ..."); // no tr wrapping "language"
    appLanguageMenu_->setToolTip(TR(T_TOOLTIP_APPLANGUAGE));

    appLanguageMenu_->addAction(setAppLanguageToEnglishAct_);
    appLanguageMenu_->addSeparator();
    appLanguageMenu_->addAction(setAppLanguageToJapaneseAct_);
    appLanguageMenu_->addSeparator();
    appLanguageMenu_->addAction(setAppLanguageToTraditionalChineseAct_); // Temporarily disabled since no traditional chinese at this time
    appLanguageMenu_->addAction(setAppLanguageToSimplifiedChineseAct_);
  }

  userLanguageMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(userLanguageMenu_, true); // persistent = true

    userLanguageMenu_->setTitle(TR(T_MENUTEXT_USERLANGUAGE) + " ...");
    userLanguageMenu_->setToolTip(TR(T_TOOLTIP_USERLANGUAGE));

    userLanguageMenu_->addAction(setUserLanguageToEnglishAct_);
    userLanguageMenu_->addSeparator();
    userLanguageMenu_->addAction(setUserLanguageToJapaneseAct_);
    userLanguageMenu_->addSeparator();
    userLanguageMenu_->addAction(setUserLanguageToTraditionalChineseAct_);
    userLanguageMenu_->addAction(setUserLanguageToSimplifiedChineseAct_);
    userLanguageMenu_->addSeparator();
    userLanguageMenu_->addAction(setUserLanguageToKoreanAct_);
    userLanguageMenu_->addSeparator();
    userLanguageMenu_->addAction(setUserLanguageToFrenchAct_);
    userLanguageMenu_->addAction(setUserLanguageToGermanAct_);
    userLanguageMenu_->addAction(setUserLanguageToItalianAct_);
    userLanguageMenu_->addAction(setUserLanguageToSpanishAct_);
    userLanguageMenu_->addAction(setUserLanguageToPortugueseAct_);
    userLanguageMenu_->addAction(setUserLanguageToRussianAct_);
    //userLanguageMenu_->addSeparator();
    //userLanguageMenu_->addAction(setUserLanguageToUnknownAct_);
  }

  gameMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(gameMenu_, this);
    gameMenu_->setTitle(tr("Sync with Galgame")  + " ...");
    gameMenu_->setToolTip(tr("Sync with Galgame"));
#ifdef AC_ENABLE_GAME

#ifdef WITH_WIN_PICKER
    gameMenu_->addAction(showProcessAct_);
#endif // WITH_WIN_PICKER
    gameMenu_->addAction(showSyncGameAct_);
#endif // AC_ENABLE_GAME
  }

#ifdef AC_ENABLE_GAME
  gameTextMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(gameTextMenu_, this);
    gameTextMenu_->setTitle(tr("Galgame Text")  + " ...");
    gameTextMenu_->setToolTip(tr("Galgame Text"));

    gameTextMenu_->addAction(toggleGameTextVisibleAct_);
    gameTextMenu_->addAction(toggleTranslateSelectionAct_);
    gameTextMenu_->addSeparator();
    gameTextMenu_->addAction(toggleGameTextColorfulAct_);
    gameTextMenu_->addAction(toggleGameTextResizableAct_);
  }
#endif // AC_ENABLE_GAME

  annotationLanguageMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(annotationLanguageMenu_, true); // persistent = true

    annotationLanguageMenu_->setTitle(TR(T_MENUTEXT_ANNOTATIONLANGUAGE) + " ...");
    annotationLanguageMenu_->setToolTip(TR(T_TOOLTIP_ANNOTATIONLANGUAGE));

    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToAnyAct_);
    //annotationLanguageMenu_->addAction(toggleAnnotationLanguageToUnknownAct_);
    annotationLanguageMenu_->addSeparator();
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToEnglishAct_);
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToJapaneseAct_);
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToChineseAct_);
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToKoreanAct_);
    annotationLanguageMenu_->addSeparator();
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToFrenchAct_);
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToGermanAct_);
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToItalianAct_);
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToSpanishAct_);
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToPortugueseAct_);
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToRussianAct_);
  }

//#ifdef Q_OS_WIN
//  if (!QtWin::isWindowsVistaOrLater())
//#endif // Q_OS_WIN
  {
    themeMenu_ = new QMenu(this);

    ui->setContextMenuStyle(themeMenu_, true); // persistent = true
    themeMenu_->setTitle(TR(T_MENUTEXT_THEME) + " ...");
    themeMenu_->setToolTip(TR(T_TOOLTIP_THEME));

    themeMenu_->addAction(setThemeToDefaultAct_);
    themeMenu_->addAction(setThemeToRandomAct_);
    themeMenu_->addSeparator();
    themeMenu_->addAction(setThemeToDarkAct_);
    themeMenu_->addAction(setThemeToBlackAct_);
    themeMenu_->addAction(setThemeToBlueAct_);
    themeMenu_->addAction(setThemeToBrownAct_);
    themeMenu_->addAction(setThemeToCyanAct_);
    themeMenu_->addAction(setThemeToGrayAct_);
    themeMenu_->addAction(setThemeToGreenAct_);
    themeMenu_->addAction(setThemeToPinkAct_);
    themeMenu_->addAction(setThemeToPurpleAct_);
    themeMenu_->addAction(setThemeToRedAct_);
    themeMenu_->addAction(setThemeToWhiteAct_);
    themeMenu_->addAction(setThemeToYellowAct_);
  }

  subtitleColorMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(subtitleColorMenu_, true); // persistent = true

    subtitleColorMenu_->setTitle(tr("Subtitle Color") + " ...");
    subtitleColorMenu_->setToolTip(tr("Subtitle Color"));

    subtitleColorMenu_->addAction(setSubtitleColorToDefaultAct_);
    subtitleColorMenu_->addSeparator();
    subtitleColorMenu_->addAction(setSubtitleColorToWhiteAct_);
    subtitleColorMenu_->addAction(setSubtitleColorToCyanAct_);
    subtitleColorMenu_->addAction(setSubtitleColorToBlueAct_);
    subtitleColorMenu_->addAction(setSubtitleColorToPurpleAct_);
    subtitleColorMenu_->addAction(setSubtitleColorToRedAct_);
    subtitleColorMenu_->addAction(setSubtitleColorToOrangeAct_);
    subtitleColorMenu_->addAction(setSubtitleColorToBlackAct_);
  }

  browseMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(browseMenu_, true); // persistent = true

    browseMenu_->setTitle(TR(T_MENUTEXT_BROWSE) + " ...");
    browseMenu_->setToolTip(TR(T_TOOLTIP_BROWSE));
  }

  trackMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(trackMenu_, true); // persistent = true

    trackMenu_->setTitle(TR(T_MENUTEXT_TRACK) + " ...");
    trackMenu_->setToolTip(TR(T_TOOLTIP_TRACK));
  }

  recentMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(recentMenu_, true); // persistent = true
    recentMenu_->setTitle(TR(T_MENUTEXT_RECENT) + " ...");
    recentMenu_->setToolTip(TR(T_TOOLTIP_RECENT));
  }

  playlistMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(playlistMenu_, true); // persistent = true
    playlistMenu_->setTitle(TR(T_MENUTEXT_PLAYLIST) + " ...");
    playlistMenu_->setToolTip(TR(T_TOOLTIP_PLAYLIST));
  }

  aspectRatioMenu_ = new QMenu(this); {
    aspectRatioMenu_->setTitle(TR(T_ASPECTRATIO) + " ...");
    aspectRatioMenu_->setToolTip(TR(T_ASPECTRATIO));
    ui->setContextMenuStyle(aspectRatioMenu_, true); // persistent = true

    aspectRatioMenu_->addAction(setDefaultAspectRatioAct_);
    aspectRatioMenu_->addSeparator();
    aspectRatioMenu_->addAction(setStandardAspectRatioAct_);
    aspectRatioMenu_->addAction(setWideScreenAspectRatioAct_);
  }

  videoMenu_ = new QMenu(this); {
    videoMenu_->setTitle(tr("Adjust Video") + " ...");
    videoMenu_->setToolTip(tr("Adjust Video"));
    ui->setContextMenuStyle(videoMenu_, true); // persistent = true

    videoMenu_->addAction(showMediaInfoAct_);
    videoMenu_->addSeparator();

    videoMenu_->addMenu(aspectRatioMenu_);
    videoMenu_->addSeparator();

    videoMenu_->addAction(saturationUpAct_);
    videoMenu_->addAction(saturationDownAct_);
    videoMenu_->addAction(saturationResetAct_);
    videoMenu_->addSeparator();
    videoMenu_->addAction(gammaUpAct_);
    videoMenu_->addAction(gammaDownAct_);
    videoMenu_->addAction(gammaResetAct_);
    videoMenu_->addSeparator();
    videoMenu_->addAction(hueUpAct_);
    videoMenu_->addAction(hueDownAct_);
    videoMenu_->addAction(hueResetAct_);
    videoMenu_->addSeparator();
    videoMenu_->addAction(contrastUpAct_);
    videoMenu_->addAction(contrastDownAct_);
    videoMenu_->addAction(contrastResetAct_);
    videoMenu_->addSeparator();
    videoMenu_->addAction(brightnessUpAct_);
    videoMenu_->addAction(brightnessDownAct_);
    videoMenu_->addAction(brightnessResetAct_);
  }

  backwardMenu_ = new QMenu(this); {
    backwardMenu_->setIcon(QIcon(RC_IMAGE_BACKWARD));
    backwardMenu_->setTitle(TR(T_MENUTEXT_BACKWARD) + " ...");
    backwardMenu_->setToolTip(TR(T_TOOLTIP_BACKWARD));
    ui->setContextMenuStyle(backwardMenu_, true); // persistent = true

    backwardMenu_->addAction(backward5sAct_);
    backwardMenu_->addAction(backward30sAct_);
    backwardMenu_->addAction(backward90sAct_);
    backwardMenu_->addAction(backward5mAct_);
    backwardMenu_->addAction(backward10mAct_);
  }

  forwardMenu_ = new QMenu(this); {
    forwardMenu_->setIcon(QIcon(RC_IMAGE_FORWARD));
    forwardMenu_->setTitle(TR(T_MENUTEXT_FORWARD) + " ...");
    forwardMenu_->setToolTip(TR(T_TOOLTIP_FORWARD));
    ui->setContextMenuStyle(forwardMenu_, true); // persistent = true

    forwardMenu_->addAction(forward5sAct_);
    forwardMenu_->addAction(forward25sAct_);
    forwardMenu_->addAction(forward85sAct_);
    forwardMenu_->addAction(forward5mAct_);
    forwardMenu_->addAction(forward10mAct_);
  }

  openMenu_ = new QMenu(this); {
    openMenu_->setTitle(tr("Open") + " ...");
    openMenu_->setToolTip(tr("Open"));
    ui->setContextMenuStyle(openMenu_, true); // persistent = true
  }

  importAnnotationMenu_ = new QMenu(this); {
    importAnnotationMenu_->setTitle(tr("Import Annotations") + " ...");
    importAnnotationMenu_->setToolTip(tr("Import Annotations"));
    ui->setContextMenuStyle(importAnnotationMenu_, true); // persistent = true

    importAnnotationMenu_->addAction(openAnnotationUrlAct_);
    importAnnotationMenu_->addAction(openAnnotationFileAct_);
    importAnnotationMenu_->addAction(downloadAnnotationsAct_);
    importAnnotationMenu_->addSeparator();
    importAnnotationMenu_->addAction(updateAnnotationsAct_);
  }

  subtitleMenu_ = new QMenu(this); {
    subtitleMenu_->setTitle(tr("Subtitle") + " ...");
    subtitleMenu_->setToolTip(tr("Subtitle"));
    ui->setContextMenuStyle(subtitleMenu_, true); // persistent = true
  }

  playMenu_ = new QMenu(this); {
    playMenu_->setTitle(tr("Play") + " ...");
    playMenu_->setToolTip(tr("Play menu"));
    ui->setContextMenuStyle(playMenu_, true); // persistent = true
  }

  audioTrackMenu_ = new QMenu(this); {
    audioTrackMenu_->setTitle(tr("Audio Track") + " ...");
    audioTrackMenu_->setToolTip(tr("Audio Track"));
    ui->setContextMenuStyle(audioTrackMenu_, true); // persistent = true
  }

  audioChannelMenu_ = new QMenu(this); {
    audioChannelMenu_->setTitle(tr("Audio Channel") + " ...");
    audioChannelMenu_->setToolTip(tr("Audio Channel"));
    ui->setContextMenuStyle(audioChannelMenu_, true); // persistent = true

    audioChannelMenu_->addAction(setStereoChannelAct_);
    audioChannelMenu_->addSeparator();
    audioChannelMenu_->addAction(setLeftChannelAct_);
    audioChannelMenu_->addAction(setRightChannelAct_);
    audioChannelMenu_->addSeparator();
    audioChannelMenu_->addAction(setReverseStereoChannelAct_);
    audioChannelMenu_->addAction(setDolbysChannelAct_);
  }

  audioMenu_ = new QMenu(this); {
    audioMenu_->setTitle(tr("Adjust Audio") + " ...");
    audioMenu_->setToolTip(tr("Adjust Audio"));
    ui->setContextMenuStyle(audioMenu_, true); // persistent = true

    audioMenu_->addMenu(audioTrackMenu_);
    audioMenu_->addMenu(audioChannelMenu_);
    audioMenu_->addSeparator();
    audioMenu_->addAction(setAudioDelayAct_);
  }

  sectionMenu_ = new QMenu(this); {
    sectionMenu_->setIcon(QIcon(RC_IMAGE_SECTION));
    sectionMenu_->setTitle(TR(T_MENUTEXT_SECTION) + " ...");
    sectionMenu_->setToolTip(TR(T_TOOLTIP_SECTION));
    ui->setContextMenuStyle(sectionMenu_, true); // persistent = true
  }

  closeMenu_ = new QMenu(this); {
    closeMenu_->setTitle(tr("After Finished Playing") + " ...");
    closeMenu_->setToolTip(tr("After finished playing all files in the same folder"));
    ui->setContextMenuStyle(closeMenu_, true); // persistent = true

    closeMenu_->addAction(nothingAfterFinishedAct_);
    closeMenu_->addSeparator();
    closeMenu_->addAction(sleepAfterFinishedAct_);
    closeMenu_->addAction(shutdownAfterFinishedAct_);
  }

  utilityMenu_ = new QMenu(this); {
    utilityMenu_->setTitle(tr("Utilities") + " ...");
    utilityMenu_->setToolTip(tr("Utilities"));
    ui->setContextMenuStyle(utilityMenu_, true); // persistent = true

    utilityMenu_->addMenu(closeMenu_);
    utilityMenu_->addSeparator();
    utilityMenu_->addAction(openProxyBrowserAct_);
    utilityMenu_->addAction(showDownloaderAct_);
    utilityMenu_->addAction(showTranslatorAct_);
    utilityMenu_->addAction(toggleMagnifierVisibleAct_);
    utilityMenu_->addAction(showAnnotationEditorAct_ );
    utilityMenu_->addAction(downloadAnnotationsAct_);
    utilityMenu_->addAction(showLibraryAct_);

#ifndef Q_OS_MAC
    utilityMenu_->addAction(newWindowAct_);
#endif // !Q_OS_MAC
  }

  settingsMenu_ = new QMenu(this); {
    settingsMenu_->setTitle(tr("Settings") + " ...");
    settingsMenu_->setToolTip(tr("Settings"));
    ui->setContextMenuStyle(settingsMenu_, true); // persistent = true

    settingsMenu_->addMenu(translatorMenu_);
    settingsMenu_->addSeparator();

    //settingsMenu_->addAction(tr("Preferences"), this, SLOT(showPreferences()), QKeySequence("CTRL+,"));
    //settingsMenu_->addAction(toggleSiteAccountViewVisibleAct_);
    //settingsMenu_->addAction(toggleNetworkProxyDialogVisibleAct_);

    settingsMenu_->addAction(togglePreferLocalDatabaseAct_);
    settingsMenu_->addAction(toggleClipboardMonitorEnabledAct_);
    settingsMenu_->addAction(toggleSaveMediaAct_);
    settingsMenu_->addAction(toggleSaveAnnotationFileAct_);
    settingsMenu_->addAction(toggleSubmitAct_);

    settingsMenu_->addSeparator();
//#ifndef Q_OS_MAC
//    settingsMenu_->addAction(toggleMultipleWindowsEnabledAct_);
//#endif // Q_OS_MAC

#ifndef Q_OS_MAC
    settingsMenu_->addAction(toggleWindowOnTopAct_);
#endif // Q_OS_MAC
#ifndef Q_OS_LINUX
    settingsMenu_->addAction(toggleEmbeddedPlayerOnTopAct_); // FIXME: X11::zeroInput problem
#endif // Q_OS_LINUX
    //settingsMenu_->addAction(togglePlayerLabelEnabled_);

#ifndef Q_OS_MAC
    settingsMenu_->addAction(toggleMenuBarVisibleAct_);
#endif // !Q_OS_MAC
    settingsMenu_->addAction(toggleMenuThemeEnabledAct_);
#ifdef Q_OS_WIN
    if (QtWin::isWindowsVistaOrLater())
      settingsMenu_->addAction(toggleAeroEnabledAct_);
#endif // Q_OS_WIN
    settingsMenu_->addMenu(themeMenu_);
  }

  annotationMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(annotationMenu_, true); // persistent = true

    annotationMenu_->setTitle(tr("Annotation Information") + " ...");
    annotationMenu_->setToolTip(tr("Annotation Information"));
  }

  annotationSettingsMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(annotationSettingsMenu_, true); // persistent = true

    annotationSettingsMenu_->setTitle(tr("Annotation Settings") + " ...");
    annotationSettingsMenu_->setToolTip(tr("Annotation Settings"));

    //annotationSettingsMenu_->addMenu(annotationEffectMenu_);
    annotationSettingsMenu_->addAction(togglePreferFloatAnnotationAct_);
    annotationSettingsMenu_->addAction(toggleAnnotationAvatarVisibleAct_);
    annotationSettingsMenu_->addAction(toggleAnnotationMetaVisibleAct_);
    annotationSettingsMenu_->addAction(toggleAnnotationBandwidthLimitedAct_);
    annotationSettingsMenu_->addAction(toggleAnnotationTraditionalChineseAct_);
    annotationSettingsMenu_->addSeparator();
    annotationSettingsMenu_->addAction(toggleSubtitleAnnotationVisibleAct_);
    annotationSettingsMenu_->addAction(toggleSubtitleOnTopAct_);
    annotationSettingsMenu_->addMenu(subtitleColorMenu_);
    //annotationSettingsMenu_->addAction(toggleNonSubtitleAnnotationVisibleAct_);
    annotationSettingsMenu_->addSeparator();
    annotationSettingsMenu_->addAction(increaseAnnotationScaleAct_);
    annotationSettingsMenu_->addAction(decreaseAnnotationScaleAct_);
    annotationSettingsMenu_->addAction(resetAnnotationScaleAct_);
    annotationSettingsMenu_->addSeparator();
    annotationSettingsMenu_->addAction(increaseAnnotationRotationAct_);
    annotationSettingsMenu_->addAction(decreaseAnnotationRotationAct_);
    annotationSettingsMenu_->addAction(resetAnnotationRotationAct_);
    annotationSettingsMenu_->addSeparator();
    annotationSettingsMenu_->addAction(increaseAnnotationOffsetAct_);
    annotationSettingsMenu_->addAction(decreaseAnnotationOffsetAct_);
    annotationSettingsMenu_->addAction(resetAnnotationOffsetAct_);
  }


//#ifndef Q_OS_WIN
  // Use menuBar_ instead MainWindow::menuBar() to enable customization.
  // It is important to create the menubar WITHOUT PARENT, so that diff windows could share the same menubar.
  // See: http://doc.qt.nokia.com/latest/mac-differences.html#menu-bar
  // See: http://doc.qt.nokia.com/stable/qmainwindow.html#menuBar

  // jichi 11/11/11:  It's important not to use act like quitAct_ or aboutAct_ with translations.

  //menuBar_ = new QMenuBar;
  QMenu *m;
  m = menuBar()->addMenu(tr("&File")); {
    m->addAction(showLibraryAct_);
    m->addSeparator();
    m->addAction(openFileAct_);
    m->addAction(openUrlAct_);
    m->addSeparator();
    m->addAction(openVideoDeviceAct_);
#ifdef Q_OS_WIN // TODO add support for Mac/Linux
    m->addAction(openAudioDeviceAct_);
#endif // Q_OS_WIN
    m->addAction(openDeviceAct_);
    m->addSeparator();
    m->addAction(openSubtitleAct_);
    m->addAction(openAnnotationUrlAct_);
    m->addAction(downloadAnnotationsAct_);
    m->addSeparator();
    m->addAction(tr("&Quit"), this, SLOT(close())); // DO NOT TRANSLATE ME
  }

  //editMenu = menuBar()->addMenu(tr("&Edit"));
  //editMenu->addAction(undoAct);
  //editMenu->addAction(insertAct);
  //editMenu->addAction(analyzeAct);
  //editMenu->addAction(transformAct);
  //editMenu->addAction(applyAct);
  //editMenu->addAction(generateAct);
  //editMenu->addAction(clearAct);

  //viewMenu_ = menuBar()->addMenu(tr("&View"));
  m = menuBar()->addMenu(tr("&Settings")); {
    m->addAction(tr("&Preferences"), this, SLOT(showPreferences()), QKeySequence("CTRL+,"));
  }

  m = menuBar()->addMenu(tr("&Help"));
  m->addAction(tr("&Help"), this, SLOT(help())); // DO NOT TRANSLATE ME
  m->addAction(tr("&About"), this, SLOT(about())); // DO NOT TRANSLATE ME
//#endif // !Q_OS_WIN

  openButtonMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(openButtonMenu_, true); // persistent = true
    openButtonMenu_->setTitle(TR(T_OPEN));
    openButtonMenu_->setToolTip(TR(T_OPEN));
    openButtonMenu_->setIcon(QIcon(RC_IMAGE_OPEN));

    openButtonMenu_->addAction(showLibraryAct_);
#ifdef AC_ENABLE_GAME
    openButtonMenu_->addSeparator();
    openButtonMenu_->addAction(showProcessAct_);
    openButtonMenu_->addAction(showSyncGameAct_);
#endif // AC_ENABLE_GAME
    openButtonMenu_->addSeparator();
    openButtonMenu_->addAction(openFileAct_);
    openButtonMenu_->addAction(openUrlAct_);
    openButtonMenu_->addSeparator();
    openButtonMenu_->addAction(openVideoDeviceAct_);
#ifdef Q_OS_WIN // TODO add support for Mac/Linux
    openButtonMenu_->addAction(openAudioDeviceAct_);
#endif // Q_OS_WIN
    openButtonMenu_->addAction(openDeviceAct_);
    openButtonMenu_->addSeparator();
    openButtonMenu_->addAction(downloadAnnotationsAct_);
    openButtonMenu_->addSeparator();
    openButtonMenu_->addAction(openSubtitleAct_);
    openButtonMenu_->addAction(openAnnotationFileAct_);
    openButtonMenu_->addAction(openAnnotationUrlAct_);
  }

  // PlayerUI
  embeddedPlayer_->menuButton()->setMenu(contextMenu_);
  mainPlayer_->menuButton()->setMenu(contextMenu_);
  miniPlayer_->menuButton()->setMenu(contextMenu_);
  embeddedPlayer_->openButton()->setMenu(openButtonMenu_);
  mainPlayer_->openButton()->setMenu(openButtonMenu_);
  miniPlayer_->openButton()->setMenu(openButtonMenu_);
}

// jichi:10/17/2011: TODO: Totally remove dock widgets; use layout instead
void
MainWindow::createDockWindows()
{
  // Main view
  setCentralWidget(videoView_);

  // Osd view
  //osdDock_ = new OsdDock(this);
  //osdDock_->setWidget(osdWindow_);
  //embeddedPlayer_->hide();
  //annotationView_->setFullScreenView(osdDock_);
  //osdDock_->showFullScreen();

  // Main player
  QDockWidget *mainPlayerDock  = new MainPlayerDock(this);
  mainPlayerDock->setWidget(mainPlayer_);
  addDockWidget(Qt::BottomDockWidgetArea, mainPlayerDock);

  // Main player
  QDockWidget *libraryDock = new MainLibraryDock(this);
  libraryDock->setWidget(libraryView_);
  addDockWidget(Qt::RightDockWidgetArea, libraryDock);
  libraryView_->hide();

  // Mini player
  //miniPlayerDock_ = new MiniPlayerDock(this);
  //miniPlayerDock_->setWidget(miniPlayer_);
  //miniPlayer_->setDockWidget(miniPlayerDock_);
  //miniPlayerDock_->hide();
  //viewMenu_->addAction(miniPlayerDock_->toggleViewAction());

  //userPanelDock_->setWidget(userPanel_);
  //userPanel_->setDockWidget(userPanelDock_);
  //userPanelDock_->hide();
  //viewMenu_->addAction(userPanelDock_->toggleViewAction());
}


// - Hub events -

void
MainWindow::updateTokenMode()
{
  // Stop
  if (hub_->isMediaTokenMode() && !player_->isStopped())
    player_->stop();

#ifdef WITH_WIN_TEXTHOOK
  if (!hub_->isSignalTokenMode()) {
    messageHandler_->setActive(false);
    TextHook::globalInstance()->setEnabled(true);
  }
#endif // WITH_WIN_TEXTHOOK

  //if (!hub_->isLiveTokenMode())
  //  closeChannel();

  annotationView_->invalidateAnnotations();

  // Restart

  switch (hub_->tokenMode()) {
  case SignalHub::MediaTokenMode:
#ifdef WITH_WIN_MOUSEHOOK
    MouseHook::globalInstance()->stop();
#endif // WITH_WIN_MOUSEHOOK
    break;
  case SignalHub::LiveTokenMode:
  case SignalHub::SignalTokenMode:
    if (annotationView_->trackedWindow())
      hub_->setEmbeddedPlayerMode();
#ifdef WITH_WIN_MOUSEHOOK
    MouseHook::globalInstance()->start();
#endif // WITH_WIN_MOUSEHOOK
    break;
  }

  clickListener_->setEnabled(hub_->isMediaTokenMode());
  if (!hub_->isMediaTokenMode())
    hub_->play();
}

void
MainWindow::updatePlayMode()
{
  windowTitleSuffix_.clear();
  //switch (hub_->playerMode()) {
  //case SignalHub::LivePlayMode:
  //  openChannel();
  //  break;
  //case SignalHub::NormalPlayMode:
  //default:
  //  closeChannel();
  //}
}

void
MainWindow::invalidatePlayerMode()
{
  switch (hub_->playerMode()) {
  case SignalHub::NormalPlayerMode:
  case SignalHub::MiniPlayerMode:
    if (annotationView_->trackedWindow())
      hub_->setEmbeddedPlayerMode();
    else
      hub_->setNormalPlayerMode();
    break;
  case SignalHub::EmbeddedPlayerMode:
    if (!annotationView_->trackedWindow())
      hub_->setNormalPlayerMode();
    break;
  }
}

void
MainWindow::updateOsdWindowOnTop()
{ osdWindow_->setWindowOnTop(annotationView_->trackedWindow()); }

void
MainWindow::updatePlayerMode()
{
  switch (hub_->playerMode()) {
  case SignalHub::NormalPlayerMode:
    embeddedPlayer_->hide();
    miniPlayer_->hide();
    mainPlayer_->show();
    if (hub_->isNormalWindowMode()) {
      DOUT("showNormal");
      showNormal();
    } else {
      DOUT("show");
      show();
    }
    break;

  case SignalHub::MiniPlayerMode:
    //setVisible(v);
    mainPlayer_->hide();
    embeddedPlayer_->hide();
    miniPlayer_->show();
    if (!annotationView_->trackedWindow()) {
      if (hub_->isNormalWindowMode()) {
        DOUT("showNormal");
        showNormal();
      } else {
        DOUT("show");
        show();
      }
    }
    break;

  case SignalHub::EmbeddedPlayerMode:
    //setVisible(m);
    if (annotationView_->trackedWindow())
      hide();
    mainPlayer_->hide();
    miniPlayer_->hide();
    embeddedPlayer_->show();
    osdWindow_->show();
    osdWindow_->raise();
    break;
  }

  annotationView_->updateGeometry();
}

void
MainWindow::updateWindowMode()
{
  windowModeUpdateTime_ = QDateTime::currentMSecsSinceEpoch();

  switch (hub_->windowMode()) {
  case SignalHub::FullScreenWindowMode:
#ifdef WITH_WIN_DWM
    if (AcUi::isAeroAvailable())
      AcUi::globalInstance()->setWindowDwmEnabled(this, false);
#endif // WITH_WIN_DWM
#ifdef Q_OS_LINUX
    if (hub_->isMediaTokenMode() && player_->hasMedia())
      AcUi::globalInstance()->setBlackBackground(this);
#endif // Q_OS_LINUX

    if (mainPlayer_->isVisible())
      mainPlayer_->hide();

    switch (hub_->tokenMode()) {
    case SignalHub::MediaTokenMode:
      DOUT("showFullScreen");
      showFullScreen();
      if (hub_->isNormalPlayerMode())
        hub_->setEmbeddedPlayerMode();
      annotationView_->setFullScreenMode();
      break;
    case SignalHub::LiveTokenMode:
    case SignalHub::SignalTokenMode:
#ifndef Q_OS_LINUX
      hub_->setEmbeddedPlayerMode(); // always embed in full screen signal mode
      annotationView_->setFullScreenMode();
#endif // Q_OS_LINUX
      break;
    }
    break;

  case SignalHub::NormalWindowMode:
    //if (embeddedPlayer_->isVisible())
    //  embeddedPlayer_->hide();

    annotationView_->setFullScreenMode(false);

    if (!annotationView_->trackedWindow()) {
      if (!hub_->isMediaTokenMode() ||
          !player_->isPlaying())
        hub_->setNormalPlayerMode();
      else if (hub_->isNormalWindowMode()) {
        DOUT("showNormal");
        showNormal();
      } else {
        DOUT("show");
        show();
      }

      QSize sz = size();
      QSize screen = QApplication::desktop()->availableGeometry(this).size();
      if (hub_->isEmbeddedPlayerMode() && !annotationView_->trackedWindow() &&
          isPlaying() &&
          (isMaximized() ||
          (sz == PREFERRED_WINDOW_SIZE ||
           sz.width() > screen.width() - 100 || sz.height() > screen.height() - 50))) {
        sz = player_->videoDimension();
        if (sz.isEmpty() || sz.width() < PREFERRED_MIN_WIDTH || sz.height() < PREFERRED_MIN_HEIGHT ||
            isMaximized() ||
            sz.width() > screen.width() - 100 || sz.height() > screen.height() - 50)
          sz = PREFERRED_WINDOW_SIZE;
        if (sz != size()) {
          move(pos().x() + (width() - sz.width()) / 2,
               pos().y() + (height() - sz.height()) / 2);
          resize(sz);
          //QTimer::singleShot(0, embeddedPlayer_, SLOT(updateGeometry()));
        }
      }
    } else {
      hide();
      updatePlayerMode();
      osdWindow_->show();
      osdWindow_->raise();
    }

    //autoHideCursorTimer_->stop();
    //showCursor();
    break;
  }

  embeddedPlayer_->hide();
  QTimer::singleShot(0, embeddedPlayer_, SLOT(hide()));
  dragPos_ = BAD_POS;
}

/*
void
MainWindow::setVisible(bool visible)
{
  if (visible == isVisible())
    return;

  Base::setVisible(visible);

  if (!hub_->isFullScreenWindowMode()) {
#ifdef WITH_WIN_DWM
    if (AcUi::isAeroAvailable())
      AcUi::globalInstance()->setWindowDwmEnabled(this, true);
    else
#endif // WITH_WIN_DWM
      AcUi::globalInstance()->setWindowBackground(this, false); // persistent is false
  }
}
*/

// - Open file -

void
MainWindow::open()
{
  switch (hub_->tokenMode()) {
  case SignalHub::LiveTokenMode:
  case SignalHub::MediaTokenMode: openFile(); break;
  case SignalHub::SignalTokenMode: openProcess(); break;
  }
  //showLibrary();
}

UrlDialog*
MainWindow::mediaUrlDialog()
{
  static UrlDialog *w = nullptr;
  if (!w) {
    w = new MediaUrlDialog(this);
    connect(w, SIGNAL(urlEntered(QString)), SLOT(openUrl(QString)));
    windows_.append(w);
  }
  //mediaUrlDialog_->setSave(player_->isBufferSaved());
  return w;
}

void
MainWindow::openUrl()
{
  UrlDialog *d = mediaUrlDialog();
  if (d->isEmpty()) {
    QString url = dataManager_->token().url();
    if (!url.isEmpty())
      d->setText(url);
  }
  d->show();
}


UrlDialog*
MainWindow::annotationUrlDialog()
{
  static UrlDialog *w = nullptr;
  if (!w) {
    w = new AnnotUrlDialog(this);
    connect(w, SIGNAL(urlEntered(QString)), SLOT(openAnnotationUrlAndSubmit(QString)));
    connect(this, SIGNAL(annotationUrlAdded(QString)), w, SLOT(addHistory(QString)), Qt::QueuedConnection);
    windows_.append(w);
  }
  //annotationUrlDialog_->setSave(submit_);
  return w;
}

UrlDialog*
MainWindow::annotationDownloaderDialog()
{
  static UrlDialog *w = nullptr;
  if (!w) {
    w = new AnnotDownUrlDialog(this);
    connect(w, SIGNAL(urlEntered(QString)), annotationDownloader_, SLOT(download(QString)));
    //connect(this, SIGNAL(annotationUrlAdded(QString)), w, SLOT(addHistory(QString)), Qt::QueuedConnection);
    windows_.append(w);
  }
  return w;
}

void
MainWindow::downloadAnnotations()
{
  UrlDialog *w = annotationDownloaderDialog();
  QString url = currentUrl();
  if (!url.isEmpty())
    w->setText(url);
  w->show();
}

void
MainWindow::openAnnotationUrl()
{ annotationUrlDialog()->show(); }

void
MainWindow::openAnnotationUrlFromAliases(const AliasList &l, const QHash<QString, QString> &cache, bool async)
{
  if (!l.isEmpty()) {
    int site;
    foreach (const Alias &a, l)
      if (a.type() == Alias::AT_Url &&
          (site = MrlAnalysis::matchSite(a.text())) && site < MrlAnalysis::AnnotationSite) {
        QString url = DataManager::normalizeUrl(a.text());
        if (!cache.isEmpty()) {
          auto p = cache.find(url);
          if (p != cache.end()) {
            if (async)
              emit openAnnotationFileRequested(p.value());
            else
              openAnnotationFile(p.value());
            continue;
          }
        }
        if (async)
          emit openAnnotationUrlRequested(url);
        else
          openAnnotationUrl(url);
      }
  }
}

void
MainWindow::openAnnotationUrl(const QString &input, bool submit)
{
  if (!hub_->isMediaTokenMode()) {
    emit warning(tr("importing annotations to Galgame is not allowed"));
    return;
  }
  QString url = DataManager::normalizeUrl(input);
  emit message(tr("analyzing URL ...") + " " + url);
  if (registerAnnotationUrl(url)) {
    bool ok = mrlResolver_->resolveSubtitle(url);
    if (!ok)
      emit warning(tr("failed to resolve URL") + ": " + url);
    else if (submit)
      submitAliasText(url, Alias::AT_Url);

    emit annotationUrlAdded(url);

    if (isVisible() && !isMinimized() && !isFullScreen() && !isWindowOnTop()) {
      raise();
      osdWindow_->raise();
    }
  }
}

//void
//MainWindow::openUrl(const QString &url, bool save)
//{
//  player_->setBufferSaved(save);
//  openUrl(url);
//}

void
MainWindow::openUrl(const QString &input)
{
  if (libraryView_->isVisible() && libraryView_->autoHide())
    libraryView_->hide();

  QString url = DataManager::normalizeUrl(input);
  if (url.startsWith("ttp://"))
    url.prepend('h');
  if (mrlResolver_->resolveMedia(url))
    emit message(tr("analyzing URL ...") + " " + url);
  else {
    if (isRemoteMrl(url))
      openMrl(url, false); // checkPath = false
    else
      openLocalUrl(url);
  }
}

void
MainWindow::openFile()
{
  if (!QFile::exists(recentPath_))
    recentPath_.clear();

  static const QString filter =
      TR(T_FORMAT_SUPPORTED) + ("(" G_FILTER_SUPPORTED ")" ";;")
#ifdef AC_ENABLE_GAME
    + TR(T_GALGAME)   + ("(" G_FILTER_PROGRAM ")" ";;")
#endif // AC_ENABLE_GAME
    + TR(T_FORMAT_VIDEO)     + ("(" G_FILTER_VIDEO ")" ";;")
    + TR(T_FORMAT_AUDIO)     + ("(" G_FILTER_AUDIO ")" ";;")
    + TR(T_FORMAT_PICTURE)   + ("(" G_FILTER_PICTURE ")" ";;")
    + TR(T_FORMAT_PREVIEW)   + ("(" G_FILTER_PREVIEW ")" ";;")
    + TR(T_FORMAT_ALL)       + ("(" G_FILTER_ALL ")");

  QStringList l = QFileDialog::getOpenFileNames(
        this, tr("Open File"), recentPath_, filter);

  if (!l.isEmpty()) {
    playlist_ = l;

    QString fileName = l.first();
    recentPath_ = QFileInfo(fileName).absolutePath();
    openSource(fileName);
  }
}

void
MainWindow::openAnnotationFile()
{
  if (!QFile::exists(recentPath_))
    recentPath_.clear();

  static const QString filter =
      TR(T_FORMAT_ANNOTATION) + ("(" G_FILTER_ANNOTATION ")" ";;")
    + TR(T_FORMAT_ALL)       + ("(" G_FILTER_ALL ")");

  QStringList l = QFileDialog::getOpenFileNames(
        this, tr("Open Annotation Files"), recentPath_, filter);

  if (!l.isEmpty()) {
    recentPath_ = QFileInfo(l.first()).absolutePath();
    foreach (const QString &fileName, l)
      openAnnotationFile(fileName);
  }
}

void
MainWindow::repairExternalAnnotationFileNames()
{
  foreach (const QString &f, externalAnnotationFiles()) {
    QString url = AnnotationCodecManager::parseAnnotatedUrl(f);
    if (!url.isEmpty())
      importAnnotationFile(f, url);
  }
}

void
MainWindow::importAnnotationFile(const QString &fileName, const QString &url)
{
  if (!hub_->isMediaTokenMode() || !player_->hasMedia())
    return;
  QString a_name = AnnotationCacheManager::hashFileName(url);
  if (a_name.isEmpty())
    return;
  QString path = player_->mediaPath();
  if (isRemoteMrl(path))
    return;
  QFileInfo m_fi(path);
  if (m_fi.isDir())
    return;
  QFileInfo a_fi(fileName);
  if (m_fi.absolutePath().compare(a_fi.absolutePath(), Qt::CaseInsensitive)) {
    QString newFile = m_fi.completeBaseName() + "." + a_name;
    newFile.prepend('/').prepend(m_fi.absolutePath());
    qxTrashOrRemoveFile(newFile);
    QFile::copy(fileName, newFile);
  } else {
    QString newFile = m_fi.completeBaseName() + "." + a_name;
    if (newFile.compare(a_fi.fileName(), Qt::CaseInsensitive)) {
      newFile.prepend('/').prepend(m_fi.absolutePath());
      qxTrashOrRemoveFile(newFile);
      QFile::rename(fileName, newFile);
    }
  }
}

void
MainWindow::openAnnotationFile(const QString &fileName, bool import)
{
  if (!hub_->isMediaTokenMode()) {
    emit warning(tr("importing annotations to Galgame is not allowed"));
    return;
  }
  QString url = AnnotationCodecManager::parseAnnotatedUrl(fileName);
  if (!url.isEmpty()) {
    if (import)
      importAnnotationFile(fileName, url);
    if (!registerAnnotationUrl(url)) {
      DOUT("annotation already imported: " << fileName);
      return;
    }
  } else if (!registerAnnotationFile(fileName)) {
    emit message(tr("annotations already imported") + ": " + fileName);
    return;
  }

  AnnotationList l = AnnotationCodecManager::parseFile(fileName);
  if (l.isEmpty()) {
    emit warning(tr("annotations not found") + ": " + fileName);
    return;
  }

  AnnotationList annots;
  const Token &t = dataManager_->token();
  if (t.hashId() || t.hasDigest())
    foreach (Annotation a, l) {
      a.setTokenId(t.id());
      a.setTokenSection(t.section());
      a.setTokenDigest(t.digest());
      annots.append(a);
    }
  else
    annots = l;

  QString src = QFileInfo(fileName).fileName();
  QString msg = QString("%1 :" HTML_SS_OPEN(color:red) "+%2" HTML_SS_CLOSE() ", %3")
    .arg(tr("annotations found"))
    .arg(QString::number(annots.size()))
    .arg(src);
  emit message(msg);
  emit addAnnotationsRequested(annots); // exit current event loop
  // Never annotations from file never go to cache
  //if (cache_->isValid() &&
  //    (t.hasId() || t.hasDigest()))
  //  cache_->insertAnnotations(annots);

  DOUT("exit");
}

void
MainWindow::openProcess()
{
#ifdef AC_ENABLE_GAME
  if (hub_->isMediaTokenMode() && player_->hasMedia())
    player_->closeMedia();
  showSyncGame();
#endif // AC_ENABLE_GAME
}

void
MainWindow::openWindow()
{
#ifdef AC_ENABLE_GAME
  showProcess();
#endif // AC_ENABLE_GAME
}

bool
MainWindow::isDevicePath(const QString &path)
{
#ifdef Q_OS_WIN
  if (path.contains(QRegExp("^[a-zA-Z]:$")) ||
      path.contains(QRegExp("^[a-zA-Z]:\\\\$")))
    return true;
#endif // Q_OS_MAC
#ifdef Q_OS_UNIX
  if (QtUnix::isDeviceFile(path))
    return true;
#endif // Q_OS_UNIX
  return false;
}

void
MainWindow::openDevice()
{
  static DeviceDialog *w = nullptr;
  if (!w) {
    w = new DeviceDialog(this);
    connect(w, SIGNAL(deviceSelected(QString,bool)), SLOT(openDevicePath(QString,bool)));
    AC_CONNECT_WARNING(w, this, Qt::AutoConnection);
    windows_.append(w);
  }
  w->show();
  w->raise();
}

void
MainWindow::openVideoDevice()
{
  if (!QFile::exists(recentPath_))
    recentPath_.clear();

  QString path = QFileDialog::getExistingDirectory(
        this, TR(T_TITLE_OPENVIDEODEVICE), recentPath_);

  if (!path.isEmpty()) {
    playlist_.clear();

    recentPath_ = path;
    openDevicePath(path, false); // isAudioCD == false
  }
}

void
MainWindow::openAudioDevice()
{
  if (!QFile::exists(recentPath_))
    recentPath_.clear();

  QString path = QFileDialog::getExistingDirectory(
        this, TR(T_TITLE_OPENAUDIODEVICE), recentPath_);

  if (!path.isEmpty()) {
    playlist_.clear();

    recentPath_ = path;
    openDevicePath(path, true); // isAudioCD == true
  }
}

void
MainWindow::openDevicePath(const QString &path, bool isAudioCD)
{
  //if (!isDevicePath(path)) {
  //  emit warning(TR(T_ERROR_BAD_DEVICEPATH) + ": " + path);
  //  return;
  //}

  playlist_.clear();

  QString mrl = path;
  if (isAudioCD && !mrl.startsWith(PLAYER_URL_CD, Qt::CaseInsensitive))
    mrl = PLAYER_URL_CD + path;

  recentUrlLocked_ = false;
  openMrl(mrl);
}

void
MainWindow::openDirectory()
{
  if (!QFile::exists(recentPath_))
    recentPath_.clear();

  QString path = QFileDialog::getExistingDirectory(
        this, TR(T_TITLE_OPENAUDIODEVICE), recentPath_);

  if (!path.isEmpty()) {
    playlist_.clear();

    recentPath_ = path;
    recentUrlLocked_ = false;
    openMrl(path);
  }
}

void
MainWindow::openSubtitle()
{
  if (!QFile::exists(recentPath_))
    recentPath_.clear();

  static const QString filter =
      TR(T_FORMAT_SUBTITLE)  + ("(" G_FILTER_SUBTITLE ")" ";;")
    + TR(T_FORMAT_ALL)       + ("(" G_FILTER_ALL ")");

  QStringList l = QFileDialog::getOpenFileNames(
        this, tr("Open Subtitles"), recentPath_, filter);

  if (!l.isEmpty()) {
    recentPath_ = QFileInfo(l.first()).absolutePath();
    foreach (const QString &fileName, l)
      openSubtitleFile(fileName);
  }
}

void
MainWindow::openLocalUrl(const QUrl &url)
{
  if (!url.isValid())
    return;

  recentUrlLocked_ = false;
  QString path = url.toLocalFile();
  DOUT("url =" << url << ", path =" << path);
  openSource(path.isEmpty() ? url.toString() : path);
}

void
MainWindow::openLocalUrls(const QList<QUrl> &urls)
{
  // FIXME: add to playlist rather then this approach!!!!!
  if (!urls.empty()) {
    QStringList subs;
    QStringList playlist;
    foreach (const QUrl &url, urls) {
      QString fileName = url.toLocalFile();
      if (!fileName.isEmpty()) {
        if (Player::isSupportedSubtitle(fileName))
          subs.append(fileName);
        else
          playlist.append(fileName);
      }
    }
    switch (playlist.size()) {
    case 0:
      if (!subs.isEmpty())
        openLocalUrl(subs.first());
      break;
    case 1:
      openLocalUrl(playlist.first());
      if (!subs.isEmpty()) {
        for (auto p = subs.constEnd(); p != subs.constBegin();
             openSubtitleFile(*--p));
      }
      break;
    default:
      playlist_ = playlist;
      openLocalUrl(playlist_.first());
    }
  }
}

void
MainWindow::openMimeData(const QMimeData *mime)
{
  recentUrlLocked_ = false;
  if (mime && mime->hasUrls())
    openLocalUrls(mime->urls());
}


void
MainWindow::setRecentOpenedFile(const QString &path)
{
  recentOpenedFile_ = path;
  if (playlist_.size() > 1)
    updatePlaylistMenu();
}

void
MainWindow::openRemoteMedia(const MediaInfo &mi, QNetworkCookieJar *jar)
{
  DOUT("refurl =" << mi.refurl << ", mrls.size =" << mi.mrls.size());
  recentUrlLocked_ = false;
  playlist_.clear();
  if (mi.mrls.isEmpty()) {
    emit errorMessage(tr("failed to resolve URL") + ": " + mi.refurl);
    DOUT("exit: empty mrls");
    return;
  }
  //if (mi.mrls.size() > 1) {
  //  foreach (const MrlInfo &mrl, mi.mrls)
  //    playlist_.append(mrl.url);
  //}

  //if (nam && mi.mrls.size() > 1) {
  //  if (!mi.title.isEmpty() && !isRemoteMrl(mi.title))
  //    player_->setMediaTitle(mi.title);
  //  recentDigest_.clear();
  //  recentUrl_.clear();
//
  //  MrlInfo mrl = mi.mrls.first();
  //  DOUT("mrl =" << mrl.url);
//
  //  MediaStreamInfo msi;
  //  foreach (const MrlInfo &mrl, mi.mrls) {
  //    if (mrl.url.isEmpty())
  //      continue;
  //    msi.urls.append(mrl.url);
  //    msi.durations.append(mrl.duration);
  //    msi.sizes.append(mrl.size);
  //  }
  //  MediaStreamer::globalInstance()->stream(msi, nam);
  //  return;
  //}

  if (!player_->isValid())
    resetPlayer();

  if (player_->hasMedia())
    closeMedia();

  if (jar)
    player_->setCookieJar(jar);
  if (!mi.title.isEmpty() && !isRemoteMrl(mi.title))
    player_->setMediaTitle(mi.title);

  recentDigest_.clear();
  recentUrl_.clear();
  if (!mi.refurl.isEmpty()) {
    addRecent(mi.refurl);
    recentUrl_ = mi.refurl;
    recentUrlLocked_ = true;
    //setToken();
  }

  if (!mi.mrls.isEmpty()) {
    QStringList urls;
    qint64 duration = 0;
    foreach (const MrlInfo &i, mi.mrls) {
      urls.append(i.url);
      duration += i.duration;
    }
    player_->setStream(urls, mi.refurl, duration);
  }
  openMrl(mi.mrls.first().url, false); // checkPath = false

  //importAnnotationsFromUrl(mi.suburl);
  if (!mi.suburl.isEmpty())
    QTimer::singleShot(5000, // TODO: use event loop in aother thread rather than 5 sec
      new detail::ImportAnnotationsFromUrl(mi.suburl, mi.refurl, this),
      SLOT(trigger())
    );

  //static bool once = true;
  //if (once) {
  //  once = false;
  //  QTimer::singleShot(0, this, SLOT(updateContextMenu()));
  //}
}

void
MainWindow::closeMedia()
{
  if (player_->hasMedia()) {
    player_->closeMedia();

    //DOUT("processEvent: enter");
    //enum { timeout = 1000 };
    //qApp->processEvents(QEventLoop::AllEvents, timeout);
    //DOUT("processEvent: exit");
  }

  dataManager_->clearToken();
  dataManager_->removeAliases();
  annotationView_->invalidateAnnotations();

}

void
MainWindow::removeAliasAnnotationFile(const Alias &a)
{
  if (a.type() == Alias::AT_Url && a.hasText())
    removeAnnotationFile(a.text());
}

void
MainWindow::removeAnnotationFile(const QString &url)
{
  QStringList l = externalAnnotationFiles();
  if (!l.isEmpty()) {
    QString key = DataManager::normalizeUrl(url);
    foreach (const QString &f, l) {
      QString that = AnnotationCodecManager::parseAnnotatedUrl(f);
      if (!that.isEmpty() && DataManager::normalizeUrl(that) == key) {
        qxTrashOrRemoveFile(f);
        emit message(tr("annotation file removed") + ": " + f);
      }
    }
  }
}

//void
//MainWindow::loadExternalAnnotations()
//{
//  foreach (const QString &f, externalAnnotationFiles())
//    openAnnotationFile(f);
//}

bool
MainWindow::hasExternalAnnotationFiles() const
{
  if (!player_->hasMedia())
    return false;

  QString path = (player_->mediaPath());
  if (isRemoteMrl(path))
    return false;

  DOUT("enter");
  QFileInfo fi(path);
  QDir dir = fi.absoluteDir();
  if (!dir.exists()) {
    DOUT("exit: dir does not exist");
    return false;
  }

  QString baseName = fi.completeBaseName();
  static const QStringList filters = QStringList() G_FORMAT_ANNOTATION(<< "*.");
  foreach (const QString &f, dir.entryList(filters, QDir::Files))
    if (f.startsWith(baseName + ".")) {
      DOUT("exit: found annotation file");
      return true;
    }
  DOUT("exit: not found");
  return false;
}


QStringList
MainWindow::externalAnnotationFiles() const
{
  if (!player_->hasMedia())
    return QStringList();

  QString path = (player_->mediaPath());
  if (isRemoteMrl(path))
    return QStringList();

  DOUT("enter");
  QFileInfo fi(path);
  QDir dir = fi.absoluteDir();
  if (!dir.exists()) {
    DOUT("exit: dir does not exist");
    return QStringList();
  }

  QStringList ret;
  QString baseName = fi.completeBaseName();
  static const QStringList filters = QStringList() G_FORMAT_ANNOTATION(<< "*.");
  foreach (const QString &f, dir.entryList(filters, QDir::Files))
    if (f.startsWith(baseName + ".")) {
      QString path = dir.absolutePath() + "/" + f;
      DOUT("found annotation file:" << path);
      ret.append(path);
    }
  DOUT("exit");
  return ret;
}

QStringList
MainWindow::externalAnnotationUrls() const
{
  QStringList ret;
  foreach (const QString &f, externalAnnotationFiles()) {
    QString url = AnnotationCodecManager::parseAnnotatedUrl(f);
    if (url.isEmpty())
      continue;
    url = DataManager::normalizeUrl(url);
    if (!ret.contains(url))
      ret.append(url);
  }
  return ret;
}

void
MainWindow::openStreamUrl(const QString &rtsp)
{ openMrl(rtsp, false); }

void
MainWindow::openMrl(const QString &input, bool checkPath)
{
  DOUT("enter: input =" << input);
  QString path = QDir::fromNativeSeparators(input);

  disableNavigation();

  if (!annotationView_->trackedWindow()) {
    if (!isVisible()) {
      DOUT("show");
      show();
    } if (isMinimized()) {
      DOUT("showNormal");
      showNormal();
    }
  }

  bool fullScreen = hub_->isLiveTokenMode() && hub_->isEmbeddedPlayerMode();
  hub_->setMediaTokenMode();
  if (fullScreen) {
    hub_->setFullScreenWindowMode();
    updateWindowMode();
  }

  recentDigest_.clear();
  if (!recentUrlLocked_)
    recentUrl_.clear();

  setRecentOpenedFile(path);

  if (!recentUrl_.isEmpty())
    setBrowsedUrl(recentUrl_);
  else
    setBrowsedFile(path);

  if (checkPath && !isRemoteMrl(path)) {
    QString file = removeUrlPrefix(path);
    QFileInfo fi(file);
    if (!fi.exists()) {
      emit warning(TR(T_ERROR_BAD_FILEPATH) + ": " + file);
      DOUT("exit: path not exist: " + file);
      return;
    }
    if (fi.isFile() && !Player::isSupportedFile(file))
      emit warning(tr("play file with unknown extension") + ": "
        HTML_SS_OPEN(color:orange) + fi.suffix() + HTML_SS_CLOSE()
      );
  }

  tokenType_ = fileType(path);

  if (!player_->isValid())
    resetPlayer();

  DOUT("invoke openMedia");
  if (player_->hasMedia())
    closeMedia();

  // Forward computing file digest to avoid IO conflict with VLC player
  static bool once = true;
  if (once)
    once = false;
  else if (recentDigest_.isEmpty() && isDigestReady(path)) {
    emit messageOnce(tr("Analyzing ..."));
    emit message(tr("analyzing") + ": " + path);
    recentDigest_ = digestFromFile(removeUrlPrefix(path));
  }

  bool ok = player_->openMedia(path);
  if (!ok) {
    emit warning(tr("failed to open media") + ": " + path);
    DOUT("exit: openMedia failed");
    return;
  }

  bool remote = isRemoteMrl(path);
  if (!remote)
    addRecent(path);

  if (player_->hasPlaylist()) {
    DOUT("exit: media has playlist");
    return;
  }

  if (player_->hasMedia()) {
    repairExternalAnnotationFileNames();
    if (player_->isMouseEventEnabled())
      player_->startVoutTimer();
    DOUT("invoke invalidateMediaAndPlay");
    invalidateMediaAndPlay();
  }

  if (isVisible() && !isMinimized() && !isFullScreen() && !isWindowOnTop()) {
    raise();
    osdWindow_->raise();
  }
  DOUT("exit");
}

bool
MainWindow::isPosOutOfScreen(const QPoint &globalPos) const
{
  QRect screen = QApplication::desktop()->availableGeometry(this);
  return !screen.contains(globalPos);
}

bool
MainWindow::isRectOutOfScreen(const QRect &globalRect) const
{
  QRect screen = QApplication::desktop()->availableGeometry(this);
  return !screen.intersects(globalRect);
}

bool
MainWindow::isDigestReady(const QString &path) const
{
  if (path.isEmpty())
    return false;
  if (isRemoteMrl(path))
    return false;
  if (Player::isSupportedPlaylist(path))
    return false;
  if (path.endsWith(".cda", Qt::CaseInsensitive))
    return false;

//#ifdef Q_OS_WIN
//  if (path.endsWith(':') || path.endsWith(":\\") || path.endsWith(":/"))
//    if (path.startsWith(PLAYER_URL_CD) ||
//        dataManager_->token().type() == Token::TT_Audio)
//      return false;
//#endif // Q_OS_WIN

  return true;
}

bool
MainWindow::isAliasReady(const QString &path) const
{
  QFileInfo fi(path);
  if (fi.isDir())
    return false;
  if (isDevicePath(path))
    return false;
  return true;
}

QString
MainWindow::digestFromFile(const QString &path)
{
  if (isRemoteMrl(path))
    return QString();
  QString digest = Token::digestFromFile(path);
  if (digest.isEmpty()) {
    QString localPath = QUrl(path).toLocalFile();
    QString decodedLocalPath = QUrl::fromPercentEncoding(localPath.toAscii());
    digest = Token::digestFromFile(decodedLocalPath);
  }
  return digest;
}

void
MainWindow::invalidateMediaAndPlay(bool async)
{
  DOUT("enter: async =" << async);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }

  if (player_->hasMedia()) {
    if (async) {
      //QTimer::singleShot(2000, this, SLOT(loadExternalAnnotations())); // offload CPU burden
      emit message(tr("analyzing media ...")); // might cause parallel contension
      QtConcurrent::run(this, &Self::invalidateMediaAndPlay, false);
      DOUT("exit: returned from async branch");
      return;
    }

    invalidateToken(player_->mediaPath());

    emit playMediaRequested();
  }
  DOUT("exit");
}

void
MainWindow::playMedia()
{
  DOUT("enter");
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (player_->hasMedia()) {
    QString path = player_->mediaPath();
    if (!isRemoteMrl(path))
      addRecent(path);
    //if (mode_ == NormalPlayMode)
    //  videoView_->resize(INIT_WINDOW_SIZE);

    player_->play();

//#ifdef Q_OS_WIN
//    if (!isRemoteMrl(path))
//      QTimer::singleShot(0, player_, SLOT(loadExternalSubtitles())); // load external subtitles later
//#endif // Q_OS_WIN
    //loadSubtitlesTimer_->start(3); // 3 times
    //updateWindowTitle();

    if (!isRemoteMrl(path) && Player::isSupportedAudio(path))
      resize(INIT_WINDOW_SIZE);
    else if (!annotationView_->trackedWindow())
      updateWindowSize();

#ifdef Q_OS_MAC
    // ensure videoView_ stretch in full screen mode
    if (hub_->isEmbeddedPlayerMode() && !videoView_->isViewVisible()) {
      videoView_->showView();
      hub_->setEmbeddedPlayerMode(false);
      hub_->setEmbeddedPlayerMode(true);
    }
#endif // Q_OS_MAC

#ifdef Q_OS_LINUX
    if (hub_->isFullScreenWindowMode())
      AcUi::globalInstance()->setBlackBackground(this);
#endif // Q_OS_LINUX

    windowTitleSuffix_.clear();
    updateWindowTitle();
    updateTrackMenu();
    loadSubtitlesLater();
  }
  DOUT("exit");
}

void
MainWindow::updateWindowSize()
{
  if (hub_->isNormalPlayerMode() && player_->hasMedia())
    actualSize();
}

void
MainWindow::openSubtitleFile(const QString &path)
{
  if (!player_->hasMedia()) {
    emit message(TR(T_ERROR_NO_MEDIA))   ;
    return;
  }

  if (!QFile::exists(path)) {
    emit warning(TR(T_ERROR_BAD_FILEPATH) + ": " + path);
    return;
  }

  bool ok = player_->openSubtitle(path);
  if (!ok)
    emit warning(TR(T_ERROR_BAD_SUBTITLE) + ": " + path);
}

void
MainWindow::nextFrame()
{
  if (hub_->isMediaTokenMode() && player_->hasMedia())
    player_->nextFrame();
}

void
MainWindow::showMinimizedAndPause()
{
  showMinimized();
  pause();
}

void
MainWindow::actualSize()
{
  if (hub_->isMediaTokenMode() && player_->hasMedia()) {
    hub_->setFullScreenWindowMode(false);
    if (dataManager_->token().type() == Token::TT_Audio) {
      hub_->setNormalPlayerMode();
      resize(INIT_WINDOW_SIZE);
    } else {
      QSize screen = QApplication::desktop()->availableGeometry(this).size();
      hub_->setEmbeddedPlayerMode();
      QSize sz = player_->videoDimension();
      if (sz.isEmpty() || sz.width() < PREFERRED_MIN_WIDTH || sz.height() < PREFERRED_MIN_HEIGHT ||
          sz.width() > screen.width() - 100 || sz.height() > screen.height() - 50)
        sz = PREFERRED_WINDOW_SIZE;
      if (libraryView_->isVisible())
        sz.rwidth() += libraryView_->width();
      if (sz != size()) {
        move(pos().x() + (width() - sz.width()) / 2,
             pos().y() + (height() - sz.height()) / 2);
        resize(sz);
        QTimer::singleShot(0, embeddedPlayer_, SLOT(updateGeometry()));
      }
    }
  }
}

void
MainWindow::pause()
{
  switch (hub_->tokenMode()) {
  case SignalHub::MediaTokenMode:
    if (player_->isPlaying())
      player_->pause();
    break;

  case SignalHub::SignalTokenMode:
  case SignalHub::LiveTokenMode:
#ifdef WITH_WIN_TEXTHOOK
    TextHook::globalInstance()->setEnabled(false);
    emit messageOnce(tr("Paused"));
#endif // WITH_WIN_TEXTHOOK
    break;
  }
}

void
MainWindow::promptStop()
{
  if (hub_->isStopped())
    return;

  if (QMessageBox::Ok != QMessageBox::question(this,
        tr("Confirm"),  // title
        tr("Stop playing?"),  // emssage
        QMessageBox::Ok | QMessageBox::Cancel,  // choices
        QMessageBox::Cancel // default choice
      ))
    return;
  QTimer::singleShot(0, hub_, SLOT(stop()));
}

void
MainWindow::stop()
{
  switch (hub_->tokenMode()) {
  case SignalHub::MediaTokenMode:
    if (player_->hasMedia() && !player_->isStopped())
      player_->stop();
    break;

  case SignalHub::LiveTokenMode:
    //closeChannel();
    //hub_->setMediaTokenMode();
    break;

  case SignalHub::SignalTokenMode:
#ifdef WITH_WIN_TEXTHOOK
    messageHandler_->setActive(false);
    emit message(tr("detaching all processes ..."));
    TextHook::globalInstance()->detachAllProcesses();
    TextHook::globalInstance()->setEnabled(true);
    emit messageOnce(tr("Stopped"));
#endif // WITH_WIN_TEXTHOOK
    hub_->setNormalPlayerMode();
    break;
  }

  if (!disposed_)
    libraryView_->show();
}

bool
MainWindow::isAutoPlayNext() const
{ return toggleAutoPlayNextAct_->isChecked(); }

bool
MainWindow::repeatCurrent() const
{ return toggleRepeatCurrentAct_->isChecked(); }

bool
MainWindow::noRepeat() const
{ return toggleNoRepeatAct_->isChecked(); }

void
MainWindow::setAutoPlayNext()
{
  toggleAutoPlayNextAct_->setChecked(true);
  toggleRepeatCurrentAct_->setChecked(false);
  toggleNoRepeatAct_->setChecked(false);
}

void
MainWindow::setRepeatCurrent()
{
  toggleAutoPlayNextAct_->setChecked(false);
  toggleRepeatCurrentAct_->setChecked(true);
  toggleNoRepeatAct_->setChecked(false);
}

void
MainWindow::setNoRepeat()
{
  toggleAutoPlayNextAct_->setChecked(false);
  toggleRepeatCurrentAct_->setChecked(false);
  toggleNoRepeatAct_->setChecked(true);
}

void
MainWindow::play()
{
  switch (hub_->tokenMode()) {
  case SignalHub::MediaTokenMode:
    if (player_->hasMedia())
      player_->play();
    else
      open();
    break;

  case SignalHub::LiveTokenMode:
    //if (!annotationView_->isStarted() || !liveTimer_->isActive())
    //  openChannel();
    //break;

  case SignalHub::SignalTokenMode:
#ifdef AC_ENABLE_GAME
    messageHandler_->setActive(true);
#endif // AC_ENABLE_GAME
#ifdef WITH_WIN_TEXTHOOK
    TextHook::globalInstance()->setEnabled(true);
    emit messageOnce(tr("Synchronized"));
#endif // WITH_WIN_TEXTHOOK
    break;
  }
}

void
MainWindow::playPauseMedia()
{
  if (hub_->isMediaTokenMode() && player_->hasMedia())
    player_->playPause();
}

void
MainWindow::playPause()
{
  switch (hub_->tokenMode()) {
  case SignalHub::MediaTokenMode:
    if (player_->hasMedia())
      player_->playPause();
    else
      open();
    break;

  case SignalHub::LiveTokenMode:
  case SignalHub::SignalTokenMode:
    if (hub_->isPlaying())
      hub_->pause();
    else
      hub_->play();
    break;
  }
}

void
MainWindow::replay()
{
  if (!hub_->isMediaTokenMode()) {
    play();
    return;
  }

  if (player_->hasMedia()) {
    player_->setTime(0);
    emit seeked();

    // A quick fix to solve delay on position update
    // TO BE REMOVED!
    if (hasVisiblePlayer())
      visiblePlayer()->updatePositionSlider();

    if (!player_->isPlaying())
      player_->play();
  }
}

void
MainWindow::snapshot(bool mediaOnly)
{
  switch (hub_->tokenMode()) {
  case SignalHub::MediaTokenMode:
    if (mediaOnly && player_->hasMedia() && !player_->isStopped()) {
      QString mediaName = QFileInfo(player_->mediaPath()).fileName();
      QTime time = qxTimeFromMsec(player_->time());

      QString saveName = mediaName + "__" + time.toString("hh_mm_ss_zzz") + ".png";
      QString savePath = grabber_->savePath() + "/" + saveName;

      bool succeed = player_->snapshot(savePath);
      if (succeed) {
        QClipboard *c = QApplication::clipboard();
        if (c) {
          QImage image(savePath);
          if (!image.isNull())
            c->setImage(image);
        }
        emit message(tr("saved") + ": " + savePath);
      } else
        emit warning(tr("failed to save snapshot") + ": " + savePath);
    } else {
      switch (hub_->tokenMode()) {
      case SignalHub::LiveTokenMode:
        grabber_->setBaseName(tr("Unknown"));
        break;
      case SignalHub::MediaTokenMode:
        if (player_->hasMedia())
          grabber_->setBaseName(player_->mediaTitle());
        else
          grabber_->setBaseName(tr("Unknown"));
        break;
      default: ;
      }
      grabber_->grabWindow(annotationView_->winId());
    }
    break;

  case SignalHub::LiveTokenMode:
  case SignalHub::SignalTokenMode:
    if (annotationView_->trackedWindow())
      grabber_->grabWindow(annotationView_->trackedWindow());
    else
      grabber_->grabDesktop();
    emit message(tr("snapshot saved on the destop"));
    break;
  }
}

void
MainWindow::seek(qint64 time)
{
  DOUT("enter: time =" << time);

  if (!hub_->isMediaTokenMode())
    return;

  if (!player_->hasMedia() || !player_->seekable())
    return;

  if (time <= 0) {
    //pause();
    //player_->setTime(0);
    replay();
    DOUT("exit: time <=0, time =" << time);
    return;
  }

  qint64 length = player_->mediaLength();
  if (time >= length) {
    //pause();
    //player_->setTime(length - 1);
    stop();
    DOUT("exit: time >= length, length =" << length << ", time =" << time);
    return;
  }

  player_->setTime(time);
  emit seeked();

  logger_->logSeeked(time);
  DOUT("exit: time =" << time);
}

void
MainWindow::forward(qint64 delta)
{
  if (!player_->hasMedia() || !player_->seekable())
    return;

  if (delta < 0) {
    backward(- delta);
    return;
  }

  if (!delta)
    delta = G_FORWARD_INTERVAL;

  return seek(player_->time() + delta);
}

void
MainWindow::backward(qint64 delta)
{
  if (!player_->hasMedia() || !player_->seekable())
    return;

  if (delta < 0) {
    forward(- delta);
    return;
  }

  if (!delta)
    delta = G_BACKWARD_INTERVAL;

  return seek(player_->time() - delta);
}

void
MainWindow::volumeUp(qreal delta)
{
  if (!hub_->isMediaTokenMode()) {
    // TODO: change system volume
    return;
  }

  if (delta == 0)
    delta = G_VOLUME_DELTA;

  qreal v_old = hub_->volume();
  qreal v_new = delta + v_old;
  if (v_new <= 0) {
    if (v_old > 0)
      hub_->setVolume(0);
  } else if (v_new >= 1) {
    if (v_old < 1)
      hub_->setVolume(1);
  } else
    hub_->setVolume(v_new);
}

void
MainWindow::volumeDown(qreal delta)
{
  if (!delta)
    delta = G_VOLUME_DELTA;

  volumeUp(- delta);
}

bool
MainWindow::isPlaying() const
{
  return hub_->isMediaTokenMode() ? player_->isPlaying() :
         hub_->isPlaying();
}

 // - Play mode -

/*
void
MainWindow::setPlayMode(PlayMode mode)
{
  if (mode_ == mode)
    return;
  mode_ = mode;

  switch (mode_) {
  case FullScreenPlayMode:
#ifdef WITH_WIN_DWM
    setAeroStyleEnabled(false);
#endif // WITH_WIN_DWM
    mainPlayer_->hide();
    showFullScreen();
    embeddedPlayer_->setVisible(!miniPlayer_->isVisible());
    break;

  case NormalPlayMode:
    embeddedPlayer_->hide();
    showNormal();
    mainPlayer_->setVisible(!miniPlayer_->isVisible());
    annotationView_->updateSize(); // Otherwise the height doesn't look right
#ifdef WITH_WIN_DWM
    setAeroStyleEnabled(true);
#endif // WITH_WIN_DWM
    break;
  }
}

*/

// - Mini mode -

bool
MainWindow::hasVisiblePlayer() const
{
  return mainPlayer_->isVisible()
      || embeddedPlayer_->isVisible()
      || miniPlayer_->isVisible();
}

PlayerUi*
MainWindow::visiblePlayer()
{
  return mainPlayer_->isVisible()? static_cast<PlayerUi*>(mainPlayer_)
       : embeddedPlayer_->isVisible() ? static_cast<PlayerUi*>(embeddedPlayer_)
       : miniPlayer_->isVisible()? static_cast<PlayerUi*>(miniPlayer_)
       : 0;
}

//void
//MainWindow::leaveMiniMode()
//{ setMiniMode(false); }

/*
void
MainWindow::setMiniMode(bool visible)
{
  //if (miniPlayer_->isVisible() == visible)
  //  return;

  switch (mode_) {
  case NormalPlayMode:
    mainPlayer_->setVisible(!visible);
    miniPlayer_->setVisible(visible);
    break;
  case FullScreenPlayMode:
    embeddedPlayer_->setVisible(!visible);
    miniPlayer_->setVisible(visible);
    break;
  }
}
*/

// - Help -

void
MainWindow::about()
{
  static QWidget *w = nullptr;
  if (!w) {
    w = new AcAbout(G_APPLICATION, G_VERSION, this);
    windows_.append(w);
  }
  w->show();
  w->raise();
}

void
MainWindow::help()
{
  static HelpDialog *w = nullptr;
  if (!w) {
    w = new HelpDialog(this);
    windows_.append(w);
  }
  w->show();
  w->raise();
}

void
MainWindow::update()
{
  bool updated = server_->isSoftwareUpdated();
  if (updated)
    emit notification(tr("you are using the lastest version"));
  else {
    emit notification(tr("new version released at Google Code"));
#ifdef AC_ENABLE_UPDATE
    emit message(tr("updater launched, please close Annot Player"));
    updaterDelegate_->open();
#else
    emit message(tr("opening update URL ...") + " " G_UPDATEPAGE_URL);
    QDesktopServices::openUrl(QUrl(G_UPDATEPAGE_URL));
#endif // AC_ENABLE_UPDATE
  }
}

// - Update -

QString
MainWindow::downloadSpeedToString(int speed)
{
  return speed < 1024 ? QString::number(speed) + " B/s" :
         speed < 1024 * 1024 ? QString::number(speed / 1024) + " KB/s" :
         QString::number(speed / (1024.0 * 1024), 'f', 2) + " MB/s";
}

void
MainWindow::updateDownloadProgress(qint64 receivedBytes, qint64 totalBytes)
{
  enum { FinishTimeout = 5000 }; // 5 seconds
  static qint64 finishTime = 0;

#define FORMAT_PERCENTAGE(_real)  QString::number((_real)*100, 'f', 2) + "%"
  enum { update_interval = 2500 }; // 2.5 seconds
  static qint64 updateTime, receivedSize;

  qint64 currentTime  = QDateTime::currentMSecsSinceEpoch();
  if (currentTime - updateTime < update_interval &&
      receivedBytes != totalBytes)
    return;

  int speed = 0;
  qreal percentage = 0;
  qint64 remainingTime = 0;
  if (totalBytes > 0 && receivedBytes > receivedSize) {
    speed = (receivedBytes - receivedSize) * 1000 / (currentTime - updateTime);
    percentage = (qreal)receivedBytes / totalBytes;
    if (speed > 0)
      remainingTime = (totalBytes - receivedBytes) * 1000 / speed; // in msecs
  }

  receivedSize = receivedBytes;
  updateTime = currentTime;

  bool finished = false;

  QString suffix;
  if (totalBytes > 0) {
    bool stopped = receivedBytes == totalBytes;
    suffix += " (";
    QString message;
    if (stopped) {
      message += tr("Complete");
      finished = true;
    } else {
      if (percentage < 0.01 / 100)
        message += tr("Buffering ...");
      else {
        double progress = player_->availablePosition();
        if (progress > percentage ||
            qFuzzyCompare(1 + progress, 1 + percentage))
          message += FORMAT_PERCENTAGE(percentage);
        else {
          message += QString("%1 / %2")
            .arg(FORMAT_PERCENTAGE(progress))
            .arg(FORMAT_PERCENTAGE(percentage));
        }
      }
      if (speed)
        message += ", " + downloadSpeedToString(speed);
      if (remainingTime) {
        QString ts = qxTimeFromMsec(remainingTime).toString();
        if (!ts.isEmpty())
          message += ", " + ts;
      }
    }
    suffix += message + ")";
    emit progressMessageChanged(message);
  }
  emit windowTitleSuffixToChange(suffix);
  emit downloadProgressUpdated();
  if (finished) {
    emit downloadFinished();
    if (!hub_->isFullScreenWindowMode()) {
      qint64 now = QDateTime::currentMSecsSinceEpoch();
      if (now > finishTime + FinishTimeout)
        emit trayMessage(tr("Download finished"), tr("Download finished"));
      finishTime = now;
    }
  }
#undef FORMAT_PERCENTAGE
}

QString
MainWindow::windowTitleBase() const
{
  QString ret;
  switch (hub_->tokenMode()) {
  case SignalHub::SignalTokenMode:
#ifdef AC_ENABLE_GAME
    ret = messageHandler_->processInfo().processName;
#endif // AC_ENABLE_GAME
    break;

  case SignalHub::LiveTokenMode:
    ret = TR(T_TITLE_LIVE);
    break;

  case SignalHub::MediaTokenMode:
    if (player_->hasMedia()) {
      ret = player_->mediaTitle();
      if (ret.isEmpty()) {
        ret = player_->mediaPath();
        if (!ret.isEmpty())
          ret = QFileInfo(ret).fileName();
      }
      if (!ret.isEmpty())
        ret.append(QString(" [%1x%2]")
          .arg(QString::number(width()))
          .arg(QString::number(height()))
        );
    }
    break;
  }

  if (ret.isEmpty())
    ret = TR(T_TITLE_PROGRAM);
  return ret;
}

void
MainWindow::updateWindowTitle()
{
  setWindowTitle(windowTitleSuffix_.isEmpty() ? windowTitleBase() :
                 windowTitleBase() + windowTitleSuffix_);
}

void
MainWindow::syncInputLineText(const QString &text)
{
  if (!mainPlayer_->inputComboBox()->hasFocus())
    mainPlayer_->inputComboBox()->lineEdit()->setText(text);
  if (!miniPlayer_->inputComboBox()->hasFocus())
    miniPlayer_->inputComboBox()->lineEdit()->setText(text);
  if (!embeddedPlayer_->inputComboBox()->hasFocus())
    embeddedPlayer_->inputComboBox()->lineEdit()->setText(text);
}

void
MainWindow::syncPrefixLineText(const QString &text)
{
  if (!mainPlayer_->prefixComboBox()->hasFocus())
    mainPlayer_->prefixComboBox()->lineEdit()->setText(text);
  if (!miniPlayer_->prefixComboBox()->hasFocus())
    miniPlayer_->prefixComboBox()->lineEdit()->setText(text);
  if (!embeddedPlayer_->prefixComboBox()->hasFocus())
    embeddedPlayer_->prefixComboBox()->lineEdit()->setText(text);
}

// - Dialogs -

void
MainWindow::showAnnotationCount()
{
  static AnnotationCountDialog *w = nullptr;
  if (!w) {
    w = new AnnotationCountDialog(dataManager_, this);
    connect(w, SIGNAL(countChanged(int)), annotationFilter_, SLOT(setAnnotationCountHint(int)));
    w->setCount(annotationFilter_->annotationCountHint());
    windows_.append(w);

    AC_CONNECT_MESSAGE(w, this, Qt::AutoConnection);
  }
  w->show();
  w->raise();
}

//void
//MainWindow::setNetworkProxyDialogVisible(bool visible)
//{
//  if (!networkProxyDialog_) {
//    networkProxyDialog_ = new NetworkProxyDialog(this);
//    windows_.append(networkProxyDialog_);
//  }
//  networkProxyDialog_->setVisible(visible);
//  if (visible)
//    networkProxyDialog_->raise();
//}

void
MainWindow::showLogin()
{
  static LoginDialog *w = nullptr;
  if (!w) {
    w = new LoginDialog(this);
    windows_.append(w);
    AC_CONNECT_WARNING(w, this, Qt::AutoConnection);
    connect(w, SIGNAL(loginRequested(QString, QString)), SLOT(login(QString, QString)));
  }
  w->setUserName(server_->user().name());
  //if (server_->user().name() == User::guest().name())
  //  loginDialog_->setPassword(User::guest().password());
  //else
  w->clearPassword();
  w->show();
  w->raise();
}

void
MainWindow::showUser()
{
  static UserView *w = nullptr;
  if (!w) {
    w = new UserView(this);
    w->setUser(server_->user());
    windows_.append(w);

    connect(this, SIGNAL(userChanged(User)), w, SLOT(setUser(User)));
  }
  //if (visible) {
  //  loginDialog_->setUserName(server_->user().name());
  //  loginDialog_->setPassword(server_->user().password());
  //  loginDialog_->show();
  //  loginDialog_->raise();
  //} else
  //  loginDialog_->hide();
  w->show();
  w->raise();
}

void
MainWindow::showAnnotationSource()
{
  static TokenView *w = nullptr;
  if (!w) {
    w = new TokenView(dataManager_, server_, hub_, this);
    windows_.append(w);

    AC_CONNECT_MESSAGE(w, this, Qt::AutoConnection);
    AC_CONNECT_WARNING(w, this, Qt::AutoConnection);

    connect(w, SIGNAL(aliasSubmitted(Alias)), SLOT(submitAlias(Alias)));
    connect(w, SIGNAL(tokenBlessedWithId(qint64)), SLOT(blessTokenWithId(qint64)));
    connect(w, SIGNAL(tokenCursedWithId(qint64)), SLOT(curseTokenWithId(qint64)));
    connect(w, SIGNAL(updateAnnotationsRequested()), SLOT(updateAnnotations()));

    connect(w, SIGNAL(openUrlRequested(QString)), browserDelegate_, SLOT(openUrl(QString)));
    connect(w, SIGNAL(searchRequested(int,QString)), SLOT(searchWithEngine(int,QString)));
  }

  w->show();
  w->raise();
}

void
MainWindow::showAnnotationBrowser()
{
  if (!annotationBrowser_) {
    annotationBrowser_ = new AnnotationListView(hub_, this);
    windows_.append(annotationBrowser_);

    AC_CONNECT_MESSAGES(annotationBrowser_, this, Qt::AutoConnection);

    connect(player_, SIGNAL(mediaChanged()), annotationBrowser_, SLOT(clear()));
    connect(annotationBrowser_, SIGNAL(annotationAnalyticsRequested()), SLOT(showAnnotationAnalytics()));
    connect(annotationBrowser_, SIGNAL(userAnalyticsRequested(qint64)), SLOT(showUserAnalytics(qint64)));

    connect(annotationBrowser_, SIGNAL(userBlessedWithId(qint64)), SLOT(blessUserWithId(qint64)));
    connect(annotationBrowser_, SIGNAL(userCursedWithId(qint64)), SLOT(curseUserWithId(qint64)));
    connect(annotationBrowser_, SIGNAL(userBlockedWithId(qint64)), SLOT(blockUserWithId(qint64)));
    connect(annotationBrowser_, SIGNAL(userBlockedWithAlias(QString)), annotationFilter_, SLOT(addBlockedUserAlias(QString)));

    connect(annotationBrowser_, SIGNAL(annotationBlessedWithId(qint64)), SLOT(blessAnnotationWithId(qint64)));
    connect(annotationBrowser_, SIGNAL(annotationCursedWithId(qint64)), SLOT(curseAnnotationWithId(qint64)));
    connect(annotationBrowser_, SIGNAL(annotationBlockedWithId(qint64)), SLOT(blockAnnotationWithId(qint64)));
    connect(annotationBrowser_, SIGNAL(annotationBlockedWithText(QString)), annotationFilter_, SLOT(addBlockedText(QString)));
    connect(dataManager_, SIGNAL(annotationsChanged(AnnotationList)), annotationBrowser_, SLOT(setAnnotations(AnnotationList)));
    connect(dataManager_, SIGNAL(annotationAdded(Annotation)), annotationBrowser_, SLOT(addAnnotation(Annotation)));
    connect(annotationBrowser_, SIGNAL(annotationDeletedWithId(qint64)), dataManager_, SLOT(removeAnnotationWithId(qint64)));
    connect(annotationBrowser_, SIGNAL(annotationDeletedWithId(qint64)), annotationView_, SLOT(removeAnnotationWithId(qint64)));
    connect(annotationView_, SIGNAL(annotationDeletedWithId(qint64)), annotationBrowser_, SLOT(removeAnnotationWithId(qint64)));
    connect(annotationBrowser_, SIGNAL(annotationTextUpdatedWithId(QString,qint64)), annotationView_, SLOT(updateAnnotationTextWithId(QString,qint64)));
    connect(annotationView_, SIGNAL(annotationTextUpdatedWithId(QString,qint64)), annotationBrowser_, SLOT(updateAnnotationTextWithId(QString,qint64)));
    connect(annotationBrowser_, SIGNAL(annotationTextUpdatedWithId(QString,qint64)), dataManager_, SLOT(updateAnnotationTextWithId(QString,qint64)));
    connect(annotationView_, SIGNAL(annotationPosChanged(qint64)), annotationBrowser_, SLOT(setAnnotationPos(qint64)));
    connect(this, SIGNAL(userIdChanged(qint64)), annotationBrowser_, SLOT(setUserId(qint64)));
#ifdef AC_ENABLE_GAME
    connect(messageHandler_, SIGNAL(contextChanged(qint64,qint64)), annotationBrowser_, SLOT(setAnnotationPos(qint64)));
#endif // AC_ENABLE_GAME

    annotationBrowser_->setUserId(server_->user().id());
    if (dataManager_->hasAnnotations())
      annotationBrowser_->setAnnotations(dataManager_->annotations());
  }

  annotationBrowser_->show();
  annotationBrowser_->raise();
}

void
MainWindow::showBlacklist()
{
  static BlacklistView *w = 0;
  if (!w) {
    w = new BlacklistView(annotationFilter_, this);
    windows_.append(w);
  }
  w->show();
  w->raise();
}

BacklogDialog*
MainWindow::backlogDialog()
{
  static BacklogDialog *w = nullptr;
  if (!w) {
    w = new BacklogDialog(this);
    windows_.append(w);
    connect(w, SIGNAL(translateRequested(QString)), SLOT(translateUsingTranslator(QString)));
    connect(hub_, SIGNAL(tokenModeChanged(SignalHub::TokenMode)), w, SLOT(clear()));
    connect(player_, SIGNAL(mediaChanged()), w, SLOT(clear()));
    connect(annotationView_, SIGNAL(subtitleAdded(QString)), w, SLOT(appendSubtitle(QString)));
    connect(annotationView_, SIGNAL(annotationAdded(QString)), w, SLOT(appendAnnotation(QString)));
#ifdef AC_ENABLE_GAME
    connect(syncView_, SIGNAL(threadsSelected(TextThreadList,ProcessInfo)), w, SLOT(clear()));
    connect(messageHandler_, SIGNAL(textChanged(QString,int)), w, SLOT(appendText(QString)));
#endif // AC_ENABLE_GAME
  }
  return w;
}

void
MainWindow::showBacklog()
{
  backlogDialog()->show();
  backlogDialog()->raise();
}

/*
DownloadDialog*
MainWindow::downloadDialog()
{
  if (!downloadDialog_) {
    QWidget *parent = nullptr; // orphan window
#ifndef Q_OS_MAC
    parent = this;
#endif // !Q_OS_MAC
    downloadDialog_ = new DownloadDialog(parent);
    downloadDialog_->resize(400, 300);
    connect(downloadDialog_, SIGNAL(downloadFinished(QString,QString)), SLOT(signFileWithUrl(QString,QString)));
    connect(downloadDialog_, SIGNAL(openFileRequested(QString)), SLOT(openMrl(QString)));
  }
  return downloadDialog_;
}
*/

void
MainWindow::showDownloader()
{ downloaderDelegate_->open(); }

//bool
//MainWindow::isDownloaderVisible() const
//{ return AcDownloaderController::globalController()->isRunning(); }

ConsoleDialog*
MainWindow::consoleDialog()
{
  if (!consoleDialog_) {
    Application::globalInstance()->installMessageHandlers();
    consoleDialog_ = new ConsoleDialog(this);
    windows_.append(consoleDialog_);
    Application::globalInstance()->addMessageHandler(ConsoleDialog::messageHandler);
  }
  return consoleDialog_;
}

void
MainWindow::showConsole()
{
  consoleDialog()->show();
  consoleDialog()->raise();
}

void
MainWindow::showAnnotationEditor()
{
  static AnnotationEditor *w = nullptr;
  if (!w) {
    w = new AnnotationEditor(this);
    windows_.append(w);
    connect(w, SIGNAL(textSaved(QString)), SLOT(eval(QString)));
  }
  w->show();
  w->raise();
}

//void
//MainWindow::invalidateSiteAccounts()
//{
//  if (!siteAccountView_)
//    return;
//  emit message(tr("site accounts updated"));
//  MrlResolverSettings *settings = MrlResolverSettings::globalSettings();
//  settings->setNicovideoAccount(
//    siteAccountView_->nicovideoAccount().username,
//    siteAccountView_->nicovideoAccount().password);
//  settings->setBilibiliAccount(
//    siteAccountView_->bilibiliAccount().username,
//    siteAccountView_->bilibiliAccount().password);
//}

//void
//MainWindow::setSiteAccountViewVisible(bool visible)
//{
//  if (!siteAccountView_) {
//    siteAccountView_ = new SiteAccountView(this);
//    windows_.append(siteAccountView_);
//    connect(siteAccountView_, SIGNAL(accountChanged()), SLOT(invalidateSiteAccounts()));
//
//    QString username, password;
//    boost::tie(username, password) = AcSettings::globalSettings()->nicovideoAccount();
//    if (!username.isEmpty() && !password.isEmpty())
//      siteAccountView_->setNicovideoAccount(username, password);
//
//    boost::tie(username, password) = AcSettings::globalSettings()->bilibiliAccount();
//    if (!username.isEmpty() && !password.isEmpty())
//      siteAccountView_->setBilibiliAccount(username, password);
//  }
//  siteAccountView_->setVisible(visible);
//  if (visible)
//    siteAccountView_->raise();
//}

void
MainWindow::showMediaInfo()
{
  static QWidget *w = nullptr;
  if (!w) {
    w = new MediaInfoView(player_, dataManager_, hub_, this);
    windows_.append(w);
  }
  w->show();
  w->raise();
}

void
MainWindow::openHomePage()
{
  QString url = G_HOMEPAGE;
  if (AcSettings::globalSettings()->fackCcp())
    browserDelegate_->openUrl(url);
  else
    QDesktopServices::openUrl(url);
}

void
MainWindow::showSeek()
{
  static TimeInputDialog *w = nullptr;
  if (!w) {
    w = new SeekDialog(this);
    windows_.append(w);
    connect(w, SIGNAL(timeChanged(qint64)), SLOT(seek(qint64)));
    connect(w, SIGNAL(warning(QString)), SLOT(warn(QString)));
  }

  qint64 t = player_->hasMedia() ? player_->time() : 0;
  w->setTime(t);
  w->show();
  w->raise();
}

//void
//MainWindow::showCommentView()
//{ setCommentViewVisible(true); }
//
//void
//MainWindow::hideCommentView()
//{ setCommentViewVisible(false); }
//
//void
//MainWindow::setCommentViewVisible(bool visible)
//{
//  if (!commentView_) {
//    commentView_ = new CommentView(this);
//    windows_.append(commentView_);
//  }
//
//  if (visible && dataManager_->hasToken())
//    commentView_->setTokenId(dataManager_->token().id());
//  commentView_->setVisible(visible);
//  if (visible)
//    commentView_->raise();
//}

void
MainWindow::showWindowPicker()
{
  static PickDialog *w = nullptr;
  if (!w) {
    w = new PickDialog(this);
    w->setMessage(tr("Click the window to show annotations"));
    connect(w, SIGNAL(windowPicked(WId)), SIGNAL(windowPicked(WId)));
    connect(w, SIGNAL(cancelled()), SLOT(updateOsdWindowOnTop()));
    AC_CONNECT_MESSAGE(w, this, Qt::AutoConnection);
    windows_.append(w);

    // FIXME
    osdWindow_->setWindowOnTop();
  }
  w->show();
  w->raise();
}

void
MainWindow::showProcess()
{
  static PickDialog *w = nullptr;
  if (!w) {
    w = new PickDialog(this);
    windows_.append(w);
    w->setMessage(tr("Click the galgame's window to sync with"));

    AC_CONNECT_MESSAGE(w, this, Qt::AutoConnection);
    connect(w, SIGNAL(windowPicked(WId)), SIGNAL(windowPicked(WId)));
    connect(w, SIGNAL(cancelled()), SLOT(updateOsdWindowOnTop()));
#ifdef AC_ENABLE_GAME
    connect(w, SIGNAL(windowPicked(WId)), SLOT(openProcessWindow(WId)));
#endif // AC_ENABLE_GAME
    //connect(w, SIGNAL(windowPicked(WId)), osdWindow_, SLOT(setWindowOnTop()));

    // FIXME
    osdWindow_->setWindowOnTop();
  }
  w->show();
  w->raise();
}

// - Annotations -

void
MainWindow::submitAliasText(const QString &text, qint32 type, bool async, bool lock)
{
  if (!server_->isConnected() || !server_->isAuthorized()) {
    emit warning(tr("please log in to save media information") + ": " + text);
    return;
  }
  const Token &t = dataManager_->token();
  if (!t.isValid() && !t.hasDigest()) {
    emit warning(tr("information not saved for unknown media") + ": " + text);
    return;
  }
  emit message(tr("saving media information ...") + ": " + text);
  Alias a;
  a.setTokenId(t.id());
  a.setTokenDigest(t.digest());
  a.setTokenSection(t.section());

  a.setText(text);
  a.setType(type);
  a.setUserId(server_->user().id());

  if (a.isUrl())
    a.setLanguage(Alias::guessUrlLanguage(a.text(), server_->user().language()));
  else
    a.setLanguage(server_->user().language());

  a.setUpdateTime(QDateTime::currentMSecsSinceEpoch() / 1000);
  //if (tokenView_) // FIXME: use signals here
  //  tokenView_->addAlias(a);
  submitAlias(a, async, lock);
}

void
MainWindow::submitAlias(const Alias &input, bool async, bool lock)
{
  DOUT("enter: async =" << async << ", text =" << input.text());
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (dataManager_->aliasConflicts(input)) {
    emit warning(tr("similar information already exists") + ": " + input.text());
    DOUT("exit: alias conflics");
    return;
  }
  if (input.type() == Alias::AT_Url) {
    if (!isRemoteMrl(input.text())) {
      emit warning(tr("alias is not a valid URL") + ": " + input.text());
      return;
    }
    emit openAnnotationUrlRequested(input.text());
  }
  if (async) {
    emit message(tr("saving media information ..."));
    QtConcurrent::run(this, &Self::submitAlias, input, false, lock);
    DOUT("exit: returned from async branch");
    return;
  }

  if (lock) {
    DOUT("inetMutex locking");
    inetMutex_.lock();
    DOUT("inetMutex locked");
  }

  qint64 id = dataServer_->submitAlias(input);
  Q_UNUSED(id)
  DOUT("alias id =" << id);
  //if (id)
    emit message(tr("media information saved") + ": " + input.text());
  //else
  //  emit warning(tr("failed to update alias text") + ": " + a.text());

  if (id) {
    Alias a = input;
    a.setId(id);
    dataManager_->addAlias(a);
  }

  if (lock) {
    DOUT("inetMutex unlocking");
    inetMutex_.unlock();
    DOUT("inetMutex unlocked");
  }

  DOUT("exit");
}

QString
MainWindow::removeUrlPrefix(const QString &url)
{
  if (url.startsWith(PLAYER_URL_CD, Qt::CaseInsensitive))
    return url.mid(qstrlen(PLAYER_URL_CD));

  if (url.startsWith(PLAYER_URL_VCD, Qt::CaseInsensitive))
    return url.mid(qstrlen(PLAYER_URL_VCD));

  if (url.startsWith(PLAYER_URL_DVD, Qt::CaseInsensitive))
    return url.mid(qstrlen(PLAYER_URL_DVD));

  return url;
}

int
MainWindow::fileType(const QString &fileName)
{
  if (fileName.startsWith(PLAYER_URL_CD, Qt::CaseInsensitive))
    return Token::TT_Audio;

  if (isDevicePath(fileName))
    return Token::TT_Video;

#ifdef AC_ENABLE_GAME
  static const QStringList supportedProgramSuffices =
      QStringList() G_FORMAT_PROGRAM(<<);
  foreach (const QString &suffix, supportedProgramSuffices)
    if (fileName.endsWith("." + suffix, Qt::CaseInsensitive))
      return Token::TT_Game;
#endif // AC_ENABLE_GAME

  if (Player::isSupportedVideo(fileName) || Player::isSupportedImage(fileName))
    return Token::TT_Video;
  if (Player::isSupportedAudio(fileName))
    return Token::TT_Audio;
  if (Player::isSupportedPicture(fileName))
    return Token::TT_Picture;

  if (Player::isSupportedPlaylist(fileName))
    return Token::TT_Audio;

  return Token::TT_Null; // Unknown
}

void
MainWindow::updateAnnotations(bool async)
{
  if (server_->isConnected())
    updateOnlineAnnotations(async);
  else
    updateOfflineAnnotations(async);
}

void
MainWindow::updateOnlineAnnotations(bool async)
{
  DOUT("enter: async =" << async);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (!server_->isConnected()) {
    emit warning(tr("not connected to the Internet"));
    DOUT("exit: not connected");
    return;
  }
  if (!dataManager_->token().hashId()) {
    emit message(tr("unknown media"));
    DOUT("exit: missing token ID");
    return;
  }
  if (async) {
    emit message(tr("updating annotations ..."));
    annotationView_->removeAnnotations();
    QtConcurrent::run(this, &Self::updateOnlineAnnotations, false);
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  AliasList aliases = dataServer_->selectAliasesWithToken(dataManager_->token(), true); // true = ignore cache
  dataManager_->setAliases(aliases);

  AnnotationList annots = dataServer_->selectAnnotationsWithToken(dataManager_->token(), true); // true = ignore cache

  if (!annots.isEmpty()) {
    QString src = "http://annot.me";
    QString msg = QString("%1 :" HTML_SS_OPEN(color:red) "+%2" HTML_SS_CLOSE() ", %3")
      .arg(tr("annotations found"))
      .arg(QString::number(annots.size()))
      .arg(src);
    emit message(msg);
    emit setAnnotationsRequested(annots);
    //QTimer::singleShot(1000, this, SLOT(loadExternalAnnotations()));
  }

  QString url = dataManager_->token().url();
  if (MrlAnalysis::matchSite(url)) {
    if (async)
      emit openAnnotationUrlRequested(url);
    else
      openAnnotationUrl(url);
  }

  if (hub_->isMediaTokenMode()) {
    openAnnotationUrlFromAliases(dataManager_->aliases());

    if (toggleSaveAnnotationFileAct_->isChecked())
      foreach (const QString &f, externalAnnotationFiles()) {
        QString url = AnnotationCodecManager::parseAnnotatedUrl(f);
        if (url.isEmpty())
          continue;
        url = DataManager::normalizeUrl(url);
        if (dataManager_->urlConflicts(url))
          continue;
        if (server_->isConnected() && server_->isAuthorized())
          submitAliasText(url, Alias::AT_Url, false, false); // async = false, lock = false
        else
          openAnnotationFile(f);
      }
  }

  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");

  DOUT("exit");
}

void
MainWindow::updateOfflineAnnotations(bool async)
{
  DOUT("enter: async =" << async);
  if (!hub_->isMediaTokenMode()) {
    DOUT("exit: update offline annotations for galgame is not allowed");
    return;
  }
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (async) {
    if (!hasExternalAnnotationFiles()) {
      emit message(tr("no changes"));
      DOUT("exit: no external annotation files");
      return;
    }
    emit message(tr("updating annotations ..."));
    annotationView_->removeAnnotations();
    QtConcurrent::run(this, &Self::updateOfflineAnnotations, false);
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  QStringList urls;
  foreach (const QString &f, externalAnnotationFiles()) {
    QString url = AnnotationCodecManager::parseAnnotatedUrl(f);
    if (url.isEmpty()) {
      openAnnotationFile(f);
      continue;
    }
    url = DataManager::normalizeUrl(url);
    if (!urls.isEmpty() && urls.contains(url, Qt::CaseInsensitive))
      continue;
    urls.append(url);
    openAnnotationFile(f);
  }

  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");

  DOUT("exit");
}

void
MainWindow::invalidateToken(const QString &mrl)
{
  DOUT("enter: mrl =" << mrl);
  if (!recentDigest_.isEmpty() || isDigestReady(mrl)) {
    dataManager_->clearToken();
    dataManager_->removeAliases();
    annotationView_->invalidateAnnotations();

    if (recentDigest_.isEmpty())
      recentDigest_ = digestFromFile(removeUrlPrefix(mrl));
    if (!recentDigest_.isEmpty())
      setToken(mrl);

  } else if (!recentUrl_.isEmpty()) {
    dataManager_->clearToken();
    dataManager_->removeAliases();
    annotationView_->invalidateAnnotations();
    setToken();

  } else if (player_->trackCount() != 1) {
    dataManager_->clearToken();
    dataManager_->removeAliases();
    annotationView_->invalidateAnnotations();
  }

  if (hub_->isMediaTokenMode())
    QTimer::singleShot(0, this, SLOT(resumeAll()));
  DOUT("exit");
}

void
MainWindow::resumeAll()
{
  resumeAspectRatioTimer_->start(3); // 3 times
  resumeAudioTrackTimer_->start(3); // 3 times
  resumeAudioChannelTimer_->start(3); // 3 times
  resumePlayTimer_->start(3); // 3 times
  resumeSubtitleTimer_->start(3); // 3 times
}

void
MainWindow::showVideoViewIfAvailable()
{
#ifdef Q_OS_MAC
  if (tokenType_ != Token::TT_Audio)
    videoView_->showView();
#endif // Q_OS_MAC
}

void
MainWindow::signFile(const QString &path, bool async)
{
  QString url = dataManager_->token().url();
  DOUT("url =" << url);
  if (url.isEmpty())
    emit warning(tr("login is requied to to submit annotation URL"));
  else
    signFileWithUrl(path, url, async);
  QApplication::beep();
  if (!hub_->isFullScreenWindowMode())
    emit trayMessage(tr("File saved"), path);
}

void
MainWindow::signFileWithUrl(const QString &path, const QString &url, bool async)
{
  DOUT("enter: async =" << async << ", path =" << path << ", url =" << url);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (url.isEmpty() || url.size() > Traits::MAX_ALIAS_LENGTH) {
    emit warning(tr("URL is too long") + ": " + url);
    return;
  }
  if (async) {
    emit message(tr("signing media ...") + " " + path);
    QtConcurrent::run(this, &Self::signFileWithUrl, path, url, false);
    DOUT("exit: returned from async branch");
    return;
  }

  QString digest = digestFromFile(path);
  if (digest.isEmpty()) {
    emit warning(tr("failed to analyze media") + ": " + path);
    DOUT("exit: failed to compute file digest");
    return;
  }

  qint64 now = QDateTime::currentMSecsSinceEpoch() / 1000;

  Token token; {
    token.setUserId(server_->user().id());
    token.setCreateTime(now);
    token.setDigest(digest);
    token.setType(Token::TT_Video);
  }

  AliasList aliases;
  QString fileName = QFileInfo(path).fileName();
  if (!fileName.isEmpty()) {
    if (fileName.size() > Traits::MAX_ALIAS_LENGTH) {
      fileName = fileName.left(Traits::MAX_ALIAS_LENGTH);
      emit warning(TR(T_WARNING_LONG_STRING_TRUNCATED) + ": " + fileName);
    }
    Alias a;
    a.setUserId(server_->user().id());
    a.setType(Alias::AT_File);
    a.setLanguage(server_->user().language());
    a.setText(fileName);
    a.setUpdateTime(now);
    aliases.append(a);
  }

  if (!url.isEmpty()) {
    Alias a;
    a.setType(Alias::AT_Url);
    a.setUserId(server_->user().id());
    a.setLanguage(Alias::guessUrlLanguage(url, server_->user().language()));
    a.setText(url);
    a.setUpdateTime(now);
    aliases.append(a);
  }
  Q_ASSERT(!aliases.isEmpty());

  qint64 tid = 0;

  if (!aliases.isEmpty()) {
    DOUT("inetMutex locking");
    inetMutex_.lock();
    DOUT("inetMutex locked");

    tid = dataServer_->submitTokenAndAliases(token, aliases);

    if (tid && dataManager_->hasAnnotations() && cache_->isValid()) {
      if (!aliases.isEmpty()) {
        cache_->deleteAliasesWithTokenId(tid);
        cache_->insertAliases(aliases);
      }
      AnnotationList l;
      foreach (Annotation a, dataManager_->annotations()) {
        a.setTokenId(tid);
        a.setTokenDigest(QString());
        l.append(a);
      }
      cache_->deleteAnnotationsWithTokenId(tid);
      cache_->insertAnnotations(l);
    }

    DOUT("inetMutex unlocking");
    inetMutex_.unlock();
    DOUT("inetMutex unlocked");
  }

  if (tid)
    emit message(tr("media signed") + ": " + path);
  else
    emit warning(tr("failed to sign media") + ": " + path);

  addRecent(path);

  DOUT("exit");
}

void
MainWindow::setToken(const QString &input, bool async)
{
  DOUT("enter: async =" << async << ", path =" << input);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (async) {
    emit message(tr("searching for media information ..."));
    QtConcurrent::run(this, &Self::setToken, input, false);
    DOUT("exit: returned from async branch");
    return;
  }

  const qint64 now = QDateTime::currentMSecsSinceEpoch();
  Media media;
  media.setVisitCount(1);
  media.setVisitTime(now);

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  //Token token = dataManager_->token();
  dataManager_->removeToken();

  Token token;
  Alias alias;

  if (!input.isEmpty()) {
    QString path = removeUrlPrefix(input);
    media.setLocation(QDir::fromNativeSeparators(path));

    if (hub_->isMediaTokenMode() && player_->hasMedia())
      media.setTitle(player_->mediaTitle());

    token.setType(tokenType_ = fileType(input));

    Q_ASSERT(!recentDigest_.isEmpty());
    if (recentDigest_.isEmpty() && isDigestReady(path))
      recentDigest_ = digestFromFile(path);

    if (!recentDigest_.isEmpty()) {
      token.setDigest(recentDigest_);

      // If isAliasReady, make fileName as alias and guess tt.
      // Otherwise, keep token's original tt from tokenView_
      if (isAliasReady(path)) {
        QString fileName = QFileInfo(path).fileName();
        if (!fileName.isEmpty()) {
          if (fileName.size() > Traits::MAX_ALIAS_LENGTH) {
            fileName = fileName.left(Traits::MAX_ALIAS_LENGTH);
            emit warning(TR(T_WARNING_LONG_STRING_TRUNCATED) + ": " + fileName);
          }

          alias.setTokenDigest(recentDigest_);
          alias.setType(Alias::AT_File);
          alias.setUserId(server_->user().id());
          alias.setLanguage(server_->user().language());
          alias.setText(fileName);
          alias.setUpdateTime(now / 1000);
        }
      }
    }
  } else if (!recentDigest_.isEmpty()) {
    token.setDigest(recentDigest_);
  } else if (!recentUrl_.isEmpty()) {
    Q_ASSERT(hub_->isMediaTokenMode());
    token.setUrl(recentUrl_);
    media.setLocation(recentUrl_);

    token.setType(tokenType_ = Token::TT_Url);
    int section = currentPlaylistIndex();
    if (section < 0)
      section = 0;
    token.setSection(section);
    QString title = player_->mediaTitle();
    media.setTitle(title);
    if (!title.isEmpty() && !isRemoteMrl(title)) {
      if (recentUrlTitles_.size() > RECENT_COUNT)
        recentUrlTitles_.erase(recentUrlTitles_.begin());
      recentUrlTitles_[recentUrl_] = title;
      alias.setType(Alias::AT_Title);
      alias.setUserId(server_->user().id());
      alias.setLanguage(server_->user().language());
      alias.setText(title);
      alias.setUpdateTime(now / 1000);
    }
  }

  media.setDigest(token.digest());
  media.setType(token.type());

  if (//!token.hasType() ||
      !token.hasDigest() && !token.hasUrl()) {

    if (!media.hasTitle() && media.hasLocation() && !media.isUrl())
      media.setTitle(QFileInfo(media.location()).completeBaseName());

    mediaLibrary_->visit(media);

    DOUT("inetMutex unlocking");
    inetMutex_.unlock();
    DOUT("inetMutex unlocked");
    DOUT("exit from empty digest or tokenType branch");
    return;
  }

  token.setCreateTime(now / 1000);

  if (hub_->isMediaTokenMode() && player_->hasMedia() && token.hasDigest()) {
    int section = 0;
    if (player_->titleCount() > 1)
      section = player_->titleId();
    //else if (player_->hasPlaylist())
    //  section = player_->trackNumber();

    if (section < 0)
      section = 0;
    token.setSection(section);
  }

  AliasList aliases;
  if (alias.hasText())
    aliases.append(alias);

#ifdef AC_ENABLE_GAME
  if (tokenType_ == Token::TT_Game) {
    WId hwnd = annotationView_->trackedWindow();
    if (hwnd && QtWin::getWindowProcessId(hwnd) != QCoreApplication::applicationPid()) {
      QString t = QtWin::getWindowText(hwnd).trimmed();
      if (!t.isEmpty()) {
        t = TextCodecManager::globalInstance()->transcode(t);
        DOUT("galgame window title =" << t);
        media.setTitle(t);

        Alias a;
        a.setType(Alias::AT_Title);
        a.setUserId(server_->user().id());
        a.setLanguage(server_->user().language());
        a.setText(t);
        a.setUpdateTime(now / 1000);
        aliases.append(a);
      }
    }

    static QString programFiles;
    if (programFiles.isEmpty()) {
      programFiles = QtWin::getProgramFilesPath();
      if (!programFiles.isEmpty()) {
        programFiles = QDir::fromNativeSeparators(programFiles);
        if (!programFiles.endsWith('/'))
          programFiles.append('/');
      }
    }
    QString path = QDir::fromNativeSeparators(input);
    if (!programFiles.isEmpty() && path.startsWith(programFiles, Qt::CaseInsensitive)) {
      path.remove(programFiles, Qt::CaseInsensitive);
      QStringList l = path.split('/',  QString::SkipEmptyParts);
      l.removeLast();
      if (!l.isEmpty() && l.size() <= 3) {
        Alias a;
        a.setType(Alias::AT_Folder);
        a.setUserId(server_->user().id());
        a.setLanguage(server_->user().language());
        a.setText(l.size() == 3 ? l[1] : l.last());
        a.setUpdateTime(now / 1000);
        aliases.append(a);

        if (l.size() >= 2) {
          a.setType(Alias::AT_Brand);
          a.setText(l.first());
          aliases.append(a);
        }
      }
    } else {
      QStringList l = path.split('/',  QString::SkipEmptyParts);
      if (l.size() >= 3) {
        QString t = l[l.size() -2];
        Alias a;
        a.setType(Alias::AT_Folder);
        a.setUserId(server_->user().id());
        a.setLanguage(server_->user().language());
        a.setText(t);
        a.setUpdateTime(now / 1000);
        aliases.append(a);
      }
    }
  }
#endif // AC_ENABLE_GAME

  qint64 tid = 0;
  if (submit_) {
    tid = dataServer_->submitTokenAndAliases(token, aliases);
    //alias.setTokenId(tid)
    if (tid) {
      Token t = dataServer_->selectTokenWithId(tid);
      if (t.isValid()) {
        //aliases = dataServer_->selectAliasesWithTokenId(tid);
        token = t;
      } //else
        //emit warning(TR(T_ERROR_SUBMIT_TOKEN) + ": " + input);
    }
  } else if (token.hasDigest()) {
    Token t = dataServer_->selectTokenWithDigest(token.digest(), token.section());
    if (t.isValid() || t.hasDigest())
      token = t;
  }

  if (!token.isValid() && cache_->isValid() && token.hasDigest()) {
    emit message(tr("searching for media information in cache ..."));
    Token t = cache_->selectTokenWithDigest(token.digest(), token.section());
    if (t.isValid())
      token = t;
  }

  media.setTokenId(token.id());

  //if (!media.hasTitle() && media.hasLocation() && !media.isRemote())
  //  media.setTitle(QFileInfo(media.location()).completeBaseName());

  mediaLibrary_->visit(media);

#ifdef AC_ENABLE_GAME
  if (media.isGame()) {
    Game g;
    g.setVisitCount(1);
    g.setVisitTime(now);
    g.setTitle(media.title());
    g.setDigest(media.digest());
    g.setTokenId(media.tokenId());
    g.setLocation(media.location());
    g.setHook(TextHook::globalInstance()->currentHook());

    g.setEncoding(TextCodecManager::globalInstance()->encoding());
    g.setThreads(messageHandler_->threads());

    gameLibrary_->visit(g);

    if (!server_->isConnected()) {
      if (dataManager_->hasGameThread())
        dataManager_->removeGameThread();
      if (dataManager_->hasGameHook())
        dataManager_->removeGameHook();
    }

    if (server_->isConnected() && server_->isAuthorized() &&
        g.hasTokenId()) {
      GameHook h = g.tokenId() == dataManager_->gameHook().tokenId() ?
        dataManager_->gameHook() :
        server_->selectGameHookWithTokenId(g.tokenId());
      if (h.isValid()) {
        if (g.hasHook() && g.hook() != h.text())
          server_->updateGameHookTextWithId(g.hook(), h.id());
      } else if (g.hasHook()) {
        h.setUserId(server_->user().id());
        h.setTokenId(g.tokenId());
        h.setUpdateTime(now / 1000);
        h.setText(g.hook());
        qint64 id = server_->submitGameHook(h);
        h.setId(id);
      }

      GameThread t = g.tokenId() == dataManager_->gameThread().tokenId() ?
        dataManager_->gameThread() :
        server_->selectGameThreadWithTokenId(g.tokenId());

      TextThread tt;
      foreach (const TextThread &i, g.threads())
        if (i.isLeadingRole())  {
          tt = i;
          break;
        }
      if (!t.isValid()) {
        if (tt.hasRole()) {
          t.setUpdateTime(now / 1000);
          t.setUserId(server_->user().id());
          t.setTokenId(g.tokenId());
          t.setEncoding(Traits::codePageFromEncoding(g.encoding()));
          t.setSignature(tt.signature());
          t.setProvider(tt.provider());
          t.setType(tt.role());
          qint64 id = server_->submitGameThread(t);
          t.setId(id);
        }
      } else if (tt.hasRole() && (
                 tt.hasSignature() && tt.signature() != t.signature() ||
                 g.hasEncoding() && t.encoding() != Traits::codePageFromEncoding(g.encoding()) ||
                 tt.hasProvider() && tt.provider() != t.provider())
        ) {
        t.setUpdateTime(now / 1000);
        t.setUserId(server_->user().id());
        t.setTokenId(g.tokenId());
        t.setEncoding(Traits::codePageFromEncoding(g.encoding()));
        t.setSignature(tt.signature());
        t.setProvider(tt.provider());
        t.setType(tt.role());
        server_->updateGameThread(t);
      }

      dataManager_->setGameHook(h);
      dataManager_->setGameThread(t);
    }
  }
#endif // AC_ENABLE_GAME

  bool fromCache;
  aliases = dataServer_->selectAliasesWithToken(token, false, &fromCache); // false = ignore cache
  if (server_->isConnected() && fromCache) {
    bool hasUrl = false;
    foreach (const Alias &a, aliases)
      if (a.type() == Alias::AT_Url) {
        hasUrl = true;
        break;
      }
    if (!hasUrl)
      aliases = dataServer_->selectAliasesWithToken(token, true); // true = ignore cache
  }

  dataManager_->setToken(token);
  dataManager_->setAliases(aliases);

  DOUT("token type =" << token.type() << ", section =" << token.section() << ", url =" << token.url());
  DOUT("alias count =" << aliases.size());

//  if (commentView_)
//    commentView_->setTokenId(token.id());
#ifdef AC_ENABLE_GAME
  //syncView_->tokenView()->setAliases(aliases);
#endif // AC_ENABLE_GAME

  //bool fromCache;
  AnnotationList annots = dataServer_->selectAnnotationsWithToken(token, false, &fromCache); // false = ignore cache
  //annotationBrowser_->setAnnotations(annots);
  //annotationView_->setAnnotations(annots);

  if (hub_->isMediaTokenMode()) {
    QHash<QString, QString> urlCache;
    if (toggleSaveAnnotationFileAct_->isChecked())
      foreach (const QString &f, externalAnnotationFiles()) {
        QString url = AnnotationCodecManager::parseAnnotatedUrl(f);
        if (url.isEmpty())
          continue;
        url = DataManager::normalizeUrl(url);
        if (dataManager_->urlConflicts(url)) {
          urlCache[url] = f;
          continue;
        }
        if (server_->isConnected() && server_->isAuthorized())
          submitAliasText(url, Alias::AT_Url, false, false); // async = false, lock = false
        else
          openAnnotationFile(f);
      }

    if (!aliases.isEmpty() &&
        (annots.isEmpty() || dataServer_->preferCache() && !fromCache))
      openAnnotationUrlFromAliases(aliases, urlCache);
  }

  //if (tokens.size() == 1) {
  //  Token token = tokens.first();
  //  if (!tokenView_->hasToken()) {
  //    tokenView_->setToken(token);
  //    server_->queryClusteredTokensyTokenId(token.id());
  //    return;
  //  }
  //}
  //tokenView_->setClusteredTokens(tokens);
  //foreach (const Token &t, tokens)
  //  server_->queryAnnotationsByTokenId(t.id());

  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");

  if (!annots.isEmpty()) {
    QString src = dataServer_->preferCache() ? tr("offline database") : "http://annot.me";
    QString msg = QString("%1 :" HTML_SS_OPEN(color:red) "+%2" HTML_SS_CLOSE() ", %3")
      .arg(tr("annotations found"))
      .arg(QString::number(annots.size()))
      .arg(src);
    emit message(msg);
    emit setAnnotationsRequested(annots);
  }

  DOUT("exit");
}

void
MainWindow::showText(const QString &text, bool isSigned)
{
  DOUT("enter: text =" << text);
  Annotation annot;
  if (isSigned && server_->isAuthorized()) {
    annot.setUserId(server_->user().id());
    annot.setUserAlias(server_->user().name());
    annot.setUserAnonymous(server_->user().isAnonymous());
  }
  if (server_->isAuthorized())
    annot.setLanguage(server_->user().language());
  annot.setText(text);
  annotationView_->showAnnotation(annot);
  DOUT("exit");
}

void
MainWindow::showFresheyeTranslation(const QString &text, bool extra)
{
  //enum { U_3000 = 12288 }; // \u3000
  int lang = server_->user().language();
  if (extra)
    showAdditionalTranslation(
      Traits::isAsianLanguage(lang) ? QString(text).remove(QChar(ushort(0x3000))) : text,
      TranslatorManager::Fresheye
    );
  else
    showTranslation(
      Traits::isAsianLanguage(lang) ? QString(text).remove(QChar(ushort(0x3000))) : text,
      TranslatorManager::Fresheye
    );
}

void
MainWindow::showOcnTranslation(const QString &text, bool extra)
{
  int lang = server_->user().language();
  QString t = text;

  if (Traits::isAsianLanguage(lang))
    t.remove("\r\n");

  if (t.contains('['))
    t.replace('[', "\\[")
     .replace(']', "\\]");

  if (extra)
    showAdditionalTranslation(t, TranslatorManager::Ocn);
  else
    showTranslation(t, TranslatorManager::Ocn);
}

void
MainWindow::showAdditionalTranslation(const QString &text, int service)
{
  if (text.isEmpty())
    return;
  showTranslation(CORE_CMD_VIEW_FLOAT " " + text, service);
}

void
MainWindow::showTranslation(const QString &text, int service)
{
  if (text.isEmpty())
    return;
  int userLang = service == TranslatorManager::Romaji ? Traits::English :
                 server_->user().language();
  QString prefix;
  if (Traits::isRomanLanguage(userLang))
    prefix = CORE_CMD_LATEX_SMALL CORE_CMD_HTML_EM;
  switch (service) {
  case TranslatorManager::Romaji:
    if (prefix.isEmpty())
      prefix = CORE_CMD_LATEX_SMALL CORE_CMD_HTML_EM " ";
    if (translator_->serviceCount() > 2)
      prefix.append("Romaji: ");
    break;
  case TranslatorManager::Ocn:
    //if (translator_->serviceCount() > 1)
    //  prefix = CORE_CMD_COLOR_YELLOW " ";
    if (translator_->serviceCount() > 1 && !translator_->hasService(TranslatorManager::Romaji))
      prefix.append(CORE_CMD_COLOR_YELLOW " OCN: ");
    break;
  case TranslatorManager::Fresheye:
    prefix.append(translator_->serviceCount() > 2 || translator_->serviceCount() == 2 && !translator_->hasService(TranslatorManager::Romaji) ?
      CORE_CMD_COLOR_BLUE " freshEYE: " :
      CORE_CMD_COLOR_YELLOW " "
    ); break;
  case TranslatorManager::Infoseek:
    prefix.append(translator_->serviceCount() > 2 || translator_->serviceCount() == 2 && !translator_->hasService(TranslatorManager::Romaji) ?
      CORE_CMD_COLOR_RED " Infoseek: "  :
      CORE_CMD_COLOR_YELLOW " "
    ); break;
  case TranslatorManager::Yahoo:
    prefix.append(translator_->serviceCount() > 2 || translator_->serviceCount() == 2 && !translator_->hasService(TranslatorManager::Romaji) ?
      CORE_CMD_COLOR_BLACK " Yahoo!: ":
      CORE_CMD_COLOR_YELLOW " "
    ); break;
  case TranslatorManager::Microsoft:
    prefix.append(translator_->serviceCount() > 2 || translator_->serviceCount() == 2 && !translator_->hasService(TranslatorManager::Romaji) ?
      CORE_CMD_COLOR_MAGENTA " Microsoft: "  :
      CORE_CMD_COLOR_YELLOW " "
    ); break;
  case TranslatorManager::Google:
    prefix.append(translator_->serviceCount() > 2 || translator_->serviceCount() == 2 && !translator_->hasService(TranslatorManager::Romaji) ?
      CORE_CMD_COLOR_PINK " Google: " :
      CORE_CMD_COLOR_YELLOW " "
    ); break;
  case TranslatorManager::Nifty:
    prefix.append(translator_->serviceCount() > 2 || translator_->serviceCount() == 2 && !translator_->hasService(TranslatorManager::Romaji) ?
      CORE_CMD_COLOR_ORANGE " @nifty: " :
      CORE_CMD_COLOR_YELLOW " "
    ); break;
  case TranslatorManager::Excite:
    prefix.append(translator_->serviceCount() > 2 || translator_->serviceCount() == 2 && !translator_->hasService(TranslatorManager::Romaji) ?
      CORE_CMD_COLOR_BROWN " Excite: " :
      CORE_CMD_COLOR_YELLOW " "
    ); break;
  case TranslatorManager::Sdl:
    prefix.append(translator_->serviceCount() > 2 || translator_->serviceCount() == 2 && !translator_->hasService(TranslatorManager::Romaji) ?
      CORE_CMD_COLOR_CYAN " SDL: "  :
      CORE_CMD_COLOR_YELLOW " "
    ); break;
  case TranslatorManager::Systran:
    prefix.append(translator_->serviceCount() > 2 || translator_->serviceCount() == 2 && !translator_->hasService(TranslatorManager::Romaji) ?
      CORE_CMD_COLOR_BLUE " SYSTRAN: " :
      CORE_CMD_COLOR_YELLOW " "
    ); break;
  }
  if (!prefix.isEmpty() && !prefix.endsWith(' '))
    prefix.append(' ');
  showSubtitle(text, userLang, prefix);
}

void
MainWindow::showSubtitle(const QString &input, int userLang, const QString &prefix)
{
  if (input.isEmpty())
    return;

  if (!userLang)
    userLang = server_->user().language();

  /*
  enum { CHAR_WIDTH = 12, MIN_CHAR_WIDTH = 8 };
  int char_width = Traits::isRomanLanguage(userLang) ?
        CHAR_WIDTH : CHAR_WIDTH*2; // double-width for Asian characters
  char_width = qMax<int>(char_width * annotationView_->scale(), MIN_CHAR_WIDTH);

  QString text = prefix;
  // Split long text
  int max_size = annotationView_->width() * 0.9 / char_width; // 0.9 to escape margin
  if (max_size && input.size() > max_size) {
    int count = qCeil(qreal(input.size()) / max_size);
    for (int i = 0; i < count; i++) {
      if (i) // not the first
        text.append(CORE_CMD_NEWLINE " ");
      int start = i * max_size;
      int len = qMin(max_size, input.size() - start);
      if (len > 0) {
        text.append(input.mid(start, len));
        if (i != count - 1 && // not the last
            input[start+len-1].isLetterOrNumber() &&
            input[start+len].isLetterOrNumber())
          text.append('-');
      }
    }
  } else
    text.append(input);
  */

  QString text = prefix + input;

  Annotation annot;
  //if (isSigned && server_->isAuthorized()) {
  //  annot.setUserId(server_->user().id());
  //  annot.setUserAlias(server_->user().name());
  //  annot.setUserAnonymous(server_->user().isAnonymous());
  //}
  if (userLang)
    annot.setLanguage(userLang);
  if (text.startsWith(CORE_CMDCH))
    text.prepend(CORE_CMD_SUB);
  else
    text.prepend(CORE_CMD_SUB " ");
  annot.setText(text)    ;
  annotationView_->showAnnotation(annot, false); // showMeta = false
}

/*
void
MainWindow::submitLiveText(const QString &text, bool async)
{
  DOUT("enter: async =" << async << ", text =" << text);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (!server_->isConnected() || !server_->isAuthorized()) {
    showText(text, true); // isSigned = true
    DOUT("exit: returned from showText branch");
    return;
  }

  if (async) {
    emit message(tr("saving annotation ..."));
    QtConcurrent::run(this, &Self::submitLiveText, text, false);
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  Annotation annot; {
    annot.setTokenId(Token::TI_Public);
    annot.setUserId(server_->user().id());
    annot.setUserAlias(server_->user().name());
    annot.setUserAnonymous(server_->user().isAnonymous());
    annot.setLanguage(server_->user().language());
    annot.setCreateTime(QDateTime::currentMSecsSinceEpoch() / 1000);
    annot.setPos(annot.createTime());
    if (text.size() <= Traits::MAX_ANNOT_LENGTH)
      annot.setText(text);
    else {
      annot.setText(text.left(Traits::MAX_ANNOT_LENGTH));
      emit warning(TR(T_WARNING_LONG_STRING_TRUNCATED) + ": " + annot.text());
    }
  }

  qint64 id = server_->submitLiveAnnotation(annot);

  if (id)
    annot.setId(id);
  else
    emit warning(TR(T_ERROR_SUBMIT_ANNOTATION) + ": " + text);

  //annotationBrowser_->addAnnotation(annot);
  //annotationView_->addAndShowAnnotation(annot);
  //dataManager_->token().annotCount()++;
  //dataManager_->invalidateToken();
  if (id && annot.userId() == User::UI_Guest)
    emit showAnnotationOnceRequested(annot);
  else
    emit showAnnotationRequested(annot);

  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");
  DOUT("exit");
}
*/

void
MainWindow::submitText(const QString &text, bool async)
{
  DOUT("enter: async =" << async << ", text =" << text);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (!server_->isAuthorized() ||
      !dataManager_->token().hasDigest() && !dataManager_->token().hasUrl() ||
      hub_->isMediaTokenMode() && !player_->hasMedia()
#ifdef AC_ENABLE_GAME
      || hub_->isSignalTokenMode() && !messageHandler_->currentHash()
#endif // AC_ENABLE_GAME
     ) {

    showText(text, true); // isSigned = true
    DOUT("exit: returned from showText branch");
    return;
  }

  Q_ASSERT(!hub_->isLiveTokenMode());

  if (async) {
    emit message(tr("saving annotation ..."));
    QtConcurrent::run(this, &Self::submitText, text, false);
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");
  Annotation annot; {
    annot.setTokenId(dataManager_->token().id());
    annot.setTokenDigest(dataManager_->token().digest());
    annot.setTokenSection(dataManager_->token().section());
    annot.setUserId(server_->user().id());
    annot.setUserAlias(server_->user().name());
    annot.setUserAnonymous(server_->user().isAnonymous());
    annot.setLanguage(server_->user().language());
    annot.setCreateTime(QDateTime::currentMSecsSinceEpoch() / 1000);

    if (text.size() <= Traits::MAX_ANNOT_LENGTH)
      annot.setText(text);
    else {
      annot.setText(text.left(Traits::MAX_ANNOT_LENGTH));
      emit warning(TR(T_WARNING_LONG_STRING_TRUNCATED) + ": " + annot.text());
    }

    switch (hub_->tokenMode()) {
    case SignalHub::MediaTokenMode:
      annot.setPos(
        !player_->hasMedia() ? 0 :
        tokenType_ == Token::TT_Picture ? 0 :
        player_->time()
      );
      break;
    case SignalHub::LiveTokenMode:
      Q_ASSERT(0);
      break;
    case SignalHub::SignalTokenMode:
  #ifdef AC_ENABLE_GAME
      if (annot.isSubtitle()) {
        annot.setPos(messageHandler_->currentHash());
        annot.setPosType(1);
      } else {
        annot.setPos(messageHandler_->currentContext().hash);
        annot.setPosType(messageHandler_->currentContext().count);
      }
  #endif // AC_ENABLE_GAME
      break;
    }
  }

  qint64 id = dataServer_->submitAnnotation(annot);
  if (id)
    annot.setId(id);
  //else
  //  emit warning(TR(T_ERROR_SUBMIT_ANNOTATION) + ": " + text);

  //annotationBrowser_->addAnnotation(annot);
  //annotationView_->addAndShowAnnotation(annot);
  dataManager_->token().annotCount()++;
  dataManager_->invalidateToken();
  emit addAndShowAnnotationRequested(annot);

  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");
  DOUT("exit");
}

void
MainWindow::eval(const QString &input)
{
  QString text = input.trimmed();
  if (text.isEmpty())
    return;

  QString dummy;
  QStringList tags;
  boost::tie(dummy, tags) = ANNOT_PARSE_CODE(text);

  foreach (const QString &tag, tags)
    switch (qHash(tag)) {
    case AnnotCloud::H_Chat: chat(text); return;
    case AnnotCloud::H_Play: play(); return;

    case AnnotCloud::H_Quit:
    case AnnotCloud::H_Exit:
    case AnnotCloud::H_Close:
      close();
      return;
    }

  if (hub_->isSignalTokenMode() && !hub_->isStopped() ||
      player_->hasMedia() && !player_->isStopped())
    submitText(text);
  //else if (hub_->isLiveTokenMode() && server_->isConnected() && server_->isAuthorized())
  //  submitLiveText(text);
  else
    chat(text);
}

void
MainWindow::chat(const QString &input, bool async)
{
  DOUT("enter: async =" << async << ", text =" << input);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  QString text = input.trimmed();
  if (text.isEmpty()) {
    DOUT("exit: empty text skipped");
    return;
  }

  if (async) {
    //log(tr("sending chat text ..."));
    QtConcurrent::run(this, &Self::chat, input, false);
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  //emit showTextRequested(CORE_CMD_COLOR_BLUE " " + text);
  emit showTextRequested(text);
  emit said(text);

#ifdef WITH_LIB_DOLL
  Q_ASSERT(doll_);
  doll_->chat(text);
#else
  if (server_->isConnected())
    emit response(server_->chat(text));
#endif // WITH_LIB_DOLL

  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");
  DOUT("exit");
}

void
MainWindow::respond(const QString &text)
{
  DOUT("enter: text =" << text);
  showText(CORE_CMD_COLOR_PURPLE " " + text);
  say(text, "purple");
  DOUT("exit");
}

void
MainWindow::say(const QString &text, const QString &color)
{
  DOUT("enter: color =" << color << ", text =" << text);
  if (text.isEmpty()) {
    DOUT("exit: empty text");
    return;
  }

  if (color.isEmpty())
    emit message(text);
  else
    emit message(html_ss(text, "color:" + color));
  DOUT("exit");
}

// - Events -

bool
MainWindow::event(QEvent *e)
{
  Q_ASSERT(e);
  bool accept = true;
  switch (e->type()) {
  case QEvent::FileOpen: // See: http://www.qtcentre.org/wiki/index.php?title=Opening_documents_in_the_Mac_OS_X_Finder
    {
      auto fe = static_cast<QFileOpenEvent *>(e);
      Q_ASSERT(fe);
      QString url = fe->file();
      if (url.isEmpty()) {
        url = fe->url().toString();
        if (!url.isEmpty())
          url = QUrl::fromPercentEncoding(url.toLocal8Bit());
      }
      if (!url.isEmpty())
        QTimer::singleShot(0, new detail::OpenSource(url, this), SLOT(trigger()));
    } break;
  case QEvent::Gesture: gestureEvent(static_cast<QGestureEvent *>(e)); break;
  default: accept = Base::event(e);
  }
  return accept;
}

void
MainWindow::setVisible(bool visible)
{
  DOUT("enter: visible =" << visible);
  //if (visible == isVisible())
  //  return;
  if (!isMinimized() && !isFullScreen()) { // invoked by showNormal or showMaximized
#ifdef WITH_WIN_DWM
    if (AcUi::isAeroAvailable())
      AcUi::globalInstance()->setWindowDwmEnabled(this, true);
      //QTimer::singleShot(0,
      //  new detail::SetWindowDwmEnabled(true, this), SLOT(trigger())
      //);
    else
#endif // WITH_WIN_DWM
    if (!rippleEnabled_)
      AcUi::globalInstance()->setWindowBackground(this, false); // persistent is false
  }
  if (visible) {
    if (rippleTimer_ && !rippleTimer_->isActive())
      rippleTimer_->start();
  } else {
    if (rippleTimer_ && rippleTimer_->isActive())
      rippleTimer_->stop();
  }
  if (visible && !isVisible()) {
    static qreal opacity = 0.0;
    if (qFuzzyCompare(opacity + 1, 1))
      opacity = windowOpacity();
    fadeAni_->fadeIn(opacity);
  }
  QTimer::singleShot(0, osdWindow_, SLOT(raise()));
  Base::setVisible(visible);
  DOUT("exit: visible =" << isVisible());
}

void
MainWindow::focusInEvent(QFocusEvent *e)
{
  osdWindow_->raise();
  Base::focusInEvent(e);
}

void
MainWindow::focusOutEvent(QFocusEvent *e)
{
  //osdWindow_->raise();
  Base::focusOutEvent(e);
}

void
MainWindow::mousePressEvent(QMouseEvent *event)
{
  //DOUT("enter: globalPos =" << event->globalPos());
  QPoint globalPos = event->globalPos();
  QPoint gp = mapFromGlobal(globalPos);
  osdWindow_->raise();
  pressedPos_ = globalPos;
  pressedButtons_ = event->buttons();
  //QApplication::setOverrideCursor(Qt::ArrowCursor);
  //resetAutoHideCursor();

  if (magnifier_ && magnifier_->isVisible()) {
    magnifier_->setCenter(globalPos);
    magnifier_->trigger();
  }

  if (rippleFilter_ && event->button() != Qt::RightButton) {
    rippleFilter_->setDampling(16);
    rippleFilter_->setCenter(gp);
  }

  cancelContextMenu_ = annotationView_->isNearbyItemExpelled() ||
                       annotationView_->isHoveredItemPaused() ||
                       annotationView_->isHoveredItemRemoved() ||
                       annotationView_->isHoveredItemResumed() ||
                       annotationView_->isNearbyItemAttracted();
  //DOUT("cancelContextMenu =" << cancelContextMenu_);
  if (cancelContextMenu_) {
    annotationView_->setHoveredItemPaused(false);
    annotationView_->setHoveredItemResumed(false);
    annotationView_->setHoveredItemRemoved(false);
    annotationView_->setNearbyItemExpelled(false);
    annotationView_->setNearbyItemAttracted(false);
  }

  Q_ASSERT(event);
  //if (contextMenu_->isVisible())
  //  contextMenu_->hide();
#ifdef Q_OS_MAC
  if (hub_->isEmbeddedPlayerMode() &&
      isGlobalPosNearEmbeddedPlayer(globalPos)) {
    // Prevent auto hide osd player.
    // Since NSView::mouseMoved is not avaible, use mouseDown to prevent hiding instead.
    if (!hasVisiblePlayer()) {
      embeddedPlayer_->show();
      //osdDock_->raise();
      osdWindow_->raise();
      osdWindow_->setFocus(Qt::MouseFocusReason);
    } else if (embeddedPlayer_->isVisible())
      embeddedPlayer_->resetAutoHideTimeout();
      //QTimer::singleShot(0, embeddedPlayer_, SLOT(resetAutoHideTimeout()));

    if (embeddedPlayer_->isVisible())
      embeddedCanvas_->setVisible(embeddedCanvas_->underMouse() ||
                                  !embeddedPlayer_->isOnTop() && isGlobalPosOverEmbeddedPositionSlider(globalPos));
    // Alternative to windows hook:
    //if (isPlaying())
    //  pause();
  }
#endif // Q_OS_MAC

  switch (event->buttons()) {
  case Qt::LeftButton:
    if (!event->modifiers())
      holdTimer_->start();
    {
      bool shiftPressed = (event->modifiers() & Qt::ShiftModifier)
#ifdef Q_OS_WIN
          || QtWin::isKeyShiftPressed()
#endif // Q_OS_WIN
      ;
      bool cmdPressed = (event->modifiers() & Qt::ControlModifier)
#ifdef Q_OS_WIN
          || QtWin::isKeyControlPressed()
#endif // Q_OS_WIN
      ;
      bool optPressed = (event->modifiers() & Qt::AltModifier)
#ifdef Q_OS_WIN
          || QtWin::isKeyAltPressed()
#endif // Q_OS_WIN
      ;
//      bool metaPressed = (event->modifiers() & Qt::MetaModifier)
//#ifdef Q_OS_WIN
//          || QtWin::isKeyWinPressed()
//#endif // Q_OS_WIN
//      ;

      if (cmdPressed && shiftPressed) {
        emit message(tr("remove annotations"));
        removeRubberBand_->press(gp);
      } else if (optPressed) {
        annotationView_->setNearbyItemAttracted(true);
        //annotationView_->setHoveredItemRemoved(true);
        Application::globalInstance()->setCursor(Qt::ClosedHandCursor);
        //annotationView_->attractItems(gp);
        //attractRubberBand_->setCenter(gp);
        //attractRubberBand_->show();
        annotationView_->attractAllItems(gp);
        annotationView_->setItemMetaVisible(true);
      } else if (cmdPressed) {
        emit message(tr("capture annotations"));
        pauseRubberBand_->press(gp);
      } else if (shiftPressed) {
        annotationView_->setNearbyItemExpelled(true);
        annotationView_->setHoveredItemRemoved(true);
        annotationView_->removeItems(gp);
        annotationView_->expelItems(gp);
        Application::globalInstance()->setCursor(Qt::OpenHandCursor);
      }
      //else if (hub_->isMediaTokenMode() && annotationView_->isItemVisible() && (
      //    hub_->isFullScreenWindowMode() ||
      //    !player_->isStopped() && dataManager_->hasAnnotations() //&& annotationView_->hasItems()
      //  ))
      //  annotationView_->setNearbyItemAttracted(true);
      else {
#ifdef Q_OS_MAC
        if (videoView_->isViewVisible() && player_->titleCount() > 1)
          videoView_->setViewMousePressPos(globalPos);
#endif // Q_OS_MAC
        if (!isFullScreen() && !isMaximized() && dragPos_ == BAD_POS)
          dragPos_ = globalPos - frameGeometry().topLeft();
      }
    } event->accept(); break;

  case Qt::MiddleButton:
    if (resumeRubberBand_->isPressed())
      resumeRubberBand_->release();
    if (pauseRubberBand_->isPressed())
      pauseRubberBand_->release();
    if (removeRubberBand_->isPressed())
      removeRubberBand_->release();
    if (event->modifiers() == Qt::ControlModifier + Qt::MetaModifier
#ifdef Q_OS_WIN
        || QtWin::isKeyControlPressed() && QtWin::isKeyAltPressed()
#endif // Q_OS_WIN
    )
      AnnotationSettings::globalSettings()->resetOffset();
    else if (event->modifiers() == Qt::ControlModifier
#ifdef Q_OS_WIN
        || QtWin::isKeyControlPressed()
#endif // Q_OS_WIN
    )
      resetAnnotationScale();
    else if (event->modifiers() == Qt::AltModifier
#ifdef Q_OS_WIN
        || QtWin::isKeyAltPressed()
#endif // Q_OS_WIN
    )
      AnnotationSettings::globalSettings()->resetRotation();
    else {
      //removeRubberBand_->press(gp);
//      annotationView_->setHoveredItemRemoved(true);
//#ifdef Q_OS_MAC
      annotationView_->setNearbyItemExpelled(true);
//#endif // Q_OS_MAC
    }
    event->accept();
    break;

  case Qt::RightButton:
    if (resumeRubberBand_->isPressed())
      resumeRubberBand_->release();
    if (pauseRubberBand_->isPressed())
      pauseRubberBand_->release();
    if (removeRubberBand_->isPressed())
      removeRubberBand_->release();
#ifndef Q_OS_MAC
    if ((event->modifiers() & Qt::ControlModifier)
#ifdef Q_OS_WIN
        || QtWin::isKeyControlPressed()
#endif // Q_OS_WIN
    ) {
      removeRubberBand_->press(gp);
      cancelContextMenu_ = true;
    } else {
      //annotationView_->setHoveredItemRemoved(true);
      annotationView_->setNearbyItemAttracted(true);
      if (event->modifiers()) {
        annotationView_->attractAllItems(gp);
        //annotationView_->removeItems(gp);
        //annotationView_->expelAllItems(gp);
        Application::globalInstance()->setCursor(Qt::ClosedHandCursor);
        cancelContextMenu_ = true;
      }
    }
#endif // !Q_OS_MAC
    event->accept();
    break;

  case Qt::LeftButton + Qt::RightButton:
    cancelContextMenu_ = true;
    pauseRubberBand_->press(gp);
    event->accept();
    break;
  case Qt::MiddleButton + Qt::RightButton:
    cancelContextMenu_ = true;
    resumeRubberBand_->press(gp);
    event->accept();
    break;
  case Qt::LeftButton + Qt::MiddleButton:
    removeRubberBand_->press(gp);
    event->accept();
    break;
  default: ;
  }

  //if (annotationView_->isHoveredItemPaused())
  //  annotationView_->pauseItems(gp);
  //if (annotationView_->isHoveredItemResumed())
  //  annotationView_->resumeItems(gp);
  //if (annotationView_->isHoveredItemRemoved())
  //  annotationView_->removeItems(gp);
  //if (annotationView_->isNearbyItemAttracted())
  //  annotationView_->attractItems(gp);
  //if (annotationView_->isNearbyItemExpelled())
  //  annotationView_->expelItems(gp);

  Base::mousePressEvent(event);
  //DOUT("exit");
}

void
MainWindow::updateAnnotationHoverGesture()
{
  /*
#ifdef Q_OS_WIN
  bool left = QtWin::isMouseLeftButtonPressed(),
       right = QtWin::isMouseRightButtonPressed(),
       middle = QtWin::isMouseMiddleButtonPressed();
  if (left && right || left && middle || right && middle) {
    annotationView_->setHoveredItemPaused(false);
    annotationView_->setHoveredItemResumed(false);
    annotationView_->setHoveredItemRemoved(false);
    annotationView_->setNearbyItemExpelled(false);
    annotationView_->setNearbyItemAttracted(false);
    bool shift = QtWin::isKeyShiftPressed(),
         ctrl = QtWin::isKeyControlPressed(),
         alt =  QtWin::isKeyAltPressed();
    if (ctrl) {
      annotationView_->setHoveredItemPaused(true);
      if (QApplication::overrideCursor())
        QApplication::restoreOverrideCursor();
      QApplication::setOverrideCursor(Qt::ClosedHandCursor);
    } else if (alt) {
      annotationView_->setHoveredItemResumed(true);
      if (QApplication::overrideCursor())
        QApplication::restoreOverrideCursor();
      QApplication::setOverrideCursor(Qt::OpenHandCursor);
    } else if (shift) {
      annotationView_->setHoveredItemRemoved(true);
      if (QApplication::overrideCursor())
        QApplication::restoreOverrideCursor();
      QApplication::setOverrideCursor(Qt::ForbiddenCursor);
    } else {
      annotationView_->setNearbyItemExpelled(true);
      if (QApplication::overrideCursor())
        QApplication::restoreOverrideCursor();
      QApplication::setOverrideCursor(Qt::ForbiddenCursor);
    }
  }
#endif // Q_OS_WIN
  */
}

void
MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
  //DOUT("enter: globalPos =" << event->globalPos());
  Q_ASSERT(event);
  Application::globalInstance()->setCursor(Qt::ArrowCursor);

  holdTimer_->stop();

  if (rippleFilter_) {
     rippleFilter_->clearCenter();
     rippleFilter_->setDampling(4);
  }

  dragPos_ = BAD_POS;

  if (annotationView_->isNearbyItemAttracted() &&
      pressedPos_ != event->globalPos()) {
    if (player_->isPlaying() ||
        hub_->isSignalTokenMode() && hub_->isPlaying())
      annotationView_->setItemMetaVisible(false);
    if (!(hub_->isMediaTokenMode() && player_->isPaused() ||
          hub_->isSignalTokenMode() && hub_->isPaused()))
      annotationView_->resume();
  }

  //cancelContextMenu_ = annotationView_->isNearbyItemExpelled() ||
  //                     annotationView_->isHoveredItemPaused() ||
  //                     annotationView_->isHoveredItemRemoved() ||
  //                     annotationView_->isHoveredItemResumed();
  //DOUT("cancelContextMenu =" << cancelContextMenu_);
  //if (cancelContextMenu_) {
    annotationView_->setHoveredItemPaused(false);
    annotationView_->setHoveredItemResumed(false);
    annotationView_->setHoveredItemRemoved(false);
    annotationView_->setNearbyItemExpelled(false);
    annotationView_->setNearbyItemAttracted(false);
  //}

  //if (event->button() == Qt::MiddleButton && event->modifiers() == Qt::NoModifier
  //    && removeRubberBand_->isEmpty()) {
  //  //hub_->toggleMiniPlayerMode();
  //  if (!hub_->isMiniPlayerMode())
  //    hub_->setMiniPlayerMode();
  //  else if (hub_->isPlaying() || player_->isPlaying())
  //    hub_->setEmbeddedPlayerMode();
  //  else
  //    hub_->setNormalPlayerMode();
  //}

  if (removeRubberBand_->isPressed())
    removeRubberBand_->release();
  if (resumeRubberBand_->isPressed())
    resumeRubberBand_->release();
  if (pauseRubberBand_->isPressed())
    pauseRubberBand_->release();

#ifndef Q_OS_MAC
  if (event->buttons() == Qt::RightButton && !contextMenu_->isVisible()) {
    QContextMenuEvent cm(QContextMenuEvent::Mouse, event->pos(), event->globalPos(), event->modifiers());
    QCoreApplication::sendEvent(this, &cm);
  }
#endif // !Q_OS_MAC

#ifdef Q_OS_MAC
  if (event->button() == Qt::LeftButton && // event->buttons() is always zero, qt4 bug?
      videoView_->isViewVisible() && player_->titleCount() > 1)
    videoView_->setViewMouseReleasePos(event->globalPos());
#endif // Q_OS_MAC

  if (pressedButtons_ == Qt::MiddleButton && pressedPos_ == event->globalPos()) {
    if (hub_->isEmbeddedPlayerMode()) {
      if (hub_->isFullScreenWindowMode()) {
        QPoint gp = mapFromGlobal(event->globalPos());
        gp.rx() -= miniPlayer_->width() / 4;
        gp.ry() += 10;
        miniPlayer_->move(gp);
      }
      hub_->setMiniPlayerMode();
    } else if (hub_->isMiniPlayerMode())
      hub_->setEmbeddedPlayerMode();
  }

  event->accept();

  Base::mouseReleaseEvent(event);
  //DOUT("exit");
}

bool
MainWindow::isGlobalPosNearEmbeddedPlayer(const QPoint &pos) const
{
  enum { margin = 6 };
  QRect r = annotationView_->globalRect();
  return pos.x() < margin || pos.x() > osdWindow_->width() - margin ||  // Near the borders
         pos.y() < margin || pos.y() > osdWindow_->height() - margin ||
  (embeddedPlayer_->isOnTop() ? ( // Near the top
         pos.x() < r.right() + margin && pos.x() > r.left() - margin &&
         pos.y() < r.bottom() - r.height() / 4 * 3 + margin && pos.y() > r.top() - margin
  ) : ( // Near the bottom
         pos.x() < r.right() + margin && pos.x() > r.left() - 6 &&
         pos.y() < r.bottom() + margin && pos.y() > r.top() + r.height() / 4 * 3 - margin
  ));
}

bool
MainWindow::isGlobalPosOverEmbeddedPositionSlider(const QPoint &pos) const
{
  enum { margin = 6 };
  QRect r(embeddedPlayer_->mapToGlobal(QPoint()), embeddedPlayer_->size()); // globalRect of embeddedPlayer
  if (embeddedPlayer_->infoView()->isVisible())
    r.setTop(r.top() + embeddedPlayer_->infoView()->height());
  if (embeddedCanvas_->isVisible() && !embeddedCanvas_->needsDisplay())
    r.setTop(r.top() + embeddedCanvas_->height());
  return pos.x() > r.left() && pos.x() < r.right() &&
    pos.y() > r.top() - margin &&
    pos.y() < r.bottom() - embeddedPlayer_->inputComboBox()->height() -10;
}

bool
MainWindow::isGlobalPosOverMainConsole(const QPoint &pos) const
{
  enum { margin = 6 };
  QRect r = annotationView_->globalRect();
  //QRect r(annotationView_->mapToGlobal(QPoint()), annotationView_->size());
  r.moveTop(r.height() * 3/8);
  r.setWidth(r.width() / 4);
  r.setHeight(r.height() / 4);

  return r.contains(pos);
}

void
MainWindow::mouseMoveEvent(QMouseEvent *event)
{
  //DOUT("enter: globalPos =" << event->globalPos());
  //osdWindow_->raise();
  QPoint globalPos = event->globalPos();
  if (globalPos == pressedPos_ || globalPos == movedPos_)
    return;
  //DOUT("enter: globalPos =" << event->globalPos());
  QPoint gp = mapFromGlobal(globalPos);

  holdTimer_->stop();

  if (magnifier_ && magnifier_->isVisible())
    magnifier_->setCenter(globalPos);

  if (rippleFilter_ && rippleFilter_->hasCenter())
    rippleFilter_->setCenter(gp);

  pressedButtons_ = Qt::NoButton;
  cancelContextMenu_ = true;
  Q_ASSERT(event);
  //resetAutoHideCursor();
  // Prevent auto hide osd player.
  if (hub_->isEmbeddedPlayerMode()) {
    if (isGlobalPosNearEmbeddedPlayer(event->globalPos())) {
      if (!hasVisiblePlayer())
        embeddedPlayer_->show();
      else if (embeddedPlayer_->isVisible())
        embeddedPlayer_->resetAutoHideTimeout();
        //QTimer::singleShot(0, embeddedPlayer_, SLOT(resetAutoHideTimeout()));
    }

    if (embeddedPlayer_->isVisible())
      embeddedCanvas_->setVisible(hub_->isMediaTokenMode() &&
        (embeddedCanvas_->underMouse() ||
         !embeddedPlayer_->isOnTop() && isGlobalPosOverEmbeddedPositionSlider(event->globalPos())));
  }

  if ((globalPos - movedPos_).manhattanLength() > 2) {
    Qt::CursorShape cursor = Qt::ArrowCursor;
    if (annotationView_->isHoveredItemPaused())
      annotationView_->pauseItems(gp);
    if (annotationView_->isHoveredItemResumed())
      annotationView_->resumeItems(gp);
    if (annotationView_->isHoveredItemRemoved()) {
      annotationView_->removeItems(gp);
      cursor = Qt::PointingHandCursor;
    }
    if (annotationView_->isNearbyItemAttracted()) {
      annotationView_->attractItems(gp);
      //annotationView_->attractAllItems(gp);
      annotationView_->setItemMetaVisible(true);
      cursor = Qt::ClosedHandCursor;
    }
    if (annotationView_->isNearbyItemExpelled()) {
      annotationView_->expelItems(gp);
      cursor = Qt::OpenHandCursor;
    }
    Application::globalInstance()->setCursor(cursor);
  }

#ifdef Q_OS_MAC
   if (videoView_->isViewVisible())
     videoView_->setViewMouseMovePos(event->globalPos());
#endif // Q_OS_MAC

  if (pauseRubberBand_->isPressed()) {
    static bool once = true;
    if (once) {
      once = false;
      emit notification(tr("use %1 with mouse or wheel to control annotations")
        .arg( K_CTRL "/" K_OPT "/" K_CTRL "+" K_OPT "(+" K_SHIFT ")")
      );
    }
  }

  switch (event->buttons()) {
  case Qt::LeftButton:
    if (pauseRubberBand_->isPressed())
      pauseRubberBand_->drag(gp);
    else if (resumeRubberBand_->isPressed())
      resumeRubberBand_->drag(gp);
    else if (removeRubberBand_->isPressed())
      removeRubberBand_->drag(gp);
    else if (dragPos_ != BAD_POS && !isFullScreen() && !isMaximized()
#ifdef Q_OS_WIN
        &&
        (
          player_->isStopped() ||
          QDateTime::currentMSecsSinceEpoch() - windowModeUpdateTime_ > QtWin::getDoubleClickInterval()
        )
#endif // Q_OS_WIN
      ) {

      QPoint globalPos = event->globalPos();
      // FIXME: out-of-screen check not working
      //QRect globalRect(globalPos, size());
      //if (!isRectOutOfScreen(globalRect))
        move(globalPos - dragPos_);

      annotationView_->updatePos();

    } break;
  default:
    if (pauseRubberBand_->isPressed())
      pauseRubberBand_->drag(gp);
    else if (resumeRubberBand_->isPressed())
      resumeRubberBand_->drag(gp);
    else if (removeRubberBand_->isPressed())
      removeRubberBand_->drag(gp);
  }

  movedPos_ = globalPos;
  event->accept();
  Base::mouseMoveEvent(event);
  //DOUT("exit");
}

void
MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
  //DOUT("enter");
  Q_ASSERT(event);
  //QApplication::setOverrideCursor(Qt::ArrowCursor);
  //resetAutoHideCursor();

  holdTimer_->stop();
  QTimer::singleShot(0, holdTimer_, SLOT(stop()));

  annotationView_->setHoveredItemPaused(false);
  annotationView_->setHoveredItemResumed(false);
  annotationView_->setHoveredItemRemoved(false);
  annotationView_->setNearbyItemExpelled(false);
  annotationView_->setNearbyItemAttracted(false);
  if (pauseRubberBand_->isPressed())
    pauseRubberBand_->cancel();
  if (resumeRubberBand_->isPressed())
    resumeRubberBand_->cancel();
  if (removeRubberBand_->isPressed())
    removeRubberBand_->cancel();

  switch (event->buttons()) {
  case Qt::LeftButton:
    if (event->modifiers()
#ifdef Q_OS_WIN
        || QtWin::isKeyShiftPressed()
#endif // Q_OS_WIN
        )
      annotationView_->attractAllItems(mapFromGlobal(event->globalPos()));
    else if (magnifier_ && magnifier_->isVisible())
      magnifier_->fadeOut();
    else if (hub_->isFullScreenWindowMode() &&
             !mainConsole_->isEmpty() && mainConsole_->isVisible() &&
#ifndef Q_OS_WIN
        mainConsole_->underMouse() &&
#endif // !Q_OS_WIN
        isGlobalPosOverMainConsole(event->globalPos()))
      showConsole();
    else if (hub_->isFullScreenWindowMode() &&
             embeddedPlayer_->isVisible() && embeddedCanvas_->isVisible() &&
             dataManager_->hasAnnotations() &&
             !embeddedPlayer_->isOnTop() &&
#ifdef Q_OS_WIN
             isGlobalPosOverEmbeddedPositionSlider(event->globalPos())
#else
             embeddedCanvas_->underMouse()
#endif // Q_OS_WIN
             )
      showAnnotationAnalytics();
    else
      hub_->toggleFullScreenWindowMode();
    event->accept();
    break;
  case Qt::MiddleButton:
    annotationView_->attractAllItems(mapFromGlobal(event->globalPos()));
    break;
//  case Qt::RightButton:
//#ifndef Q_OS_MAC
//    {
//      QPoint gp = mapFromGlobal(event->globalPos());
//      annotationView_->removeItems(gp);
//      annotationView_->expelAllItems(gp);
//      cancelContextMenu_ = true;
//    }
//#endif // Q_OS_MAC
//    break;
  default: ;
  }

  Base::mouseDoubleClickEvent(event);
  //DOUT("exit");
}

void
MainWindow::wheelEvent(QWheelEvent *event)
{
  //DOUT("enter");
  Q_ASSERT(event);
  if (!event->delta())
    return;

  if (magnifier_ && magnifier_->isVisible()) {
    QCoreApplication::sendEvent(magnifier_, event);
    return;
  }

  if (event->modifiers() == Qt::ControlModifier + Qt::AltModifier) {
    if (event->delta() > 0)
      increaseAnnotationOffset();
    else
      decreaseAnnotationOffset();
  } else if (event->modifiers() == Qt::ControlModifier
#ifdef Q_OS_WIN
    || QtWin::isMouseLeftButtonPressed()
#endif // Q_OS_WIN
  ) {
    if (!annotationView_->isPaused() && annotationView_->hasPausedItems()) {
      if (event->delta() > 0)
        annotationView_->scalePausedItems(1.1);
      else
        annotationView_->scalePausedItems(1 / 1.1);
    } else {
      if (event->delta() > 0)
        annotationScaleUp();
      else
        annotationScaleDown();
    }
  } else if (event->modifiers() == Qt::AltModifier
#ifdef Q_OS_WIN
    || QtWin::isMouseRightButtonPressed()
#endif // Q_OS_WIN
  ) {
    cancelContextMenu_ = true;
    //enum { zoom = -3 };
    //qreal angle = event->delta() / (8.0 * zoom);
    qreal angle = event->delta() > 0 ? -5 : 5;
    if (!annotationView_->isPaused() && annotationView_->hasPausedItems())
      annotationView_->rotatePausedItems(angle);
    else
      AnnotationSettings::globalSettings()->setRotation(
        AnnotationSettings::globalSettings()->rotation() + angle
      );
  } else {
    qreal vol = hub_->volume();
    vol += event->delta() / (8 * 360.0);
    vol =  vol > 1.0 ? 1.0 :vol < 0.0 ? 0.0 : vol;
    hub_->setVolume(vol);
  }
  event->accept();
  //Base::wheelEvent(event);
  //DOUT("exit");
}

void
MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
  //DOUT("enter: cancelContextMenu =" << cancelContextMenu_);
  Q_ASSERT(event);
  //resetAutoHideCursor();
  //QApplication::setOverrideCursor(Qt::ArrowCursor);
  //autoHideCursorTimer_->stop();
  //showCursor();

  annotationView_->setHoveredItemPaused(false);
  annotationView_->setHoveredItemResumed(false);
  annotationView_->setHoveredItemRemoved(false);
  annotationView_->setNearbyItemExpelled(false);
  annotationView_->setNearbyItemAttracted(false);
#ifndef Q_OS_MAC
  if (!cancelContextMenu_ &&
      (!resumeRubberBand_->isPressed() || resumeRubberBand_->isEmpty()) &&
      (!removeRubberBand_->isPressed() || removeRubberBand_->isEmpty()) &&
      (!pauseRubberBand_->isPressed() || pauseRubberBand_->isEmpty()))
#endif // !Q_OS_MAC
  {
    updateContextMenu();
    contextMenu_->popup(event->globalPos());
  }

  if (resumeRubberBand_->isPressed())
    resumeRubberBand_->cancel();
  if (removeRubberBand_->isPressed())
    removeRubberBand_->cancel();
  if (pauseRubberBand_->isPressed())
    pauseRubberBand_->cancel();

  event->accept();

  Base::contextMenuEvent(event);
  //DOUT("exit");
}

void
MainWindow::updateAnnotationSettingsMenu()
{
  updateAnnotationSubtitleMenu();
  updateAnnotationEffectMenu();
  togglePreferFloatAnnotationAct_->setChecked(AnnotationSettings::globalSettings()->preferFloat());
  toggleAnnotationAvatarVisibleAct_->setChecked(AnnotationSettings::globalSettings()->isAvatarVisible());
  toggleAnnotationMetaVisibleAct_->setChecked(AnnotationSettings::globalSettings()->isMetaVisible());
  toggleAnnotationBandwidthLimitedAct_->setChecked(annotationView_->isItemCountLimited());
  toggleAnnotationTraditionalChineseAct_->setChecked(AnnotationSettings::globalSettings()->preferTraditionalChinese());

  resetAnnotationScaleAct_->setEnabled(!qFuzzyCompare(AnnotationSettings::globalSettings()->scale(), 1.0));
  resetAnnotationRotationAct_->setEnabled(!qFuzzyCompare(AnnotationSettings::globalSettings()->rotation() + 1.0, 1.0));
  resetAnnotationOffsetAct_->setEnabled(AnnotationSettings::globalSettings()->offset());
}

void
MainWindow::updateAnnotationSubtitleMenu()
{
  toggleSubtitleAnnotationVisibleAct_->setChecked(annotationView_->isSubtitleVisible());
  toggleNonSubtitleAnnotationVisibleAct_->setChecked(annotationView_->isNonSubtitleVisible());
  toggleSubtitleOnTopAct_->setChecked(AnnotationSettings::globalSettings()->isSubtitleOnTop());

  bool t = toggleSubtitleAnnotationVisibleAct_->isChecked();
  toggleSubtitleOnTopAct_->setEnabled(t);
  subtitleColorMenu_->setEnabled(t);
}

void
MainWindow::updateGameMenu()
{
#ifdef AC_ENABLE_GAME
  toggleTranslateAct_->setEnabled(hub_->isSignalTokenMode() && server_->isConnected());
  showGamePreferencesAct_->setEnabled(hub_->isSignalTokenMode() && dataManager_->token().hasDigest());
#endif // AC_ENABLE_GAME
  //toggleSubtitleAnnotationVisibleAct_->setChecked(annotationView_->isSubtitleVisible());
  //toggleSubtitleOnTopAct_->setChecked(isSubtitleOnTop());

  //updateTranslatorMenu();
}

void
MainWindow::updateContextMenu()
{
  //DOUT("enter");
  contextMenu_->clear();
  if (!contextMenuActions_.isEmpty()) {
    foreach (QAction *a, contextMenuActions_)
      if (a)
        QTimer::singleShot(0, a, SLOT(deleteLater()));
    contextMenuActions_.clear();
  }

  //bool online = server_->isConnected() && server_->isAuthorized();

  {
    //// Live mode
    //if (online &&
    //    (hub_->isLiveTokenMode() || hub_->isStopped() && player_->isStopped())) {
    //  toggleLiveModeAct_->setChecked(hub_->isLiveTokenMode());
    //  contextMenu_->addAction(toggleLiveModeAct_);
//
    //  //toggleLiveDialogVisibleAct_->setChecked(liveDialog_->isVisible());
    //  //contextMenu_->addAction(toggleLiveDialogVisibleAct_);
    //}
#ifndef Q_OS_MAC
    //contextMenu_->addAction(newWindowAct_);
#endif // !Q_OS_MAC
    QString url = currentUrl();
    if (!url.isEmpty()) {
      copyCurrentUrlAct_->setText(tr("Copy") + ": " + shortenText(url));
      downloadCurrentUrlAct_->setVisible(!player_->isDownloadFinished());
      contextMenu_->addMenu(currentMenu_);
    }

    QString title = currentTitle();
    if (!title.isEmpty()) {
      copyCurrentTitleAct_->setText(tr("Copy") + ": " + shortenText(title));
      contextMenu_->addMenu(searchMenu_);
    }

    contextMenu_->addSeparator();
  }

  // Open
  if (!hub_->isSignalTokenMode() || hub_->isStopped()) {
    //contextMenu_->addSeparator();

    //contextMenu_->addAction(openAct_);

    openMenu_->clear(); {
      openMenu_->addAction(showLibraryAct_);
      openMenu_->addSeparator();
      openMenu_->addAction(openFileAct_);
      openMenu_->addAction(openUrlAct_);
      openMenu_->addAction(openVideoDeviceAct_);
#ifdef Q_OS_WIN // TODO add support for Mac/Linux
      openMenu_->addAction(openAudioDeviceAct_);
#endif // Q_OS_WIN
      openMenu_->addAction(openDeviceAct_);

      if (hub_->isMediaTokenMode() && player_->hasMedia()) {
        openMenu_->addSeparator();
        openMenu_->addAction(openSubtitleAct_);
      }
    }
    contextMenu_->addMenu(openMenu_);

    if (!browsedFiles_.isEmpty())
      contextMenu_->addMenu(browseMenu_);

    // Recent
    //updateRecent();
    if (!recentFiles_.isEmpty())
      contextMenu_->addMenu(recentMenu_);

    // - Playlist
    //updatePlaylist();
    if (playlist_.size() > 1 && hub_->isMediaTokenMode())
      contextMenu_->addMenu(playlistMenu_);

    // Tracks
    if (hub_->isMediaTokenMode() && player_->hasMedia() &&
        player_->trackCount() > 1)
      //updateTrackMenu();
      contextMenu_->addMenu(trackMenu_);

    // DVD sections
    if (hub_->isMediaTokenMode() && player_->hasMedia() &&
        player_->titleCount() > 1) {

      sectionMenu_->clear();

      if (player_->titleId() > 0)
        sectionMenu_->addAction(previousSectionAct_);
      if (player_->titleId() < player_->titleCount() - 1)
        sectionMenu_->addAction(nextSectionAct_);
      sectionMenu_->addSeparator();

      for (int tid = 0; tid < player_->titleCount(); tid++) {
        QString text = TR(T_SECTION) + " " + QString::number(tid+1);
        auto a = new QxActionWithId(tid, text, sectionMenu_);
        contextMenuActions_.append(a);
        if (tid == player_->titleId()) {
#ifdef Q_OS_WIN
          a->setIcon(QIcon(RC_IMAGE_CURRENTSECTION));
#else
          a->setCheckable(true);
          a->setChecked(true);
#endif // Q_OS_WIN
        }
        connect(a, SIGNAL(triggeredWithId(int)), player_, SLOT(setTitleId(int)));
        sectionMenu_->addAction(a);
      }

      contextMenu_->addMenu(sectionMenu_);
    }

    contextMenu_->addSeparator();
    if (hub_->isMediaTokenMode() && player_->hasMedia()) {
      contextMenu_->addMenu(importAnnotationMenu_);

      // Subtitle menu

      subtitleMenu_->clear();
      if (player_->hasSubtitles()) {
        bool t = player_->isSubtitleVisible();
        toggleSubtitleVisibleAct_->setChecked(t);

        QStringList l = player_->subtitleDescriptions();
        int id = 0;
        foreach (QString subtitle, l) {
          if (subtitle.isEmpty())
            subtitle = TR(T_SUBTITLE) + " " + QString::number(id+1);
          else
            subtitle = QString::number(id+1) + ". " + subtitle;
          auto a = new QxActionWithId(id, subtitle, subtitleMenu_);
          contextMenuActions_.append(a);
          if (id == player_->subtitleId()) {
#ifdef Q_OS_WIN
            a->setIcon(QIcon(RC_IMAGE_CURRENTSUBTITLE));
#else
            a->setCheckable(true);
            a->setChecked(true);
#endif // Q_OS_WIN
          }
          a->setEnabled(t);
          if (t)
            connect(a, SIGNAL(triggeredWithId(int)), player_, SLOT(setSubtitleId(int)));
          subtitleMenu_->addAction(a);
          id++;
        }

        subtitleMenu_->addSeparator();
        subtitleMenu_->addAction(toggleSubtitleVisibleAct_);
      }
      subtitleMenu_->addAction(openSubtitleAct_);
      contextMenu_->addMenu(subtitleMenu_);

      updateAspectRatioMenu();
      contextMenu_->addMenu(videoMenu_);

      updateAudioChannelMenu();

      // Audio track menu
      if (player_->hasAudioTracks()) {
        audioTrackMenu_->clear();

        QStringList l = player_->audioTrackDescriptions();
        int id = 0;
        foreach (QString name, l) {
          if (name.isEmpty())
            name = TR(T_AUDIOTRACK) + " " + QString::number(id+1);
          else
            name = QString::number(id+1) + ". " + name;
          auto a = new QxActionWithId(id, name, audioTrackMenu_);
          contextMenuActions_.append(a);
          if (id == player_->audioTrackId()) {
#ifdef Q_OS_WIN
            a->setIcon(QIcon(RC_IMAGE_CURRENTTRACK));
#else
            a->setCheckable(true);
            a->setChecked(true);
#endif // Q_OS_WIN
          }
          connect(a, SIGNAL(triggeredWithId(int)), player_, SLOT(setAudioTrackId(int)));
          audioTrackMenu_->addAction(a);
          id++;
        }
        audioTrackMenu_->setEnabled(true);
      } else
        audioTrackMenu_->setEnabled(false);

      setAudioDelayAct_->setText(tr("Audio Delay"));
      qint64 secs = player_->audioDelay() / 1000;
      if (secs)
        setAudioDelayAct_->setText(setAudioDelayAct_->text() + ": " + QString::number(secs) + tr(" sec."));

      contextMenu_->addMenu(audioMenu_);
    }
  }

  // Basic control
  contextMenu_->addSeparator();
  if (hub_->isSignalTokenMode()) {
    contextMenu_->addAction(snapshotAct_);
    if (!hub_->isStopped())
      contextMenu_->addAction(promptStopAct_);

  } else if (player_->hasMedia()) {
    contextMenu_->addAction(isPlaying() ? pauseAct_ : playAct_);

    playMenu_->clear();

    // jichi 11/18/2011: Removed from play menu only in order to save space ....
    //if (hub_->isMediaTokenMode())
    //  playMenu_->addAction(nextFrameAct_);

    if (hub_->isMediaTokenMode())
      playMenu_->addAction(replayAct_);

    if (!hub_->isMediaTokenMode() && !hub_->isStopped() ||
        hub_->isMediaTokenMode() && !player_->isStopped())
      playMenu_->addAction(stopAct_);

    if (hub_->isMediaTokenMode()) {
      playMenu_->addAction(previousAct_);
      playMenu_->addAction(nextAct_);
    }
    playMenu_->addSeparator();
    if (!hub_->isMediaTokenMode() || !player_->isStopped())
      playMenu_->addAction(snapshotAct_);
    if (hub_->isMediaTokenMode())
      playMenu_->addAction(snapshotAllAct_);
    if (hub_->isMediaTokenMode()) {
      playMenu_->addSeparator();
      playMenu_->addAction(toggleAutoPlayNextAct_);
      playMenu_->addAction(toggleRepeatCurrentAct_);
      playMenu_->addAction(toggleNoRepeatAct_);
    }

    contextMenu_->addMenu(playMenu_);
  }
  if (hub_->isMediaTokenMode() && player_->hasMedia()) {
    contextMenu_->addMenu(backwardMenu_);
    contextMenu_->addMenu(forwardMenu_);

    contextMenu_->addAction(showSeekAct_);
  }

  // Modes
  {
    contextMenu_->addSeparator();

#ifdef WITH_WIN_PICKER
    contextMenu_->addAction(showWindowPickerAct_);

    toggleWindowTrackingAct_->setChecked(annotationView_->trackedWindow());
    contextMenu_->addAction(toggleWindowTrackingAct_);
#endif // WITH_WIN_PICKER

    toggleFullScreenModeAct_->setChecked(hub_->isFullScreenWindowMode());
    contextMenu_->addAction(toggleFullScreenModeAct_);

    toggleMiniModeAct_->setChecked(hub_->isMiniPlayerMode());
    contextMenu_->addAction(toggleMiniModeAct_);

    toggleEmbeddedModeAct_->setChecked(hub_->isEmbeddedPlayerMode());
    contextMenu_->addAction(toggleEmbeddedModeAct_);

    if (hub_->isMediaTokenMode() && player_->hasMedia()) {
      QSize sz = player_->videoDimension();
      if (!sz.isEmpty()) {
        actualSizeAct_->setChecked(sz == size());

        QString t = TR(T_ACTUALSIZE) + QString(": %1x%2")
            .arg(QString::number(sz.width())).arg(QString::number(sz.height()));
        actualSizeAct_->setText(t);
        actualSizeAct_->setEnabled(!annotationView_->trackedWindow());
        contextMenu_->addAction(actualSizeAct_);
      }
    }
  }

  // Annotation filter
  {
    contextMenu_->addSeparator();

    toggleAnnotationFilterEnabledAct_->setChecked(annotationFilter_->isEnabled());
    contextMenu_->addAction(toggleAnnotationFilterEnabledAct_);
    if (annotationFilter_->isEnabled()) {

      contextMenu_->addAction(showBlacklistAct_);

      //toggleAnnotationCountDialogVisibleAct_->setChecked(annotationCountDialog_ && annotationCountDialog_->isVisible());
      //int count = annotationFilter_->annotationCountHint();
      //QString text = count <= 0 ? TR(T_MENUTEXT_ANNOTATIONLIMIT) :
      //  QString("%1 (%2)").arg(TR(T_MENUTEXT_ANNOTATIONLIMIT))
      //                    .arg(QString::number(count));
      //toggleAnnotationCountDialogVisibleAct_->setText(text);
      //contextMenu_->addAction(toggleAnnotationCountDialogVisibleAct_ );

      contextMenu_->addMenu(annotationLanguageMenu_);
    }
  }

  // Annotations
  {
    contextMenu_->addSeparator();

    toggleAnnotationVisibleAct_->setChecked(annotationView_ && annotationView_->isItemVisible());
    contextMenu_->addAction(toggleAnnotationVisibleAct_ );

    bool t = toggleAnnotationVisibleAct_->isChecked();

    annotationSettingsMenu_->setEnabled(t);
    //resumeAnnotationAct_->setEnabled(t);

    contextMenu_->addAction(showAnnotationBrowserAct_ );

    contextMenu_->addAction(showAnnotationSourceAct_ );

    updateAnnotationsAct_->setEnabled(dataManager_->token().isValid() && server_->isConnected());
    contextMenu_->addAction(updateAnnotationsAct_);

    annotationMenu_->clear(); {
      annotationMenu_->addAction(showBacklogAct_);

      annotationMenu_->addAction(showAnnotationAnalyticsAct_);
      showAnnotationAnalyticsAct_->setEnabled(dataManager_->hasAnnotations());

      annotationMenu_->addSeparator();

      annotationMenu_->addAction(resumeAnnotationAct_);

    //if (ALPHA) if (commentView_ && commentView_->tokenId()) {
    //  toggleCommentViewVisibleAct_->setChecked(commentView_ && commentView_->isVisible());
    //  annotationMenu_->addAction(toggleCommentViewVisibleAct_ );
    //}
    }
    contextMenu_->addMenu(annotationMenu_);

    if (t)
      updateAnnotationSettingsMenu();
    contextMenu_->addMenu(annotationSettingsMenu_);
  }

#ifdef AC_ENABLE_GAME
  if (hub_->isSignalTokenMode()) {
    contextMenu_->addSeparator();
    updateGameTextMenu();
    contextMenu_->addMenu(gameTextMenu_);

    contextMenu_->addMenu(translatorMenu_);
    contextMenu_->addAction(toggleTranslateAct_);
#ifdef WITH_WIN_ATLAS
    contextMenu_->addAction(toggleAtlasEnabledAct_);
#endif // WITH_WIN_ATLAS

    contextMenu_->addAction(toggleSubtitleOnTopAct_);
    contextMenu_->addAction(showBacklogAct_);
  }

  contextMenu_->addSeparator();
  if (hub_->isSignalTokenMode() || player_->isStopped()) {
    contextMenu_->addMenu(gameMenu_);
    updateGameMenu();
  }

  if (hub_->isStopped() &&
      (hub_->isSignalTokenMode() || !player_->hasMedia())) {
    openCurrentGameAct_->setEnabled(!gameLibrary_->isEmpty());
    contextMenu_->addAction(openCurrentGameAct_);
    contextMenu_->addAction(toggleAppLocaleEnabledAct_);
  }
  if (hub_->isSignalTokenMode())
    contextMenu_->addAction(showGamePreferencesAct_);

#endif // AC_ENABLE_GAME

  // Network
  {
    contextMenu_->addSeparator();
    if (!server_->isAuthorized())
      contextMenu_->addAction(loginAct_);
  }

//  if (ALPHA)
//  if (player_->hasMedia()
//#ifdef AC_ENABLE_GAME
//      && hub_->isMediaTokenMode()
//#endif // AC_ENABLE_GAME
//      ) {
//    // Sync mode
//    contextMenu_->addSeparator();
//
//    toggleSyncModeAct_->setChecked(hub_->isSyncPlayMode());
//    contextMenu_->addAction(toggleSyncModeAct_);
//
//    toggleSyncDialogVisibleAct_->setChecked(syncDialog_ && syncDialog_->isVisible());
//    contextMenu_->addAction(toggleSyncDialogVisibleAct_);
//  }

  // App
  {
    contextMenu_->addSeparator();

    // Language
    if (player_->isStopped() && hub_->isStopped() ||
        hub_->isLiveTokenMode()) {
      contextMenu_->addMenu(appLanguageMenu_);

      //int l = Localizer::globalInstance()->language();
      int l = AcSettings::globalSettings()->language();
      setAppLanguageToJapaneseAct_->setChecked(l == Traits::Japanese);
      setAppLanguageToTraditionalChineseAct_->setChecked(l == Traits::TraditionalChinese);
      setAppLanguageToSimplifiedChineseAct_->setChecked(l == Traits::SimplifiedChinese);

      switch (l) {
      case Traits::Japanese:
      case Traits::TraditionalChinese:
      case Traits::SimplifiedChinese:
        setAppLanguageToEnglishAct_->setChecked(false);
        break;
      default:
        setAppLanguageToEnglishAct_->setChecked(true);
      }
    }

  }

  // Help
  {
    contextMenu_->addSeparator();

    contextMenu_->addAction(preferencesAct_);

    updateSettingsMenu();
    contextMenu_->addMenu(settingsMenu_);

    //contextMenu_->addAction(checkInternetConnectionAct_);
    toggleMagnifierVisibleAct_->setChecked(magnifier_ && magnifier_->isVisible());
    contextMenu_->addMenu(utilityMenu_);

    contextMenu_->addMenu(helpContextMenu_);
    contextMenu_->addAction(quitAct_);
  }
  //DOUT("exit");
}

void
MainWindow::updateTranslatorMenu()
{
  toggleGoogleTranslatorAct_->setChecked(translator_->hasService(TranslatorManager::Google));
  toggleExciteTranslatorAct_->setChecked(translator_->hasService(TranslatorManager::Excite));
  toggleMicrosoftTranslatorAct_->setChecked(translator_->hasService(TranslatorManager::Microsoft));
  toggleYahooTranslatorAct_->setChecked(translator_->hasService(TranslatorManager::Yahoo));
  toggleFresheyeTranslatorAct_->setChecked(translator_->hasService(TranslatorManager::Fresheye));
  toggleOcnTranslatorAct_->setChecked(translator_->hasService(TranslatorManager::Ocn));
  toggleRomajiTranslatorAct_->setChecked(translator_->hasService(TranslatorManager::Romaji));
  toggleSdlTranslatorAct_->setChecked(translator_->hasService(TranslatorManager::Sdl));
  toggleSystranTranslatorAct_->setChecked(translator_->hasService(TranslatorManager::Systran));
  toggleNiftyTranslatorAct_->setChecked(translator_->hasService(TranslatorManager::Nifty));
  toggleInfoseekTranslatorAct_->setChecked(translator_->hasService(TranslatorManager::Infoseek));
}

void
MainWindow::updateAspectRatioMenu()
{
  QString ratio;
  if (player_ && player_->hasMedia())
    ratio = player_->aspectRatio();

  setDefaultAspectRatioAct_->setChecked(ratio.isEmpty());
  setStandardAspectRatioAct_->setChecked(ratio == "4:3");
  setWideScreenAspectRatioAct_->setChecked(ratio == "16:9");
}

void
MainWindow::updateAudioChannelMenu()
{
  Player::AudioChannel ch = player_->hasMedia() ? player_->audioChannel() : Player::NoChannel;
  if (ch && player_->audioChannelCount() > 1) {
    setLeftChannelAct_->setChecked(ch == Player::LeftChannel);
    setRightChannelAct_->setChecked(ch == Player::RightChannel);
    setStereoChannelAct_->setChecked(ch == Player::StereoChannel);
    setReverseStereoChannelAct_->setChecked(ch == Player::ReverseStereoChannel);
    setDolbysChannelAct_->setChecked(ch == Player::DolbysChannel);

    audioChannelMenu_->setEnabled(true);
  } else
    audioChannelMenu_->setEnabled(false);
}

void
MainWindow::updateSettingsMenu()
{
  //updateTranslatorMenu();

  toggleSubmitAct_->setVisible(server_->user().isDeveloper());
  toggleMenuThemeEnabledAct_->setVisible(server_->user().isDeveloper());

  togglePlayerLabelEnabled_->setChecked(embeddedCanvas_->isEnabled());
  toggleEmbeddedPlayerOnTopAct_->setChecked(embeddedPlayer_->isOnTop());
  //toggleSiteAccountViewVisibleAct_->setChecked(siteAccountView_ && siteAccountView_->isVisible());
  //toggleNetworkProxyDialogVisibleAct_->setChecked(networkProxyDialog_ && networkProxyDialog_->isVisible());
  toggleWindowOnTopAct_->setChecked(isWindowOnTop());
  toggleClipboardMonitorEnabledAct_->setChecked(clipboardMonitor_->isEnabled());
  togglePreferLocalDatabaseAct_->setChecked(dataServer_->preferCache());

#ifdef Q_OS_LINUX
  toggleMenuBarVisibleAct_->setChecked(menuBar()->isVisible());
#endif // Q_OS_LINUX

  toggleSaveMediaAct_->setChecked(player_->isBufferSaved());
  toggleSubmitAct_->setChecked(submit_);

  toggleMenuThemeEnabledAct_->setChecked(AcUi::globalInstance()->isMenuEnabled());

  //toggleMultipleWindowsEnabledAct_->setChecked(Settings::globalSettings()->isMultipleWindowsEnabled());

  bool themeEnabled = true;
#ifdef Q_OS_WIN
  if (QtWin::isWindowsVistaOrLater()) {
    bool aero = AcUi::globalInstance()->isAeroEnabled();
    toggleAeroEnabledAct_->setChecked(aero);
    themeEnabled = !aero;
    if (themeEnabled != themeMenu_->isEnabled()) {
      themeMenu_->setEnabled(themeEnabled);
      themeMenu_->setVisible(themeEnabled);
     }
  }
#endif // Q_OS_WIN
  if (themeEnabled)
    updateMenuTheme();
}

void
MainWindow::updateMenuTheme()
{
  AcUi::Theme t = AcUi::globalInstance()->theme();
  setThemeToDefaultAct_->setChecked(t == AcUi::DefaultTheme);
  setThemeToRandomAct_->setChecked(t == AcUi::RandomTheme);
  setThemeToDarkAct_->setChecked(t == AcUi::DarkTheme);
  setThemeToBlackAct_->setChecked(t == AcUi::BlackTheme);
  setThemeToBlueAct_->setChecked(t == AcUi::BlueTheme);
  setThemeToBrownAct_->setChecked(t == AcUi::BrownTheme);
  setThemeToCyanAct_->setChecked(t == AcUi::CyanTheme);
  setThemeToGrayAct_->setChecked(t == AcUi::GrayTheme);
  setThemeToGreenAct_->setChecked(t == AcUi::GreenTheme);
  setThemeToPinkAct_->setChecked(t == AcUi::PinkTheme);
  setThemeToPurpleAct_->setChecked(t == AcUi::PurpleTheme);
  setThemeToRedAct_->setChecked(t == AcUi::RedTheme);
  setThemeToWhiteAct_->setChecked(t == AcUi::WhiteTheme);
  setThemeToYellowAct_->setChecked(t == AcUi::YellowTheme);
}

void
MainWindow::updateUserMenu()
{
  userMenu_->clear();
  if (!server_->isAuthorized())
    return;

#ifdef AC_ENABLE_GAME
  toggleTranslateAct_->setEnabled(hub_->isSignalTokenMode() && server_->isConnected());
#endif // AC_ENABLE_GAME

  // Menu

  //if (userMenu_->isEmpty()) {

#ifdef AC_ENABLE_GAME
    if (hub_->isSignalTokenMode() && !hub_->isStopped()) {
      userMenu_->addAction(showGamePreferencesAct_);
      showGamePreferencesAct_->setEnabled(hub_->isSignalTokenMode() && dataManager_->token().hasDigest());

      userMenu_->addMenu(gameTextMenu_);
      updateGameTextMenu();

      userMenu_->addMenu(translatorMenu_);
      userMenu_->addAction(toggleTranslateAct_);
#ifdef WITH_WIN_ATLAS
      userMenu_->addAction(toggleAtlasEnabledAct_);
#endif // WITH_WIN_ATLAS
      userMenu_->addAction(showBacklogAct_);
      userMenu_->addAction(showAnnotationAnalyticsAct_);
      showAnnotationAnalyticsAct_->setEnabled(dataManager_->hasAnnotations());
      userMenu_->addSeparator();
    }
#endif // AC_ENABLE_GAME

    userMenu_->addMenu(userLanguageMenu_);
    userMenu_->addAction(toggleUserAnonymousAct_);
    userMenu_->addAction(showUserAct_);
    userMenu_->addAction(logoutAct_);
  //}

  updateTranslatorMenu();

  //if (ALPHA) {
  //  if (server_->isConnected())
  //    userMenu_->addAction(disconnectAct_);
  //  else
  //    userMenu_->addAction(connectAct_);
  //}

  // Actions

  toggleUserAnonymousAct_->setChecked(server_->user().isAnonymous());

  {
    setUserLanguageToEnglishAct_->setChecked(server_->user().language() == Traits::English);
    setUserLanguageToJapaneseAct_->setChecked(server_->user().language() == Traits::Japanese);
    setUserLanguageToTraditionalChineseAct_->setChecked(server_->user().language() == Traits::TraditionalChinese);
    setUserLanguageToSimplifiedChineseAct_->setChecked(server_->user().language() == Traits::SimplifiedChinese);
    setUserLanguageToKoreanAct_->setChecked(server_->user().language() == Traits::Korean);
    setUserLanguageToFrenchAct_->setChecked(server_->user().language() == Traits::French);
    setUserLanguageToGermanAct_->setChecked(server_->user().language() == Traits::German);
    setUserLanguageToItalianAct_->setChecked(server_->user().language() == Traits::Italian);
    setUserLanguageToSpanishAct_->setChecked(server_->user().language() == Traits::Spanish);
    setUserLanguageToPortugueseAct_->setChecked(server_->user().language() == Traits::Portuguese);
    setUserLanguageToRussianAct_->setChecked(server_->user().language() == Traits::Russian);
    //setUserLanguageToUnknownAct_->setChecked(server_->user().language() == Traits::UnknownLanguage);
  }
}

void
MainWindow::updateTrackMenu()
{
  trackMenu_->clear();
  if (!player_->hasMedia() || !player_->hasPlaylist())
    return;

  foreach (Player::MediaInfo mi, player_->playlist()) {
    QString text = QString("%1. %2").arg(QString::number(mi.track + 1)).arg(mi.title);
    auto a = new QxActionWithId(mi.track, text, trackMenu_);
    a->setToolTip(mi.path);
    if (mi.track == player_->trackNumber()) {
#ifdef Q_OS_LINUX
      a->setCheckable(true);
      a->setChecked(true);
#else
      a->setIcon(QIcon(RC_IMAGE_CURRENTTRACK));
#endif // Q_OS_LINUX
    }
    connect(a, SIGNAL(triggeredWithId(int)), player_, SLOT(setTrackNumber(int)));
    trackMenu_->addAction(a);
  }

  trackMenu_->addSeparator();

  trackMenu_->addAction(QIcon(RC_IMAGE_PREVIOUS), TR(T_PREVIOUS), player_, SLOT(previousTrack()));
  trackMenu_->addAction(QIcon(RC_IMAGE_NEXT), TR(T_NEXT), player_, SLOT(nextTrack()));
}

// - Move and resize events -

void
MainWindow::moveEvent(QMoveEvent *event)
{
  Base::moveEvent(event);
  annotationView_->updatePos();
  osdWindow_->raise();
}

void
MainWindow::resizeEvent(QResizeEvent *event)
{
  Base::resizeEvent(event);
  annotationView_->updateSize();
  osdWindow_->raise();
  if (hub_->isMediaTokenMode() && player_->hasMedia())
    updateWindowTitle();
}

void
MainWindow::changeEvent(QEvent *event)
{
  Q_ASSERT(event);
  if (event->type() == QEvent::WindowStateChange) {
    holdTimer_->stop();
    if (isMaximized())
      emit windowMaximized();
  }
  Base::changeEvent(event);

#ifdef Q_OS_LINUX
  if (event->type() == QEvent::WindowStateChange) {
    if (windowState() == Qt::WindowFullScreen)
      QtX11::setWindowInputShape(osdWindow_->winId(), embeddedPlayer_->pos(), embeddedPlayer_->rect());
    else
      QtX11::zeroWindowInputShape(osdWindow_->winId());
  }
#endif // Q_OS_LINUX
}

// - Keyboard events -

void
MainWindow::keyReleaseEvent(QKeyEvent *event)
{
  //DOUT("enter");
#ifdef Q_OS_WIN
  switch (event->key()) {
  case Qt::Key_Shift: case Qt::Key_Alt: case Qt::Key_Control:
    updateAnnotationHoverGesture();
  }
#endif // Q_OS_WIN
  Base::keyReleaseEvent(event);
  //DOUT("exit");
}

void
MainWindow::keyPressEvent(QKeyEvent *event)
{
  Q_ASSERT(event);
  switch (event->key()) {
    //if (!hasVisiblePlayer() || !visiblePlayer()->lineEdit()->hasFocus())
  case Qt::Key_Escape:
    if (hub_->isMiniPlayerMode()) {
      if (player_->isPlaying() ||
          hub_->isSignalTokenMode() && hub_->isPlaying())
        hub_->setEmbeddedPlayerMode();
      else
        hub_->toggleMiniPlayerMode();
    } else
      hub_->setMiniPlayerMode();
    break;
  case Qt::Key_Return:  hub_->toggleFullScreenWindowMode(); break;
  //case Qt::Key_Backspace: stop(); break;
  case Qt::Key_Space:   playPause(); break;

  case Qt::Key_Up:      volumeUp(); break;
  case Qt::Key_Down:    volumeDown(); break;

  case Qt::Key_Left:
    if (event->modifiers() == Qt::NoModifier) backward();
    else backward30s();
    break;

  case Qt::Key_Right:
    if (event->modifiers() == Qt::NoModifier) forward();
    else forward25s();
    break;

#ifdef Q_OS_WIN
  case Qt::Key_CapsLock:
    menuBar()->setVisible(!menuBar()->isVisible());
    break;
#endif // Q_OS_WIN

/*
    // Modifiers
  case Qt::Key_Control:
    if (!isEditing()) {
      PlayerUi *ui = visiblePlayer();
      if (ui)
        ui->clearFocus();
      setFocus();
    }
#ifdef Q_OS_WIN
    updateAnnotationHoverGesture();
#endif // Q_OS_WIN
    Base::keyPressEvent(event);
    break;
  case Qt::Key_Shift:
    if (!isEditing()) {
      PlayerUi *ui = visiblePlayer();
      if (ui)
        ui->clearFocus();
      setFocus();
    }
#ifdef Q_OS_WIN
    updateAnnotationHoverGesture();
#endif // Q_OS_WIN
    Base::keyPressEvent(event);
    break;
#ifdef Q_OS_WIN
  case Qt::Key_Alt:
    {
      PlayerUi *ui = visiblePlayer();
      if (ui)
        ui->clearFocus();
      setFocus();
    }
#ifdef Q_OS_WIN
    updateAnnotationHoverGesture();
#endif // Q_OS_WIN
    Base::keyPressEvent(event);
    break;
#endif // Q_OS_WIN
*/

  case Qt::Key_Shift:
  case Qt::Key_A: case Qt::Key_B: case Qt::Key_C: case Qt::Key_D: case Qt::Key_E: case Qt::Key_F: case Qt::Key_G: case Qt::Key_H: case Qt::Key_I: case Qt::Key_J: case Qt::Key_K: case Qt::Key_L: case Qt::Key_M:
  case Qt::Key_N: case Qt::Key_O: case Qt::Key_P: case Qt::Key_Q: case Qt::Key_R: case Qt::Key_S: case Qt::Key_T: case Qt::Key_U: case Qt::Key_V: case Qt::Key_W: case Qt::Key_X: case Qt::Key_Y: case Qt::Key_Z:
  case Qt::Key_1: case Qt::Key_2: case Qt::Key_3: case Qt::Key_4: case Qt::Key_5: case Qt::Key_6: case Qt::Key_7: case Qt::Key_8: case Qt::Key_9: //case Qt::Key_0:
    if (hub_->isFullScreenWindowMode()) { // Osd mode
      if (!hasVisiblePlayer())
        embeddedPlayer_->show();
      else if (embeddedPlayer_->isVisible())
        embeddedPlayer_->resetAutoHideTimeout();
        //QTimer::singleShot(0, embeddedPlayer_, SLOT(resetAutoHideTimeout()));
    }
    if (hasVisiblePlayer()
        && !visiblePlayer()->inputComboBox()->hasFocus()
        && !visiblePlayer()->prefixComboBox()->hasFocus()) {
#ifdef Q_OS_WIN
      QtWin::setFocus(visiblePlayer()->inputComboBox());
#else
      visiblePlayer()->inputComboBox()->setFocus();
#endif // Q_OS_WIN
    }
  default:
    Base::keyPressEvent(event);
  }
}

void
MainWindow::maximizedToFullScreen()
{
  if (isMaximized()) {
    DOUT("showFullScreen");
    showFullScreen();
    QTimer::singleShot(0, hub_, SLOT(setFullScreenWindowMode()));
  }
}

// - Drag and drop events -

bool
MainWindow::isMimeDataSupported(const QMimeData *mime)
{ return mime && mime->hasUrls(); }

void
MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
  DOUT("enter");
  Q_ASSERT(event);
  if (isMimeDataSupported(event->mimeData()))
    event->acceptProposedAction();
  Base::dragEnterEvent(event);
  DOUT("exit");
}

void
MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
  //DOUT("enter");
  Q_ASSERT(event);
  //if (isMimeDataSupported(event->mimeData()))
    event->acceptProposedAction();
  Base::dragMoveEvent(event);
  //DOUT("exit");
}

void
MainWindow::dropEvent(QDropEvent *event)
{
  DOUT("enter");
  Q_ASSERT(event);
  if (isMimeDataSupported(event->mimeData())) {
    setFocus();
    event->acceptProposedAction();
    openMimeData(event->mimeData());
  }
  Base::dropEvent(event);
  DOUT("exit");
}

void
MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
  DOUT("enter");
  event->accept();
  Base::dragLeaveEvent(event);
  DOUT("exit");
}

// - Close -

void
MainWindow::dispose()
{
  DOUT("enter: size =" << size());

  if (annotationBrowser_ && annotationBrowser_->isVisible() && dataManager_->hasAnnotations())
    annotationBrowser_->hide(); // prevent hanging when deleting annotations

  saveSettingsTimer_->stop();

  enum { wait_player_stop_timeout = 2000 }; // 2 seconds
  int timeout = player_->isStopped() ? 0 : wait_player_stop_timeout;

  if (player_->hasMedia()) {
    //rememberAspectRatio();
    rememberPlayPos();
    rememberSubtitle();
    rememberAudioTrack();
    rememberAudioChannel();
  }
  DOUT("player dispose");
  player_->dispose();

  //connect(fadeAni_, SIGNAL(finished()), SLOT(hide()));
  //fadeAni_->fadeOut(windowOpacity());

  if (rippleTimer_ && rippleTimer_->isActive())
    rippleTimer_->stop();
  if (embeddedPlayer_->isVisible())
    embeddedPlayer_->hide();
  if (isVisible()) {
    foreach (QWidget *w, windows_)
      if (w->isVisible()) {
        DOUT("hide" << w);
        //if (auto p = qobject_cast<AcWindow *>(w))
        //  p->fadeOut();
        //else if (auto p = qobject_cast<AcMainWindow *>(w))
        //  p->fadeOut();
        //else
          w->hide();
      }

    fadeAni_->fadeOut(windowOpacity());
    qint64 duration = timeout = fadeAni_->duration();
    if (!timeout)
      timeout = duration;
    else if (timeout < duration)
      duration = timeout;
    QTimer::singleShot(duration, this, SLOT(hide()));
  } else {
    osdWindow_->hide();
    foreach (QWidget *w, windows_) {
      DOUT("hide" << w);
      w->hide();
    }
  }

  QTimer::singleShot(timeout, this, SLOT(close()));

  if (tray_)
    tray_->hide();

#ifdef WITH_WIN_TEXTHOOK
  TextHook::globalInstance()->clear();
#endif // WITH_WIN_TEXTHOOK

  hub_->stop();

  dataServer_->dispose();

#ifdef WITH_WIN_PICKER
  WindowPicker::globalInstance()->stop();
#endif // WITH_WIN_PICKER
#ifdef WITH_WIN_HOOK
  if (HOOK->isActive())
    HOOK->stop();
#endif // WITH_WIN_HOOK

  FlvCodec::globalInstance()->stop();
  DOUT("exit");
}

//void
//MainWindow::quit()
//{
//  fadeAni_->fadeOut(windowOpacity());
//  QTimer::singleShot(fadeAni_->duration(), this, SLOT(hide()));
//  QTimer::singleShot(fadeAni_->duration(), this, SLOT(close()));
//}

void
MainWindow::closeEvent(QCloseEvent *event)
{
  DOUT("enter: disposed =" << disposed_ << ", visible =" << isVisible());

  if (!disposed_) {
    disposed_ = true;
    if (event)
      event->ignore();

    dispose();
    DOUT("exit: close later");
    return;
  }

  DOUT("size =" << size());

  appServer_->stop();

  if (fadeAni_->state() == QAbstractAnimation::Running)
    fadeAni_->stop();

  // Save settings
  AcSettings::globalSettings()->dispose();
  saveSettings();

  //MediaStreamer::globalInstance()->stop();
  //{
  //  StreamService *ss = StreamService::globalInstance();
  //  ss->stop(10 * 1000); // 10 seconds
  //  if (ss->isActive())
  //    ss->terminateService();
  //}

  DownloaderController::globalController()->abort();

  //enum { ProcessEventsTimeout = 2000 };
  //DOUT("processEvents:enter: timeout =" << ProcessEventsTimeout);
  //qApp->processEvents(QEventLoop::AllEvents, ProcessEventsTimeout);
  //DOUT("processEvents:leave");

  if (QThreadPool::globalInstance()->activeThreadCount()) {
    // wait for at most 5 seconds ant kill all threads
    enum { CloseTimeout = 5000 };
    QThreadPool::globalInstance()->waitForDone(CloseTimeout);
  }

  //if (parentWidget())
  //  QTimer::singleShot(0, parentWidget(), SLOT(close()));

#ifdef Q_OS_WIN
  QTimer::singleShot(0, qApp, SLOT(quit())); // ensure quit app and clean up zombie threads
#endif // Q_OS_WIN

  Base::closeEvent(event);
  emit windowClosed();
  DOUT("exit");
}

// - Window on top -

bool
MainWindow::isWindowOnTop() const
{ return windowFlags() & Qt::WindowStaysOnTopHint; }

void
MainWindow::setWindowOnTop(bool t)
{
  if (t != isWindowOnTop()) {
//#ifdef WITH_WIN_DWM
//    AcUi::globalInstance()->setDwmEnabled(false);
//#endif // WITH_WIN_DWM
    bool visible = isVisible();
    setWindowFlags(t ? windowFlags() | Qt::WindowStaysOnTopHint :
                       windowFlags() & ~Qt::WindowStaysOnTopHint);
    osdWindow_->setWindowFlags(t ? osdWindow_->windowFlags() | Qt::WindowStaysOnTopHint :
                                   osdWindow_->windowFlags() & ~Qt::WindowStaysOnTopHint);
//#ifdef WITH_WIN_TEXTHOOK
//    TextHook::globalInstance()->setParentWinId(winId());
//#endif // WITH_WIN_TEXTHOOK

#ifdef WITH_WIN_DWM
    AcUi::globalInstance()->setDwmEnabled(true);
#endif // WITH_WIN_DWM
    if (visible) {
      if (hub_->isFullScreenWindowMode()) {
        DOUT("showFullScreen");
        showFullScreen();
      } else {
        DOUT("show");
        show();
      }
    }

//#ifdef Q_OS_MAC
//    if (t)
//      osdWindow_->ensureStaysOnTop();
//#endif // Q_OS_MAC

    if (t)
      emit message(tr("always on top enabled"));
    else
      emit message(tr("always on top disabled"));
  }

#ifdef Q_OS_WIN
  if (t) {
    if (!hub_->isMediaTokenMode() && annotationView_->trackedWindow() &&
        !QtWin::isWindowAboveWindow(winId(), annotationView_->trackedWindow()))
      QtWin::setTopWindow(winId());
    if (!windowStaysOnTopTimer_->isActive())
      windowStaysOnTopTimer_->start();
  } else {
    if (windowStaysOnTopTimer_->isActive())
      windowStaysOnTopTimer_->stop();
  }
#endif // Q_OS_WIN
}

// - Synchronize -

void
MainWindow::blessTokenWithId(qint64 id, bool async)
{
  DOUT("enter: async =" << async);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (!id) {
    emit warning(tr("invalid cast id"));
    DOUT("exit from invalid parameter branch");
    return;
  }

  if (!server_->isConnected() || !server_->isAuthorized()) {
    emit warning(tr("cannot perform cast when offline"));
    DOUT("exit from offline branch");
    return;
  }

  if (async) {
    emit message(tr("blessing media ..."));
    QtConcurrent::run(this, &Self::blessTokenWithId, id, false);
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  bool ok = server_->blessTokenWithId(id);
  if (ok) {
    emit message(tr("media blessed"));
    if (dataManager_->token().id() == id) {
      dataManager_->token().blessCount()++;
      //dataManager_->invalidateToken();
    }
  } else
    emit warning(tr("failed to bless media"));

  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");
  DOUT("exit: async =" << async);
}

void
MainWindow::curseTokenWithId(qint64 id, bool async)
{
  DOUT("enter: async =" << async);
  if (!id) {
    emit warning(tr("invalid cast id"));
    DOUT("exit from invalid parameter branch");
    return;
  }

  if (!server_->isConnected() || !server_->isAuthorized()) {
    emit warning(tr("cannot perform cast when offline"));
    DOUT("exit from offline branch");
    return;
  }

  if (async) {
    emit message(tr("cursing media ..."));
    QtConcurrent::run(this, &Self::curseTokenWithId, id, false);
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  bool ok = server_->curseTokenWithId(id);
  if (ok) {
    emit message(tr("media cursed"));
    if (dataManager_->token().id() == id) {
      dataManager_->token().curseCount()++;
      //dataManager_->invalidateToken();
    }
  } else
    emit warning(tr("failed to curse media"));

  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");
  DOUT("exit: async =" << async);
}

void
MainWindow::blessUserWithId(qint64 id, bool async)
{
  DOUT("enter: async =" << async);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (!id) {
    emit warning(tr("invalid cast id"));
    DOUT("exit from invalid parameter branch");
    return;
  }

  if (!server_->isConnected() || !server_->isAuthorized()) {
    emit warning(tr("cannot perform cast when offline"));
    DOUT("exit from offline branch");
    return;
  }
  if (id == server_->user().id()) {
    emit warning(tr("cannot perform cast to yourself"));
    DOUT("exit from self branch");
    return;
  }

  if (async) {
    emit message(tr("blessing user ..."));
    QtConcurrent::run(this, &Self::blessUserWithId, id, false);
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  server_->blessUserWithId(id);
  emit message(tr("user blessed"));

  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");
  DOUT("exit: async =" << async);
}

void
MainWindow::curseUserWithId(qint64 id, bool async)
{
  DOUT("enter: async =" << async);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (!id) {
    emit warning(tr("invalid cast id"));
    DOUT("exit from invalid parameter branch");
    return;
  }

  if (!server_->isConnected() || !server_->isAuthorized()) {
    emit warning(tr("cannot perform cast when offline"));
    DOUT("exit from offline branch");
    return;
  }
  if (id == server_->user().id()) {
    emit warning(tr("cannot perform cast to yourself"));
    DOUT("exit from self branch");
    return;
  }

  if (async) {
    emit message(tr("cursing user ..."));
    QtConcurrent::run(this, &Self::curseUserWithId, id, false);
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  server_->curseUserWithId(id);
  emit message(tr("user cursed"));

  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");
  DOUT("exit: async =" << async);
}

void
MainWindow::blockUserWithId(qint64 id, bool async)
{
  DOUT("enter: async =" << async);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (!id) {
    emit warning(tr("invalid cast id"));
    DOUT("exit from invalid parameter branch");
    return;
  }

  if (!server_->isConnected() || !server_->isAuthorized()) {
    emit warning(tr("cannot perform cast when offline"));
    DOUT("exit from offline branch");
    return;
  }
  if (id == server_->user().id()) {
    emit warning(tr("cannot perform cast to yourself"));
    DOUT("exit from self branch");
    return;
  }

  if (async) {
    emit message(tr("blocking user ..."));
    QtConcurrent::run(this, &Self::blockUserWithId, id, false);
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  server_->blockUserWithId(id);
  emit message(tr("user blocked"));

  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");
  DOUT("exit: async =" << async);
}

#define CAST(_cast, _annot) \
  void \
  MainWindow::_cast##_annot##WithId(qint64 id, bool async) \
  { \
    DOUT("enter: async =" << async); \
    if (disposed_) { \
      DOUT("exit: returned from disposed branch"); \
      return; \
    } \
    if (!id) { \
      emit warning(tr("invalid cast id")); \
      DOUT("exit from invalid parameter branch"); \
      return; \
    } \
\
    if (!server_->isConnected() || !server_->isAuthorized()) { \
      emit warning(tr("cannot perform cast when offline")); \
      DOUT("exit from offline branch"); \
      return; \
    } \
\
    if (async) { \
      emit message(tr("casting spell to target ...")); \
      QtConcurrent::run(this, &Self::_cast##_annot##WithId, id, false); \
      DOUT("exit: returned from async branch"); \
      return; \
    } \
\
    DOUT("inetMutex locking"); \
    inetMutex_.lock(); \
    DOUT("inetMutex locked"); \
\
    server_->_cast##_annot##WithId(id); \
\
    DOUT("inetMutex unlocking"); \
    inetMutex_.unlock(); \
    DOUT("inetMutex unlocked"); \
    DOUT("exit: async =" << async); \
  }

  CAST(bless, Annotation)
  CAST(block, Annotation)
  CAST(curse, Annotation)
  CAST(bless, Alias)
  CAST(block, Alias)
  CAST(curse, Alias)
#undef CAST

void
MainWindow::logout(bool async)
{
  DOUT("enter: async =" << async);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  Q_UNUSED(async)
  //if (async) {
  //  emit message(tr("connecting server to logout ..."));
  //  QtConcurrent::run(this, &Self::logout, false);
  //  DOUT("exit: returned from async branch");
  //  return;
  //}

  //DOUT("inetMutex locking");
  //inetMutex_.lock();
  //DOUT("inetMutex locked");

  emit userIdChanged(0);
  server_->logout();

  //DOUT("inetMutex unlocking");
  //inetMutex_.unlock();
  //DOUT("inetMutex unlocked");

  emit userChanged(User());

  //s->setUserName(QString());
  AcSettings::globalSettings()->setPassword(QString());

  showLogin();
  DOUT("exit: async =" << async);
}

void
MainWindow::checkInternetConnection(bool async)
{
  DOUT("enter: async =" << async << "connect =" << server_->isConnected());
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (async) {
    emit message(tr("connecting to server ..."));
    QtConcurrent::run(this, &Self::checkInternetConnection, false);
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  server_->updateConnected();

  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");

  if (server_->isConnected())
    emit message(tr("server connected"));
  else
    emit message(tr("server disconnected"));

  DOUT("exit");
}

void
MainWindow::deleteCaches()
{
  // TODO
}

void
MainWindow::login(const QString &userName, const QString &encryptedPassword, bool async)
{
  DOUT("enter: async =" << async << ", userName =" << userName);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (async) {
    QtConcurrent::run(this, &Self::login, userName, encryptedPassword, false);
    DOUT("exit: returned from async branch");
    return;
  }

  bool updated = true;

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  AcSettings *settings = AcSettings::globalSettings();

  server_->updateConnected();
  if (disposed_) {
    DOUT("inetMutex unlocking"); inetMutex_.unlock(); DOUT("inetMutex unlocked");
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (!server_->isConnected() && cache_->isValid()) {
    // login from cache
    emit message(TR(T_MESSAGE_TRY_LOGINFROMCACHE));
    User user = cache_->selectUserWithNameAndPassword(userName, encryptedPassword);
    if (user.isValid()) {
      if (!Traits::isKnownLanguage(user.language()))
        user.setLanguage(Traits::English);
      settings->setUserName(userName);
      settings->setPassword(encryptedPassword);

      server_->setAuthorized(true);
      server_->setUser(user);
      emit userIdChanged(user.id());
      emit message(TR(T_SUCCEED_LOGINFROMCACHE));
    } else
     emit warning(TR(T_ERROR_LOGINFROMCACHE_FAILURE));

    DOUT("inetMutex unlocking");
    inetMutex_.unlock();
    DOUT("inetMutex unlocked");
    DOUT("exit from offline branch");
    return;
  }

  settings->setUserName(userName);
  settings->setPassword(encryptedPassword);

  server_->login(userName, encryptedPassword);
  if (disposed_) {
    DOUT("inetMutex unlocking"); inetMutex_.unlock(); DOUT("inetMutex unlocked");
    DOUT("exit: returned from disposed branch");
    return;
  }

  bool online = server_->isConnected() && server_->isAuthorized();
  if (online) {
    //liveInterval_ = server_->selectLiveTokenInterval();
    if (disposed_) {
      DOUT("inetMutex unlocking"); inetMutex_.unlock(); DOUT("inetMutex unlocked");
      DOUT("exit: returned from disposed branch");
      return;
    }
    //if (liveInterval_ <= 0)
    //  liveInterval_ = DEFAULT_LIVE_INTERVAL;

    //if (!server_->user().hasLanguage())
    //  server_->setUserLanguage(Traits::UnknownLanguage);

    qint64 uid = server_->user().id();
    settings->setUserId(uid);
    //tokenView_->setUserId(uid);
    annotationView_->setUserId(uid);
    emit userIdChanged(uid);

    if (cache_->isValid())
      cache_->updateUser(server_->user());

    // Languages
    //switch (server_->user().language()) {
    //case Traits::Japanese:
    //  toggleAnnotationLanguageToJapaneseAct_->setChecked(true);
    //  break;
    //case Traits::Chinese:
    //  toggleAnnotationLanguageToChineseAct_->setChecked(true);
    //  break;
    //case Traits::Korean:
    //  toggleAnnotationLanguageToKoreanAct_->setChecked(true);
    //  break;
    //case Traits::UnknownLanguage:
    //  toggleAnnotationLanguageToUnknownAct_->setChecked(true);
    //  break;
    //case Traits::English:
    //case Traits::AnyLanguage:
    //default:
    //  toggleAnnotationLanguageToEnglishAct_->setChecked(true);
    //}
    //invalidateAnnotationLanguages();

    QDate today = QDate::currentDate();
    if (Settings::globalSettings()->updateDate() != today) {
      updated = server_->isSoftwareUpdated();
      Settings::globalSettings()->setUpdateDate(today);
    }

    if (disposed_) {
      DOUT("inetMutex unlocking"); inetMutex_.unlock(); DOUT("inetMutex unlocked");
      DOUT("exit: returned from disposed branch");
      return;
    }
    dataServer_->commitQueue();
  }

  settings->sync();

  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");

  emit userChanged(server_->user());

  if (!disposed_ && online &&
      Settings::globalSettings()->isLive() && !hub_->isSignalTokenMode() && !player_->hasMedia())
    QTimer::singleShot(0, hub_, SLOT(setLiveTokenMode()));

  if (!updated) {
    emit notification(tr("new version released at Google Code"));
#ifdef AC_ENABLE_UPDATE
    emit message(tr("updater launched, please close Annot Player"));
    updaterDelegate_->open();
#endif // AC_ENABLE_UPDATE
  }

  DOUT("exit");
}

// - Sync mode -

/*
void
MainWindow::setSyncMode(bool t)
{
  bool isSyncMode = hub_->playerMode() == SignalHub::SyncPlayMode;
  if (isSyncMode != t) {
    if (t)
      hub_->setPlayerMode(SignalHub::SyncPlayMode);
    else
      hub_->setPlayerMode(SignalHub::NormalPlayMode);
    emit syncModeChanged(t);

    if (t)
      emit message(tr("Sync mode started"));
    else
      emit message(tr("Sync mode stopped"));
  }
}
*/

// - Live mode -

/*
void
MainWindow::setLiveMode(bool t)
{
  bool isLiveMode = hub_->playerMode() == SignalHub::LivePlayMode;
  if (isLiveMode != t) {
    if (t)
      hub_->setPlayerMode(SignalHub::LivePlayMode);
    else
      hub_->setPlayerMode(SignalHub::NormalPlayMode);
    emit liveModeChanged(t);

    if (t)
      emit message(tr("Live mode started"));
    else
      emit message(tr("Live mode stopped"));
  }
}
*/

#ifdef WITH_WIN_TEXTHOOK

// - Signal mode -

void
MainWindow::openProcessWindow(WId hwnd)
{
  ulong pid = QtWin::getWindowProcessId(hwnd);
  if (pid) {
    emit message(tr("found process id for window") + QString(" (pid = %1)").arg(QString::number(pid)));
    openProcessId(pid);
  } else if (pid == QCoreApplication::applicationPid())
    emit warning(tr("cannot sync with myself"));
  else {
    emit warning(tr("process id for window was not found"));
    openProcess();
  }
}

void
MainWindow::openProcessId(ulong pid)
{
  if (!pid) {
    openProcess();
    return;
  }
  if (pid == QCoreApplication::applicationPid()) {
    emit warning(tr("cannot sync with myself"));
    return;
  }

  if (TextHook::globalInstance()->containsProcess(pid))
    emit message(tr("process was attached") + QString(" (pid = %1)").arg(pid));
  else {
    bool ok = TextHook::globalInstance()->attachOneProcess(pid);
    if (ok) {
      emit message(tr("process attached") + QString(" (pid = %1)").arg(QString::number(pid)));
      syncView_->processView()->refresh();
      ProcessInfo pi = syncView_->processView()->attachedProcessInfo();
      if (pi.isValid() && pid == pi.processId)
        emit attached(pi);
      if (!annotationView_->trackedWindow() ||
          pi.processId != QtWin::getWindowProcessId(annotationView_->trackedWindow())) {
        WId hwnd = QtWin::getGoodWindowWithProcessId(pi.processId);
        if (hwnd)
          emit windowPicked(hwnd);
      }
    } else {
      errorMessage(tr("failed to attach process ") + QString(" (pid = %1)").arg(QString::number(pid)));
      if (!QtWin::isProcessActiveWithId(pid))
        emit notification(tr("Is the process running now?"));
      else if (!QtWinnt::isElevated())
        emit notification(tr("Run me as administrator and try again (o^^o)"));
      else
        emit notification(tr("Restart the target process might help -_-"));
      openProcess();
      return;
    }
  }
}

void
MainWindow::openProcessPath(const QString &path)
{
  QString processName = QFileInfo(path).fileName();
  ulong pid = QtWinnt::getProcessIdByName(processName);
  if (pid)
    emit message(tr("process was started") + QString(" (pid = %1)").arg(pid));
  else if (pid == QCoreApplication::applicationPid())
    emit warning(tr("cannot sync with myself"));
  else {
    if (isAppLocaleEnabled())
      AppLocale::createProcess(path, LCID_JA_JP);
    else
      QtWin::createProcess(path);
    emit message(tr("told process to start") + QString(" (name = %1)").arg(processName));
    pid = QtWinnt::getProcessIdByName(processName);
  }

  addRecent(path);

  syncView_->processView()->refresh();
  if (!pid) {
    emit warning(tr("failed to start process") + QString(" (name = %1)").arg(processName));
    openProcess();
  } else {
    enum { seconds = G_OPENPROCESS_TIMEOUT / 1000 }; // msecs / 1000
    emit message(tr("wait %1 seconds for process to start ...").arg(QString::number(seconds)));
    QTimer::singleShot(
      G_OPENPROCESS_TIMEOUT,
      new detail::OpenProcessId(pid, this),
      SLOT(trigger())
    );
  }
}

void
MainWindow::synchronizeProcess(const ProcessInfo &pi, bool async)
{
  DOUT("enter: async =" << async << ", pid =" << pi.processId << ", exe =" << pi.executablePath);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }

  if (!pi.isValid() || pi.executablePath.isEmpty()) {
    emit openProcessRequested();
    DOUT("exit: empty process info");
    return;
  }

  if (gameLibrary_->isEmpty() && !server_->isConnected()) {
    emit openProcessRequested();
    DOUT("exit: offline with empty library");
    return;
  }

  if (async) {
    emit messageOnce(tr("Analyzing ..."));
    QtConcurrent::run(this, &Self::synchronizeProcess, pi, false);
    DOUT("exit: returned from async branch");
    return;
  }

  QString digest = Token::digestFromFile(pi.executablePath);
  if (digest.isEmpty()) {
    emit openProcessRequested();
    DOUT("exit: cannot compute digest for file:" << pi.executablePath);
    return;
  }

  Game g;
  if (!gameLibrary_->isEmpty())
    g = gameLibrary_->findGameByDigest(digest);

  if (!(g.hasDigest() && g.hasHook()) && server_->isConnected()) {
    DOUT("inetMutex locking");
    inetMutex_.lock();
    DOUT("inetMutex locked");

    dataManager_->gameHook() = server_->selectGameHookWithTokenDigest(digest);
    if (!g.hasHook() && dataManager_->hasGameHook())
      g.setHook(dataManager_->gameHook().text());

    if (!g.hasDigest()) {
      dataManager_->gameThread() = server_->selectGameThreadWithTokenDigest(digest);
      DOUT("thread id =" << dataManager_->gameThread().id());

      if (dataManager_->hasGameThread()) {
        const GameThread &t = dataManager_->gameThread();
        g.setTokenId(t.tokenId());
        g.setDigest(digest);
        g.setLocation(QDir::fromNativeSeparators(pi.executablePath));
        g.setEncoding(Traits::codePageToEncoding(t.encoding()));
        g.setVisitTime(QDateTime::currentMSecsSinceEpoch());
        g.setVisitCount(1);

        TextThread tt;
        tt.setProvider(t.provider());
        tt.setRole(t.type());
        tt.setSignature(t.signature());
        g.threads().append(tt);
      }
    }

    DOUT("inetMutex unlocking");
    inetMutex_.unlock();
    DOUT("inetMutex unlocked");
  }

  if (g.hasHook() && !TextHook::globalInstance()->containsHook(pi.processId, g.hook())) {
    bool ok = TextHook::globalInstance()->addHook(pi.processId, g.hook());
    if (ok)
      emit message(tr("add text hook") + ": " + g.hook());
    else
      emit warn(tr("failed to add text hook") + ": " + g.hook());
  }

  if (!g.hasDigest()) {
    emit openProcessRequested();
    DOUT("exit: unknown name");
    return;
  }

  // Detect encoding
  QString encoding = g.encoding();
  if (!encoding.isEmpty())
    emit message(
      tr("resuming last game encoding") + ": "
      HTML_SS_OPEN(color:orange)
      + encoding +
      HTML_SS_CLOSE()
    );
  else {
    encoding = TextHook::guessEncodingForFile(pi.executablePath);
    if (!encoding.isEmpty())
      emit message(
        tr("detect game encoding") + ": "
        HTML_SS_OPEN(color:orange)
        + encoding +
        HTML_SS_CLOSE()
      );
  }

  if (!encoding.isEmpty())
    TextCodecManager::globalInstance()->setEncoding(encoding);

  // Detect encoding
  if (!g.hasThreads()) {
    emit openProcessRequested();
    DOUT("exit: no threads found in the game library");
  }

  emit hideSyncViewRequested();
  emit subscribeThreadsRequested(g.threads(), pi);
  DOUT("exit");
}

void
MainWindow::updateSubscription(const TextThreadList &threads, bool async)
{
  DOUT("enter: async =" << async << ", thread size =" << threads.size());
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (!hub_->isSignalTokenMode()) {
    emit warning(tr("please synchronize with a game first"));
    DOUT("exit: not in game mode");
    return;
  }
  if (threads.isEmpty()) {
    DOUT("exit: empty threads");
    return;
  }

  if (!dataManager_->token().hasDigest()) {
    DOUT("exit: empty digest");
    return;
  }

  if (!server_->isConnected() || !server_->isAuthorized())
    async = false;
  if (async) {
    emit messageOnce(tr("Saving ..."));
    QtConcurrent::run(this, &Self::updateSubscription, threads, false);
    DOUT("exit: returned from async branch");
    return;
  }

  QString digest = dataManager_->token().digest();
  if (digest.isEmpty()) {
    DOUT("exit: empty digest");
    return;
  }

  Game g = gameLibrary_->findGameByDigest(digest);
  if (!g.hasDigest()) {
    DOUT("exit: no game found in the library");
    return;
  }

  g.setHook(TextHook::globalInstance()->currentHook());
  g.setThreads(threads);
  g.setEncoding(TextCodecManager::globalInstance()->encoding());

  gameLibrary_->update(g);

  if (server_->isConnected() && server_->isAuthorized()
      && g.hasTokenId()) {
    DOUT("inetMutex locking");
    inetMutex_.lock();
    DOUT("inetMutex locked");

    if (g.hasHook()) {
      GameHook h = g.tokenId() == dataManager_->gameHook().tokenId() ?
        dataManager_->gameHook() :
        server_->selectGameHookWithTokenId(g.tokenId());
      if (h.isValid()) {
        if (g.hasHook() && g.hook() != h.text())
          server_->updateGameHookTextWithId(g.hook(), h.id());
      } else if (g.hasHook()) {
        h.setUserId(server_->user().id());
        h.setTokenId(g.tokenId());
        h.setUpdateTime(QDateTime::currentMSecsSinceEpoch() / 1000);
        h.setText(g.hook());
        qint64 id = server_->submitGameHook(h);
        h.setId(id);
      }
      dataManager_->setGameHook(h);
    }

    GameThread t = g.tokenId() == dataManager_->gameThread().tokenId() ?
      dataManager_->gameThread() :
      server_->selectGameThreadWithTokenId(g.tokenId());

    TextThread tt;
    foreach (const TextThread &i, g.threads())
      if (i.isLeadingRole())  {
        tt = i;
        break;
      }

    if (!t.isValid()) {
      if (tt.hasRole()) {
          t.setUpdateTime(QDateTime::currentMSecsSinceEpoch() / 1000);
          t.setUserId(server_->user().id());
          t.setTokenId(g.tokenId());
          t.setEncoding(Traits::codePageFromEncoding(g.encoding()));
          t.setSignature(tt.signature());
          t.setProvider(tt.provider());
          t.setType(tt.role());
          qint64 id = server_->submitGameThread(t);
          t.setId(id);
        }
    } else if (tt.hasRole() && (
               tt.hasSignature() && tt.signature() != t.signature() ||
               g.hasEncoding() && t.encoding() != Traits::codePageFromEncoding(g.encoding()) ||
               tt.hasProvider() && tt.provider() != t.provider())
      ) {
      t.setUpdateTime(QDateTime::currentMSecsSinceEpoch() / 1000);
      t.setUserId(server_->user().id());
      t.setTokenId(g.tokenId());
      t.setEncoding(Traits::codePageFromEncoding(g.encoding()));
      t.setSignature(tt.signature());
      t.setProvider(tt.provider());
      t.setType(tt.role());
      server_->updateGameThread(t);
    }

    dataManager_->setGameThread(t);

    DOUT("inetMutex unlocking");
    inetMutex_.unlock();
    DOUT("inetMutex unlocked");
  }
  DOUT("exit");
}

void
MainWindow::subscribeThreads(const TextThreadList &threads, const ProcessInfo &pi)
{
  DOUT("enter: threads count =" << threads.size() << ", pid =" << pi.processId << ", proc =" << pi.processName);
  if (player_->hasMedia() && !player_->isStopped())
    stop();

  bool updateProcess = TextHook::globalInstance()->isEmpty() ||
      pi.processId != messageHandler_->processInfo().processId;

  if (pi.isValid()) {
    backlogDialog();

    if (updateProcess) {
      showMessage(tr("opening process") + ": " + pi.processName);

      //rememberGameEncoding(pi);

      if (grabber_)
        grabber_->setBaseName(pi.processName);

      QString title = pi.processName;
      setWindowTitle(title);
      miniPlayer_->setWindowTitle(title); // TODO: move to invalidateTitle!!! or hub_();

      addRecent(pi.executablePath);
      setRecentOpenedFile(pi.executablePath);
    }

    messageHandler_->clearMessages();

    messageHandler_->setProcessInfo(pi);
    messageHandler_->setThreads(threads);

    if (!annotationView_->trackedWindow() ||
        pi.processId != QtWin::getWindowProcessId(annotationView_->trackedWindow())) {
      WId hwnd = QtWin::getGoodWindowWithProcessId(pi.processId);
      if (hwnd && hwnd != annotationView_->trackedWindow()) {
        DOUT("track new window:" << hwnd);
        emit windowPicked(hwnd);
      }
    }
  }

  hub_->setSignalTokenMode();

  QString mrl;

  if (!updateProcess)
    updateSubscription(threads);
  else if (pi.isValid()) {
    mrl = pi.executablePath;

    WId hwnd = annotationView_->trackedWindow();
    if (hwnd && QtWin::getWindowProcessId(hwnd) == pi.processId) {
      QString title = QtWin::getWindowText(hwnd).trimmed();
      if (!title.isEmpty()) {
        title = TextCodecManager::globalInstance()->transcode(title);
        if (recentUrlTitles_.size() > RECENT_COUNT)
          recentUrlTitles_.erase(recentUrlTitles_.begin());
        recentUrlTitles_[mrl] = title;
      }
    }
  }
  if (!mrl.isEmpty()) {
    recentDigest_.clear();
    recentUrl_.clear();
    invalidateToken(mrl);
  }

  if (!hub_->isPlaying())
    hub_->play();

  saveSettingsLater();
  DOUT("exit");
}

//void
//MainWindow::rememberGameEncoding(const ProcessInfo &pi)
//{
//  QString file = pi.executablePath;
//  if (file.isEmpty())
//    return;
//  file = QFileInfo(file).fileName();
//  if (file.isEmpty())
//    return;
//  QString e = TextCodecManager::globalInstance()->encoding();
//  if (e == TEXT_CODEC_JAPANESE)
//    recentGameEncodings_.remove(file);
//  else
//    recentGameEncodings_[file] = e;
//}
//
//void
//MainWindow::resumeGameEncoding(const ProcessInfo &pi)
//{
//  QString file = pi.executablePath;
//  if (file.isEmpty())
//    return;
//  file = QFileInfo(file).fileName();
//  if (file.isEmpty())
//    return;
//  auto p = recentGameEncodings_.find(file);
//  if (p != recentGameEncodings_.end()) {
//    QString e = p.value();
//    emit message(
//      tr("resuming last game encoding") + ": "
//      HTML_SS_OPEN(color:orange)
//      + e +
//      HTML_SS_CLOSE()
//    );
//    TextCodecManager::globalInstance()->setEncoding(e);
//  } else {
//    QString e = TextHook::guessEncodingForFile(file);
//    if (!e.isEmpty()) {
//      emit message(
//        tr("detect game encoding") + ": "
//        HTML_SS_OPEN(color:orange)
//        + e +
//        HTML_SS_CLOSE()
//      );
//      TextCodecManager::globalInstance()->setEncoding(e);
//    }
//  }
//}

#endif // WITH_WIN_TEXTHOOK

#ifdef AC_ENABLE_GAME

void
MainWindow::showSyncGame(bool showProcess)
{
  //if (visible) {
  //  if (hub_->isMediaTokenMode() && hub_->isNormalPlayerMode() &&
  //      !player_->isPlaying() && !player_->isPaused())
  //    hide();
  //} else {
  //  if (hub_->isMediaTokenMode() && !isVisible())
  //    show();
  //}
  //syncView_->tokenView()->setToken(tokenView_->token());
  //syncView_->tokenView()->setAliases(tokenView_->aliases());
  //syncView_->tokenView()->setUserId(tokenView_->userId());
  syncView_->show();
  syncView_->setProcessViewVisible(showProcess);
  osdWindow_->setWindowOnTop();
  syncView_->raise();
}

void
MainWindow::openCurrentGame()
{
  qint64 pid = processFilter_->currentGamePid();
  if (pid) {
    showMessage(tr("synchronizing with running game") +
                QString(" (pid = %1)").arg(QString::number(pid,16)));
    showMessageOnce(tr("Synchronizing ..."));
    openProcessId(pid);
  } else
    warn(tr("no running games in the library are detected"));
}

void
MainWindow::showGamePreferences()
{
  showSyncGame(false); // false: hide process

  //if (visible) {
  //  recentThreadView_->clear();
//
  //  QList<QByteArray> l;
  //  foreach (const QByteArray &a, messageHandler_->recentMessages())
  //    l.prepend(a);
  //  if (!l.isEmpty()) {
  //    recentThreadView_->addMessages(l, messageHandler_->signature(), messageHandler_->function());
  //    recentThreadView_->setCurrentIndex(1);
  //  }
  //}
  //recentThreadView_->setVisible(visible);
  //if (visible)
  //  recentThreadView_->raise();
}

#endif // AC_ENABLE_GAME

void
MainWindow::setWindowTrackingEnabled(bool t)
{
#ifdef WITH_WIN_PICKER
  if (lastTrackedWindow_ && (
      lastTrackedWindow_ == annotationView_->trackedWindow() ||
      !QtWin::windowHasText(lastTrackedWindow_)))
    lastTrackedWindow_ = 0;

  if (!annotationView_->trackedWindow() && !lastTrackedWindow_) {
    showWindowPicker();
    return;
  }

  if (t) {
    if (!annotationView_->trackedWindow())
      annotationView_->setTrackedWindow(lastTrackedWindow_);
  } else if (annotationView_->trackedWindow()) {
    lastTrackedWindow_ = annotationView_->trackedWindow();
    annotationView_->setTrackedWindow(0);
  }

  if (annotationView_->trackedWindow())
    hub_->setEmbeddedPlayerMode();
  else if (hub_->isEmbeddedPlayerMode() && !hub_->isFullScreenWindowMode())
    hub_->setNormalPlayerMode();
#else
  Q_UNUSED(t)
#endif // WITH_WIN_PICKER
}

// - User -

void
MainWindow::setUserAnonymous(bool t, bool async)
{
  DOUT("enter: async =" << async);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }

  if (server_->isConnected() && server_->isAuthorized()) {

    if (async) {
      emit message(tr("updating anonymous status ..."));
      QtConcurrent::run(this, &Self::setUserAnonymous, t, false);
      DOUT("exit: returned from async branch");
      return;
    }

    DOUT("inetMutex locking");
    inetMutex_.lock();
    DOUT("inetMutex locked");

    bool ok = server_->setUserAnonymous(t);
    if (!ok)
      emit warning(tr("failed to change user anonymous state"));

    DOUT("inetMutex unlocking");
    inetMutex_.unlock();
    DOUT("inetMutex unlocked");

  } else
    server_->user().setAnonymous(t);

  if (server_->user().isAnonymous())
    emit message(tr("you are anonymous now"));
  else
    emit message(tr("you are not anonymous now"));

  DOUT("exit");
}

void
MainWindow::setUserLanguage(int lang, bool async)
{
  DOUT("enter: async =" << async);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }

  if (server_->isConnected() && server_->isAuthorized()) {

    if (async) {
      emit message(tr("updating user language ..."));
      QtConcurrent::run(this, &Self::setUserLanguage, lang, false);
      DOUT("exit: returned from async branch");
      return;
    }

    DOUT("inetMutex locking");
    inetMutex_.lock();
    DOUT("inetMutex locked");

    bool ok = server_->setUserLanguage(lang);
    if (!ok)
      emit warning(tr("failed to change user language"));

    DOUT("inetMutex unlocking");
    inetMutex_.unlock();
    DOUT("inetMutex unlocked");

  } else
    server_->user().setLanguage(lang);

  emit message(tr("your language is ") + languageToString(server_->user().language()));

  DOUT("exit");
}

void MainWindow::setUserLanguageToEnglish()     { setUserLanguage(Traits::English); }
void MainWindow::setUserLanguageToJapanese()    { setUserLanguage(Traits::Japanese); }
void MainWindow::setUserLanguageToTraditionalChinese()     { setUserLanguage(Traits::TraditionalChinese); }
void MainWindow::setUserLanguageToSimplifiedChinese()     { setUserLanguage(Traits::SimplifiedChinese); }
void MainWindow::setUserLanguageToKorean()      { setUserLanguage(Traits::Korean); }
void MainWindow::setUserLanguageToFrench()      { setUserLanguage(Traits::French); }
void MainWindow::setUserLanguageToGerman()      { setUserLanguage(Traits::German); }
void MainWindow::setUserLanguageToItalian()     { setUserLanguage(Traits::Italian); }
void MainWindow::setUserLanguageToSpanish()     { setUserLanguage(Traits::Spanish); }
void MainWindow::setUserLanguageToPortuguese()  { setUserLanguage(Traits::Portuguese); }
void MainWindow::setUserLanguageToRussian()     { setUserLanguage(Traits::Russian); }
//void MainWindow::setUserLanguageToUnknown()     { setUserLanguage(Traits::UnknownLanguage); }

void
MainWindow::invalidateAnnotationLanguages()
{
  QSet<int> languages;
  if (toggleAnnotationLanguageToAnyAct_->isChecked()) {
    toggleAnnotationLanguageToEnglishAct_->setEnabled(false);
    toggleAnnotationLanguageToJapaneseAct_->setEnabled(false);
    toggleAnnotationLanguageToChineseAct_->setEnabled(false);
    toggleAnnotationLanguageToKoreanAct_->setEnabled(false);
    toggleAnnotationLanguageToFrenchAct_->setEnabled(false);
    toggleAnnotationLanguageToGermanAct_->setEnabled(false);
    toggleAnnotationLanguageToItalianAct_->setEnabled(false);
    toggleAnnotationLanguageToSpanishAct_->setEnabled(false);
    toggleAnnotationLanguageToPortugueseAct_->setEnabled(false);
    toggleAnnotationLanguageToRussianAct_->setEnabled(false);
    //toggleAnnotationLanguageToUnknownAct_->setEnabled(false);

  } else {
    toggleAnnotationLanguageToEnglishAct_->setEnabled(true);
    toggleAnnotationLanguageToJapaneseAct_->setEnabled(true);
    toggleAnnotationLanguageToChineseAct_->setEnabled(true);
    toggleAnnotationLanguageToKoreanAct_->setEnabled(true);
    toggleAnnotationLanguageToFrenchAct_->setEnabled(true);
    toggleAnnotationLanguageToGermanAct_->setEnabled(true);
    toggleAnnotationLanguageToItalianAct_->setEnabled(true);
    toggleAnnotationLanguageToSpanishAct_->setEnabled(true);
    toggleAnnotationLanguageToPortugueseAct_->setEnabled(true);
    toggleAnnotationLanguageToRussianAct_->setEnabled(true);
    //toggleAnnotationLanguageToUnknownAct_->setEnabled(true);

    //if (toggleAnnotationLanguageToUnknownAct_->isChecked())     languages |= Traits::UnknownLanguageBit;
    if (toggleAnnotationLanguageToEnglishAct_->isChecked())     languages.insert(Traits::English);
    if (toggleAnnotationLanguageToJapaneseAct_->isChecked())    languages.insert(Traits::Japanese);
    if (toggleAnnotationLanguageToChineseAct_->isChecked())     { languages.insert(Traits::SimplifiedChinese); languages.insert(Traits::TraditionalChinese); }
    if (toggleAnnotationLanguageToKoreanAct_->isChecked())      languages.insert(Traits::Korean);
    if (toggleAnnotationLanguageToFrenchAct_->isChecked())      languages.insert(Traits::French);
    if (toggleAnnotationLanguageToGermanAct_->isChecked())      languages.insert(Traits::German);
    if (toggleAnnotationLanguageToItalianAct_->isChecked())     languages.insert(Traits::Italian);
    if (toggleAnnotationLanguageToSpanishAct_->isChecked())     languages.insert(Traits::Spanish);
    if (toggleAnnotationLanguageToPortugueseAct_->isChecked())  languages.insert(Traits::Portuguese);
    if (toggleAnnotationLanguageToRussianAct_->isChecked())     languages.insert(Traits::Russian);
  }

  // Use at least unknown language
  //if (!languages) {
  //  toggleAnnotationLanguageToUnknownAct_->setChecked(true);
  //  languages = Traits::AnyLanguageBit;
  //}

  annotationFilter_->setLanguages(languages);
}

// - Language -

void
MainWindow::setAppLanguageToEnglish()
{ AcSettings::globalSettings()->setLanguage(Traits::English); invalidateAppLanguage(); }

void
MainWindow::setAppLanguageToJapanese()
{ AcSettings::globalSettings()->setLanguage(Traits::Japanese); invalidateAppLanguage(); }

// TODO: Simplified Chinese and Traditional Chinese are not differed until qt 4.8
void
MainWindow::setAppLanguageToTraditionalChinese()
{ AcSettings::globalSettings()->setLanguage(Traits::TraditionalChinese); invalidateAppLanguage(); }

void
MainWindow::setAppLanguageToSimplifiedChinese()
{ AcSettings::globalSettings()->setLanguage(Traits::SimplifiedChinese); invalidateAppLanguage(); }

void
MainWindow::invalidateAppLanguage()
{
  Settings::globalSettings()->setApplicationFilePath(QString());
  Settings::globalSettings()->sync();
  AcSettings::globalSettings()->sync();
  emit notification(tr("restart the app to use the new language"));
}

// - Helpers -

QString
MainWindow::languageToString(int lang)
{
  switch(lang) {
  //case Traits::AnyLanguage:     return TR(T_ANYLANGUAGE);
  case Traits::English:         return TR(T_ENGLISH);
  case Traits::Japanese:        return TR(T_JAPANESE);
  case Traits::TraditionalChinese:     return TR(T_CHINESE);
  case Traits::SimplifiedChinese:      return TR(T_SIMPLIFIEDCHINESE);
  case Traits::Korean:          return TR(T_KOREAN);
  case Traits::French:          return TR(T_FRENCH);
  case Traits::German:          return TR(T_GERMAN);
  case Traits::Italian:         return TR(T_ITALIAN);
  case Traits::Spanish:         return TR(T_SPANISH);
  case Traits::Portuguese:      return TR(T_PORTUGUESE);
  case Traits::Russian:         return TR(T_RUSSIAN);

  //case Traits::UnknownLanguage:
  default : return TR(T_ALIEN);
  }
}


// - Playlist -

void
MainWindow::openPlaylistItem(int i)
{
  if (i >= 0 && i < playlist_.size())
    openSource(playlist_[i]);
}

void
MainWindow::clearPlaylist()
{
  playlist_.clear();
  updatePlaylistMenu();
}

void
MainWindow::updatePlaylist()
{
  if (playlist_.isEmpty())
    return;

  int size = playlist_.size();
  QMutableListIterator<QString> i(playlist_);
  while (i.hasNext())
    if (!QFile::exists(removeUrlPrefix(i.next())))
      i.remove();
  if (size != playlist_.size())
    updatePlaylistMenu();
}

void
MainWindow::openNextPlaylistItem()
{
  if (playlist_.size() <= 1)
    return;

  int i = playlist_.indexOf(recentOpenedFile_);
  i++;
  if (i < 0 || i >= playlist_.size())
    i = 0;
  openSource(playlist_[i]);
}

int
MainWindow::currentPlaylistIndex() const
{ return playlist_.indexOf(recentOpenedFile_); }

void
MainWindow::openPreviousPlaylistItem()
{
  if (playlist_.size() <= 1)
    return;
  if (playlist_.size() <= 1)
    return;

  int i = playlist_.indexOf(recentOpenedFile_);
  i--;
  if (i < 0 || i >= playlist_.size())
    i = 0;
  openSource(playlist_[i]);
}

void
MainWindow::updatePlaylistMenu()
{
  QList<QAction*> l = playlistMenu_->actions();
  foreach (QAction *a, l)
    if (a && a != clearPlaylistAct_ && a != previousPlaylistItemAct_ && a != nextPlaylistItemAct_)
      QTimer::singleShot(0, a, SLOT(deleteLater()));
  playlistMenu_->clear();

  if (!playlist_.isEmpty()) {
    int i = playlist_.indexOf(recentOpenedFile_);
    playlistMenu_->addAction(clearPlaylistAct_);
    if (i > 0)
      playlistMenu_->addAction(previousPlaylistItemAct_);
    if (i < playlist_.size() - 1)
      playlistMenu_->addAction(nextPlaylistItemAct_);
    playlistMenu_->addSeparator();

    int index = 0;
    foreach (const QString &f, playlist_) {
      QString path = removeUrlPrefix(f);
      QString text = QFileInfo(path).fileName();
      if (text.isEmpty()) {
        text = QDir(path).dirName();
        if (text.isEmpty())
          text = f;
      }
      text = QString::number(index+1) + ". " + shortenText(text);
      auto a = new QxActionWithId(index++, text, playlistMenu_);
      if (f == recentOpenedFile_) {
#ifdef Q_OS_LINUX
        a->setCheckable(true);
        a->setChecked(true);
#else
        a->setIcon(QIcon(RC_IMAGE_CURRENTFILE));
#endif // Q_OS_LINUX
      }
      connect(a, SIGNAL(triggeredWithId(int)), SLOT(openPlaylistItem(int)));

      playlistMenu_->addAction(a);
    }
  }
}

// - Recent -

void
MainWindow::updateRecent()
{
  if (recentFiles_.isEmpty())
    return;

  int size = recentFiles_.size();
  QMutableListIterator<QString> i(recentFiles_);
  while (i.hasNext()) {
    QString path = i.next();
    if (!isRemoteMrl(path) &&
        !QFile::exists(removeUrlPrefix(path)))
      i.remove();
  }

  qxStableUniqueList(recentFiles_);

  if (size != recentFiles_.size())
    updateRecentMenu();
}

void
MainWindow::updateRecentMenu()
{
  QList<QAction*> l = recentMenu_->actions();
  foreach (QAction *a, l)
    if (a && a != clearRecentAct_ && a != validateRecentAct_)
      QTimer::singleShot(0, a, SLOT(deleteLater()));
  recentMenu_->clear();

  if (!recentFiles_.isEmpty()) {
    int i = 0;
    foreach (const QString &f, recentFiles_) {
      if (i >= RECENT_COUNT)
        break;

      QString text;
      auto p = recentUrlTitles_.find(f);
      if (isRemoteMrl(f)) {
        if (p == recentUrlTitles_.end())
          text = f;
        else {
          text = QUrl(f).host();
          if (!text.isEmpty()) {
            text.remove(QRegExp("^www."))
                .remove(QRegExp(".jp$"))
                .remove(QRegExp(".cn$"))
                .remove(QRegExp(".tv$"))
                .remove(QRegExp(".com$"))
                .remove(QRegExp("^v."));
            text.append(": ");
          }
          text.append(p.value());
        }
      } else {
        QString path = removeUrlPrefix(f);
        QFileInfo fi(path);
        text = fi.fileName();
        if (text.isEmpty()) {
          text = QDir(path).dirName();
          if (text.isEmpty())
            text = f;
#ifdef Q_OS_WIN
          else if (text.size() > 3) {
            if (char d = QtWin::getFileDriveLetter(path))
              text.prepend(": ")
                  .prepend(QChar(d).toUpper());
          }
#endif // Q_OS_WIN
        }
#ifdef Q_OS_WIN
        else if (char d = QtWin::getFileDriveLetter(path))
          text.prepend(": ")
              .prepend(QChar(d).toUpper());
#endif // Q_OS_WIN
        if (p != recentUrlTitles_.end()) {
          QString title = p.value();
          if (title != fi.completeBaseName())
            text.append(": " + p.value());
        }
      }
      Q_ASSERT(!text.isEmpty());
      text = QString::number(i+1) + ". " + shortenText(text);
      auto a = new QxActionWithId(i++, text, recentMenu_);
      connect(a, SIGNAL(triggeredWithId(int)), SLOT(openRecent(int)));

      recentMenu_->addAction(a);
    }

    recentMenu_->addSeparator();
    recentMenu_->addAction(validateRecentAct_);
    recentMenu_->addAction(clearRecentAct_);
  }
}

void
MainWindow::clearRecent()
{
  recentFiles_.clear();
  updateRecentMenu();
}

void
MainWindow::openRecent(int i)
{
  if (i >= 0 && i < recentFiles_.size())
    openSource(recentFiles_[i]);
}

void
MainWindow::openSource(const QString &path)
{
  DOUT("path =" << path);
  if (path.isEmpty())
    return;

  if (libraryView_->isVisible() && libraryView_->autoHide())
    libraryView_->hide();

  if (path.startsWith(ACSCHEME_PLAYER_IMPORT)) {
    QString url = path;
    url.replace(QRegExp("^" ACSCHEME_PLAYER_IMPORT), "http://");
    if (player_->hasMedia())
      importUrl(url);
    else
      openSource(url);
    return;
  }

  if (isRemoteMrl(path)) {
    openUrl(path);
    return;
  }

  QFileInfo fi(removeUrlPrefix(path));
  if (!fi.exists()) {
    emit warning(TR(T_ERROR_BAD_FILEPATH) + ": " + path);
    return;
  }
  QString suffix = fi.suffix();
#ifdef Q_OS_WIN
  if (!suffix.compare("lnk", Qt::CaseInsensitive)) {
    QString lnk = QtWin::resolveLink(path, winId());
    if (!lnk.isEmpty()) {
      addRecent(path);
      openSource(lnk);
    } else
      emit warning(tr("invalid lnk") + ": " + path );
  } else
#endif // Q_OS_WIN
#ifdef AC_ENABLE_GAME
  if (!suffix.compare("exe", Qt::CaseInsensitive))
    openProcessPath(path);
  else
#endif // AC_ENABLE_GAME
  if (Player::isSupportedSubtitle(fi.fileName())) {
    if (player_->hasMedia())
      openSubtitleFile(path);
    else {
      QString mrl = Player::searchSubtitleMedia(fi.absoluteFilePath());
      if (mrl.isEmpty())
        openSubtitleFile(path);
      else
        openMrl(mrl, false); // check existence = false
    }
  } else if (!suffix.compare("xml") || !suffix.compare("json")) {
    if (player_->hasMedia())
      openAnnotationFile(path);
    else {
      QString mrl = Player::searchSubtitleMedia(fi.absoluteFilePath());
      if (mrl.isEmpty())
        openAnnotationFile(path);
      else
        openMrl(mrl, false); // check existence = false
    }
  } else if (!suffix.compare("mds")) {
    Q_ASSERT(path.size() > 3);
    QString mdf = path;
    mdf.chop(3);
    mdf.append("mdf");
    DOUT("load mdf instead of mds");
    openMrl(mdf);
  } else
    openMrl(path); // check existence = true
}

void
MainWindow::addRecent(const QString &input)
{
  QString path = QDir::fromNativeSeparators(input);
  //if (recentFiles_.contains(path))
  if (!recentFiles_.isEmpty())
    recentFiles_.removeAll(path);
  recentFiles_.prepend(path);
#ifdef Q_OS_WIN
  if (!isRemoteMrl(path) &&
      Registry::globalInstance()->containsType(QFileInfo(path).suffix())) {
    QString fileName = path;
    fileName.remove(QRegExp("^file://"));
    fileName.remove(QRegExp("^[/]+"));
    //fileName.replace('/', '\\');  // already done in addRecentDocument
    QtWin::addRecentDocument(fileName);
  }
#endif // Q_OS_WIN

  // SingleShot to avoid multi-threading issue.
  QTimer::singleShot(0, this, SLOT(updateRecentMenu()));
}


#ifdef AC_ENABLE_GAME

// - Game Text -

void
MainWindow::updateGameTextMenu()
{
  bool t = isGameTextVisible();
  toggleGameTextColorfulAct_->setEnabled(t);
  toggleGameTextResizableAct_->setEnabled(t);

  toggleTranslateSelectionAct_->setEnabled(t);
  toggleTranslateSelectionAct_->setChecked(annotationView_->isTranslateWordEnabled());
}

bool
MainWindow::isGameTextVisible() const
{ return toggleGameTextVisibleAct_->isChecked(); }

bool
MainWindow::isAppLocaleEnabled() const
{ return toggleAppLocaleEnabledAct_->isChecked(); }

void
MainWindow::showGameText(const QString &text, int role)
{
  if (!isGameTextVisible())
    return;
  if (text.isEmpty()) //|| !isTranslateEnabled()
    return;

  QString out;
  bool colorful = toggleGameTextColorfulAct_->isChecked(),
       resizable = toggleGameTextResizableAct_->isChecked();

  int charWidth = resizable ? 40 : 30;

  ulong renderHints = 0;
  if (colorful)
    renderHints |= MeCabParser::ColorHint;
  if (resizable)
    renderHints |= MeCabParser::ResizeHint;

  int count = annotationView_->width() / (AnnotationSettings::globalSettings()->scale() * charWidth);
  if (!count || text.size() < count)
    out = mecab_->renderText(text, renderHints);
  else {
    int n = qCeil(text.size() / qreal(count));
    for (int i = 0; i < n; i++) {
      QString t = text.mid(i * count,
                           i == n -1 ? -1 : count);
      if (!t.isEmpty()) {
        t = mecab_->renderText(t, renderHints);
        out.append(t);
      }
    }
  }

  if (!out.isEmpty()) {
    QString prefix = CORE_CMD_NOWRAP;
    if (role == TextThread::SupportRole)
      prefix.append(CORE_CMD_VIEW_FLOAT);
    //if (!resizable)
    //  prefix.append(CORE_CMD_LATEX_BIG);
    showSubtitle(out, Traits::Japanese, prefix);
  }
}

void
MainWindow::translateGameText(const QString &text, int role)
{
  if (text.isEmpty() || !isTranslateEnabled() || !server_->isConnected())
    return;
  bool extra = role == TextThread::SupportRole;
  translate(text, extra);
}

#endif // AC_ENABLE_GAME

// - Translate -

bool
MainWindow::isTranslateEnabled() const
{ return toggleTranslateAct_->isChecked(); }

void
MainWindow::setTranslateEnabled(bool enabled)
{ toggleTranslateAct_->setChecked(enabled); }

void
MainWindow::translate(const QString &text, bool extra)
{
  //if (!server_->isConnected())
  //  return;

  int l = server_->user().language();
  translate(text, l, extra);
}

void
MainWindow::translate(const QString &text, int lang, bool extra)
{
  //if (!server_->isConnected())
  //  return;
  (extra ? extraTranslator_ : translator_)
      ->translate(text, lang ? lang : Translator::English);
}

void
MainWindow::showTraditionalChinese(const QString &input)
{ showSubtitle(TextCodec::zhs2zht(input), Traits::TraditionalChinese); }

// - Subtitle -

void
MainWindow::uncheckSubtitleColorActions()
{
  setSubtitleColorToDefaultAct_->setChecked(false);
  setSubtitleColorToWhiteAct_->setChecked(false);
  setSubtitleColorToCyanAct_->setChecked(false);
  setSubtitleColorToBlueAct_->setChecked(false);
  setSubtitleColorToPurpleAct_->setChecked(false);
  setSubtitleColorToBlackAct_->setChecked(false);
  setSubtitleColorToOrangeAct_->setChecked(false);
  setSubtitleColorToRedAct_->setChecked(false);
}


void
MainWindow::setSubtitleColorToDefault()
{
  annotationView_->setSubtitlePrefix(QString());
  uncheckSubtitleColorActions();
  setSubtitleColorToDefaultAct_->setChecked(true);
}

#define SET_SUBTITLE_COLOR(_Color, _COLOR) \
  void \
  MainWindow::setSubtitleColorTo##_Color() \
  { \
    QString prefix = CORE_CMD_COLOR_##_COLOR " "; \
    annotationView_->setSubtitlePrefix(prefix); \
    uncheckSubtitleColorActions(); \
    setSubtitleColorTo##_Color##Act_->setChecked(true); \
  }

  SET_SUBTITLE_COLOR(White, WHITE)
  SET_SUBTITLE_COLOR(Cyan, CYAN)
  SET_SUBTITLE_COLOR(Black, BLACK)
  SET_SUBTITLE_COLOR(Blue, BLUE)
  SET_SUBTITLE_COLOR(Orange, ORANGE)
  SET_SUBTITLE_COLOR(Purple, PURPLE)
  SET_SUBTITLE_COLOR(Red, RED)
#undef SET_SUBTITLE_COLOR

void
MainWindow::setSubtitleColor(int c)
{
  switch (c) {
  case Black:   setSubtitleColorToBlack(); break;
  case White:   setSubtitleColorToWhite(); break;
  case Cyan:    setSubtitleColorToCyan(); break;
  case Blue:    setSubtitleColorToBlue(); break;
  case Red:     setSubtitleColorToRed(); break;
  case Orange:  setSubtitleColorToOrange(); break;
  case Purple:  setSubtitleColorToPurple(); break;
  case DefaultColor:
  default:      setSubtitleColorToDefault();
  }
}

int
MainWindow::subtitleColor() const
{
  if (setSubtitleColorToWhiteAct_->isChecked())
    return White;
  if (setSubtitleColorToCyanAct_->isChecked())
    return Cyan;
  if (setSubtitleColorToBlackAct_->isChecked())
    return Black;
  if (setSubtitleColorToBlueAct_->isChecked())
    return Blue;
  if (setSubtitleColorToRedAct_->isChecked())
    return Red;
  if (setSubtitleColorToOrangeAct_->isChecked())
    return Orange;
  if (setSubtitleColorToPurpleAct_->isChecked())
    return Purple;
  if (setSubtitleColorToDefaultAct_->isChecked())
    return DefaultColor;
  return DefaultColor;
}


// - Themes -

#define SET_THEME_TO(_theme) \
  void \
  MainWindow::setThemeTo##_theme() \
  { \
    AcUi::globalInstance()->setTheme(AcUi::_theme##Theme); \
\
    if (hub_->isMediaTokenMode() && hub_->isEmbeddedPlayerMode() && \
        player_->hasMedia() && \
        !AcUi::globalInstance()->isAeroEnabled()) \
      AcUi::globalInstance()->setBlackBackground(this); \
  }

  SET_THEME_TO(Default)
  SET_THEME_TO(Random)
  SET_THEME_TO(Dark)
  SET_THEME_TO(Black)
  SET_THEME_TO(Blue)
  SET_THEME_TO(Brown)
  SET_THEME_TO(Cyan)
  SET_THEME_TO(Gray)
  SET_THEME_TO(Green)
  SET_THEME_TO(Pink)
  SET_THEME_TO(Purple)
  SET_THEME_TO(Red)
  SET_THEME_TO(White)
  SET_THEME_TO(Yellow)
#undef SET_THEME_TO

void
MainWindow::enableWindowTransparency()
{
  if (!AcUi::globalInstance()->isAeroEnabled()) {
    if (fadeAni_->state() == QAbstractAnimation::Running)
      fadeAni_->stop();
    //fadeAni_->fadeIn(AC_WINDOW_OPACITY);
    setWindowOpacity(AC_WINDOW_OPACITY);
  }
}

void
MainWindow::disableWindowTransparency()
{
  if (!AcUi::globalInstance()->isAeroEnabled()) {
    if (fadeAni_->state() == QAbstractAnimation::Running)
      fadeAni_->stop();
    //fadeAni_->fadeIn(1.0);
    setWindowOpacity(1.0);
  }
}

// - Browse -

QString
MainWindow::browsedDirectory() const
{
  QFileInfo fi(browsedUrl_);
  QString ret = fi.absolutePath();
  if (!QFile::exists(ret)) {
    ret = G_PATH_DOWNLOADS;
    if (!QFile::exists(ret))
      ret = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
  }
  return ret;
}

void
MainWindow::openBrowsedDirectory()
{
  QString path = browsedDirectory();
#ifdef Q_OS_WIN
  path = QDir::fromNativeSeparators(path);
  path.prepend("file:///");
#else
  path.prepend("file://");
#endif // Q_OS_WIN
  QDesktopServices::openUrl(path);
}

void
MainWindow::setBrowsedUrl(const QString &url)
{
  DOUT("url =" << url);
  enum { increment = 5 };

  //browsedFiles_.clear();
  int id = 0;
  QString prefix, suffix;

  QRegExp rxa("(http://.*_)(\\d+)", Qt::CaseInsensitive),
          rxb("(http://.*/index_)(\\d+)(.html)", Qt::CaseInsensitive);
  if (rxb.exactMatch(url)) {
    Q_ASSERT(rxb.captureCount() == 3);
    prefix = rxb.cap(1);
    QString n = rxb.cap(2);
    suffix = rxb.cap(3);
    id = n.toUInt();
  } else if (rxa.exactMatch(url)) {
    Q_ASSERT(rxa.captureCount() == 2);
    prefix = rxa.cap(1);
    QString n = rxa.cap(2);
    id = n.toUInt();
  }

  if (id) {
    browsedFiles_.clear();
    for (int i = 1; i <= id + increment; i++) {
      QString f = prefix + QString::number(i) + suffix;
      browsedFiles_.append(f);
    }
  }

  updateBrowseMenu(url);
}

void
MainWindow::setBrowsedFile(const QString &fileName)
{
  DOUT("url =" << fileName);
  //browsedFiles_.clear();

  QFileInfo fi(fileName);
  QDir dir = fi.dir();

  if (dir.exists()) {
    int tt = fileType(fileName);
    QStringList nf;
    switch (tt) {
    case Token::TT_Video:         nf = Player::supportedVideoFilters(); break;
    case Token::TT_Audio:         nf = Player::supportedAudioFilters(); break;
    case Token::TT_Picture:       nf = Player::supportedPictureFilters(); break;
    }
    if (!nf.isEmpty())
      browsedFiles_ = dir.entryInfoList(nf, QDir::Files);
  }

  updateBrowseMenu(fileName);
}

void
MainWindow::updateBrowseMenu()
{
  if (player_->hasMedia()) {
    QString fileName = recentUrl_.isEmpty() ? player_->mediaPath()
                                            : recentUrl_;
    updateBrowseMenu(fileName);
  }
}

void
MainWindow::updateBrowseMenu(const QString &fileName)
{
  browsedUrl_ = fileName;
  if (!browseMenu_->isEmpty()) {
    foreach (QAction *a, browseMenu_->actions())
      if (a != previousFileAct_ && a != nextFileAct_ &&
          a !=  moreFilesAct_ && a != lessFilesAct_ &&
          a != openBrowsedDirectoryAct_)
        QTimer::singleShot(0, a, SLOT(deleteLater()));
    browseMenu_->clear();
  }

  QString path = browsedDirectory();
  if (path.isEmpty())
    openBrowsedDirectoryAct_->setText(TR(T_MENUTEXT_OPENDIRECTORY));
  else {
    QString t = QFileInfo(path).baseName();
#ifdef Q_OS_WIN
    if (!t.contains(':')) {
      if (char c = QtWin::getFileDriveLetter(path))
        t.prepend(':')
         .prepend(QChar(c).toUpper());
    }
#endif // Q_OS_WIN
    t.prepend(TR(T_OPEN) + ": ");
    openBrowsedDirectoryAct_->setText(t);
  }

  browseMenu_->addAction(openBrowsedDirectoryAct_);
  browseMenu_->addSeparator();

  bool remote = isRemoteMrl(fileName);
  if (remote && browsedFiles_.isEmpty())
    return;

  if (browsedFiles_.size() > 10) {
    browseMenu_->addAction(previousFileAct_);
    browseMenu_->addAction(nextFileAct_);
  }

  if (remote) {
    browseMenu_->addAction(moreFilesAct_);
    if (browsedFiles_.size() > 10)
      browseMenu_->addAction(lessFilesAct_);
  }

  if (!browseMenu_->isEmpty())
    browseMenu_->addSeparator();

  QFileInfo fi(fileName);

  int id = 0;
  foreach (const QFileInfo &f, browsedFiles_) {
    QString text = QString::number(id+1) + ". " + f.fileName();
    auto a = new QxActionWithId(id++, text, browseMenu_);
    if (f.fileName() == fi.fileName()) {
#ifdef Q_OS_LINUX
      a->setCheckable(true);
      a->setChecked(true);
#else
      a->setIcon(QIcon(RC_IMAGE_CURRENTFILE));
#endif // Q_OS_LINUX
    }
    connect(a, SIGNAL(triggeredWithId(int)), SLOT(openBrowsedFile(int)));
    browseMenu_->addAction(a);
  }
}

void
MainWindow::lessBrowsedFiles()
{
  int decrement = 5;
  if (browsedFiles_.size() <= decrement)
    return;

  emit message(tr("less files to browse"));
  while (decrement--)
    browsedFiles_.pop_back();
  updateBrowseMenu();
}


void
MainWindow::moreBrowsedFiles()
{
  enum { increment = 5 };
  if (browsedFiles_.isEmpty())
    return;

  QString url = browsedFiles_.last().filePath();

  int id = 0;
  QString prefix, suffix;

  QRegExp rxa("(http://.*_)(\\d+)", Qt::CaseInsensitive),
          rxb("(http://.*/index_)(\\d+)(.html)", Qt::CaseInsensitive);
  if (rxb.exactMatch(url)) {
    Q_ASSERT(rxb.captureCount() == 3);
    prefix = rxb.cap(1);
    QString n = rxb.cap(2);
    suffix = rxb.cap(3);
    id = n.toUInt();
  } else if (rxa.exactMatch(url)) {
    Q_ASSERT(rxa.captureCount() == 2);
    prefix = rxa.cap(1);
    QString n = rxa.cap(2);
    id = n.toUInt();
  }

  if (id) {
    emit message(tr("more files added"));
    for (int i = id + 1; i <= id + increment; i++) {
      QString f = prefix + QString::number(i) + suffix;
      browsedFiles_.append(f);
    }
    updateBrowseMenu();
  }
}

void
MainWindow::openBrowsedFile(int id)
{
  if (id < 0)
    id = browsedFiles_.size() - 1;
  else if (id >= browsedFiles_.size())
    id = 0;

  if (id >= 0 && id < browsedFiles_.size())
    openUrl(browsedFiles_[id].filePath());
}

int
MainWindow::currentBrowsedFileId() const
{
  if (browsedFiles_.isEmpty())
    return 0;

  QString path;
  if (player_->hasMedia())
    path = player_->mediaPath();
  if (path.isEmpty())
    return 0;

  QFileInfo fi(path);
  //if (!fi.exists())
  //  return 0;

  int ret = 0;
  foreach (const QFileInfo &f, browsedFiles_) {
    if (f.fileName() == fi.fileName())
      break;
    ret++;
  }
  return ret;
}

void
MainWindow::openPreviousFile()
{ openBrowsedFile(currentBrowsedFileId() - 1); }

void
MainWindow::openNextFile()
{ openBrowsedFile(currentBrowsedFileId() + 1); }

// - Previous / Next -

void
MainWindow::openPreviousMrl()
{
  QString mrl = dataManager_->token().url();
  if (!mrl.isEmpty()) {
    QString n = qxDecreaseString(mrl);
    if (n != mrl) {
      n.replace("/index_0.html", "/");
      n.replace("/index_1.html", "/");
      openSource(n);
    }
  }
}

void
MainWindow::openNextMrl()
{
  QString mrl = dataManager_->token().url();
  if (!mrl.isEmpty()) {
    if (mrl.endsWith("/")) {
      mrl.append("index_2.html");
      openSource(mrl);
    } else {
      QString n = qxIncreaseString(mrl);
      if (n != mrl)
        openSource(n);
    }
  }
}

void
MainWindow::previous()
{
  if (hub_->isMediaTokenMode() && player_->hasMedia()) {
    setPreviousEnabled(false);
    if (player_->hasPlaylist())
      player_->previousTrack();
    //else if (player_->hasChapters())
    //  player_->setPreviousChapter();
    else if (player_->titleCount() > 1)
      player_->setPreviousTitle();
    else if (playlist_.size() > 1)
      openPreviousPlaylistItem();
    else if (browsedFiles_.size() > 1)
      openPreviousFile();
    else if (isNavigable())
      openPreviousMrl();
  }
}

bool
MainWindow::hasNext() const
{
  return player_->hasMedia() && (
    player_->trackNumber() < player_->trackCount() - 1 ||
    player_->titleId() < player_->titleCount() - 1 ||
    playlist_.size() > 1 && playlist_.indexOf(recentOpenedFile_) < playlist_.size() - 1 ||
    browsedFiles_.size() > 1 && currentBrowsedFileId() < browsedFiles_.size() - 1 ||
    isNavigable()
  );
}

void
MainWindow::checkReachEnd()
{
  enum { Timeout = MiniConsoleTimeout + 1000 };  // just a little longer then message timeout
  static qint64 timestamp = 0;
  if (hub_->isMediaTokenMode() && player_->isPlaying()) {
    qint64 current_msecs = player_->time(),
           total_msecs = player_->mediaLength();
    if ((total_msecs - current_msecs) < Timeout) {
      qint64 now = QDateTime::currentMSecsSinceEpoch();
      if (now - timestamp > Timeout) {
        emit messageOnce(tr("Reaching End Soon ..."));
        timestamp = now;
      }
    }
  }
}

void
MainWindow::next()
{
  if (hub_->isMediaTokenMode() && player_->hasMedia()) {
    setNextEnabled(false);
    if (player_->hasPlaylist())
      player_->nextTrack();
    //else if (player_->hasChapters())
    //  player_->setNextChapter();
    else if (player_->titleCount() > 1)
      player_->setNextTitle();
    else if (playlist_.size() > 1)
      openNextPlaylistItem();
    else if (browsedFiles_.size() > 1)
      openNextFile();
    else if (isNavigable())
      openNextMrl();
  }
}

void
MainWindow::autoPlayNext()
{
  if (isAutoPlayNext() && hasNext())
    next();
  else if (repeatCurrent())
    replay();
  else
    emit playingFinished();
}

// - Tracking window -

void
MainWindow::updateTraceButtons()
{
  bool t = annotationView_->trackedWindow();
  mainPlayer_->toggleTraceWindowButton()->setChecked(t);
  miniPlayer_->toggleTraceWindowButton()->setChecked(t);
  embeddedPlayer_->toggleTraceWindowButton()->setChecked(t);
}

void
MainWindow::setEmbeddedWindow(WId winId)
{
  if (!player_->isValid())
    return;

  if (!winId)
    player_->embed(videoView_);
  else {
#ifndef Q_OS_MAC
    player_->embed(nullptr);
    player_->setEmbeddedWindow(winId);
#endif // !Q_OS_MAC
  }
}

// - Live mode -

//void
//MainWindow::openChannel()
//{
//  Q_ASSERT(liveInterval_ > 0);
//  annotationView_->setInterval(liveInterval_);
//  liveTimer_->setInterval(liveInterval_);
//
//  annotationView_->start();
//  liveTimer_->start();
//}
//
//void
//MainWindow::closeChannel()
//{
//  liveTimer_->stop();
//  annotationView_->stop();
//}
//
//void
//MainWindow::updateLiveAnnotations(bool async)
//{
//  if (disposed_) {
//    DOUT("exit: returned from disposed branch");
//    return;
//  }
//  if (!server_->isConnected() || !server_->isAuthorized())
//    return;
//  //DOUT("enter: async =" << async);
//
//  if (async) {
//    //log(tr("updating annotations ..."));
//    QtConcurrent::run(this, &Self::updateLiveAnnotations, false);
//    //DOUT("exit: returned from async branch");
//    return;
//  }
//
//  //DOUT("inetMutex locking");
//  inetMutex_.lock();
//  //DOUT("inetMutex locked");
//
//  AnnotationList l = server_->selectLiveAnnotations();
//  //DOUT("annots.count =" << l.size());
//
//  if (!l.isEmpty())
//    //annotationView_->appendAnnotations(l);
//    emit appendAnnotationsRequested(l);
//
//  //DOUT("inetMutex unlocking");
//  inetMutex_.unlock();
//  //DOUT("inetMutex unlocked");
//
//  //DOUT("exit");
//}

// - Remote annotation -

void
MainWindow::addRemoteAnnotations(const AnnotationList &l, const QString &url, const QString &originalUrl)
{
  DOUT("enter: annot.count =" << l.size() << ", url =" << url << ", originalUrl =" << originalUrl);
  //bool remote = isRemoteMrl(url);
  //if (remote && !registerAnnotationUrl(url)) {
  //  DOUT("exit: already imported");
  //  return;
  //}
  if (l.isEmpty()) {
    DOUT("exit: empty list");
    return;
  }
  const Token &t = dataManager_->token();
  AnnotationList annots;
  if (t.hashId() || t.hasDigest())
    foreach (Annotation a, l) {
      a.setTokenId(t.id());
      a.setTokenSection(t.section());
      a.setTokenDigest(t.digest());
      annots.append(a);
    }
  else
    annots = l;

   QString src = isRemoteMrl(url) ? url : "http://nicovideo.jp";
   QString msg = QString("%1 :" HTML_SS_OPEN(color:red) "+%2" HTML_SS_CLOSE() ", %3")
     .arg(tr("annotations found"))
     .arg(QString::number(annots.size()))
     .arg(src);
   emit message(msg);
   emit addAnnotationsRequested(annots);
   DOUT("token url =" << t.url());
   if (cache_->isValid() &&
       t.hasId() && !t.hasUrl())
     cache_->insertAnnotations(annots);

   if (!originalUrl.isEmpty() &&
       toggleSaveAnnotationFileAct_->isChecked() &&
       hub_->isMediaTokenMode() && player_->hasMedia()) {
     QString path = player_->mediaPath();
     if (isRemoteMrl(path)) {
       QString title = player_->mediaTitle();
       if (title.isEmpty()) {
         auto p = recentUrlTitles_.find(originalUrl);
         if (p != recentUrlTitles_.end())
           title = p.value();
       }
       if (!title.isEmpty()) {
         QString dir = AcLocationManager::globalInstance()->downloadsLocation();
         QFileInfo fi(dir);
         if (!fi.exists())
           AcLocationManager::globalInstance()->createDownloadsLocation();
         if (fi.exists() && fi.isDir()) {
           QString baseName = qxEscapeFileName(title);
           QString annotFile = dir + "/" + baseName;
           QString cacheFile = AnnotationCacheManager::globalInstance()->findFile(originalUrl);
           if (!cacheFile.isEmpty()) {
             annotFile.append('.')
                      .append(QFileInfo(cacheFile).fileName());
             qxTrashOrRemoveFile(annotFile); // overwrite existing files
             bool ok = QFile::copy(cacheFile, annotFile);
             DOUT("copy =" << ok << ", annotFile =" << annotFile << ", cacheFile =" << cacheFile);
             if (ok)
               emit message(tr("file saved") + ": " + QFileInfo(annotFile).fileName());
             else
               emit warning(tr("failed to save file") + ": " + annotFile);
           }
         }
       }
     } else {
       QFileInfo fi(path);
       if (!fi.isDir()) {
         QString dirName = fi.absolutePath(),
                 baseName = fi.completeBaseName();
         if (!dirName.isEmpty() && !baseName.isEmpty()) {
           QString annotFile = dirName + "/" + baseName;
           QString cacheFile = AnnotationCacheManager::globalInstance()->findFile(originalUrl);
           if (!cacheFile.isEmpty()) {
             annotFile.append('.')
                      .append(QFileInfo(cacheFile).fileName());
             qxTrashOrRemoveFile(annotFile); // overwrite existing files
             bool ok = QFile::copy(cacheFile, annotFile);
             DOUT("copy =" << ok << ", annotFile =" << annotFile << ", cacheFile =" << cacheFile);
             if (ok)
               emit message(tr("file saved") + ": " + QFileInfo(annotFile).fileName());
             else
               emit warning(tr("failed to save file") + ": " + annotFile);
           }
         }
       }
     }
   }

  DOUT("exit");
}

void
MainWindow::importUrl(const QString &url)
{
  if (!url.isEmpty())
    openAnnotationUrl(url, submit_);
}

void
MainWindow::importAnnotationsFromUrl(const QString &suburl, const QString &refurl)
{
  DOUT("enter: import annot from URL:" << suburl << ", refurl =" << refurl);
  if (!suburl.isEmpty()) {
    QString url = isRemoteMrl(suburl) ? suburl : refurl;
    emit message(tr("analyzing annotation URL ...") + ": " + url);
    if (registerAnnotationUrl(suburl))
      AnnotationCodecManager::globalInstance()->fetch(suburl, refurl);
  }
  DOUT("exit");
}

bool
MainWindow::isAnnotationUrlRegistered(const QString &suburl) const
{
  QMutexLocker lock(&annotMutex_);
  QString k = MrlAnalysis::normalizeUrl(suburl);
  return annotationUrls_.contains(k, Qt::CaseInsensitive);
}

bool
MainWindow::registerAnnotationUrl(const QString &suburl)
{
  QMutexLocker lock(&annotMutex_);
  if (suburl.isEmpty())
    return false;
  QString k = DataManager::normalizeUrl(suburl);
  if (annotationUrls_.contains(k, Qt::CaseInsensitive))
    return false;
  annotationUrls_.append(k);
  return true;
}

void
MainWindow::clearAnnotationUrls()
{
  QMutexLocker lock(&annotMutex_);
  if (!annotationUrls_.isEmpty())
    annotationUrls_.clear();
}

bool
MainWindow::registerAnnotationFile(const QString &fileName)
{
  QMutexLocker lock(&annotMutex_);
  QString key = QDir::fromNativeSeparators(fileName);
  key.remove("file:///").remove("file://");

  if (fileName.isEmpty() ||
      annotationFiles_.contains(fileName, Qt::CaseInsensitive))
    return false;
  annotationFiles_.append(key);
  return true;
}

void
MainWindow::clearAnnotationFiles()
{
  QMutexLocker lock(&annotMutex_);
  if (!annotationFiles_.isEmpty())
    annotationFiles_.clear();
}

// - Source -

QString
MainWindow::currentTitle() const
{
  QString ret;
  if (hub_->isMediaTokenMode()) {
    if (player_->hasMedia()) {
      ret = player_->mediaTitle();
      if (ret.isEmpty()) {
        ret = player_->mediaPath();
        if (isRemoteMrl(ret))
          ret.clear();
      }
    }
  } else if (hub_->isSignalTokenMode())
    ret = windowTitle();

  if (!ret.isEmpty())
    ret = QFileInfo(ret).completeBaseName();
  return ret;
}

QString
MainWindow::currentUrl() const
{
  QString ret;
  if (dataManager_->token().hasUrl())
    ret = dataManager_->token().url();
  else if (dataManager_->hasAliases())
    foreach (const Alias &a, dataManager_->aliases())
      if (a.type() == Alias::AT_Url) {
        ret = a.text();
        if (ret.contains(MA_EIGEN_NICOVIDEO, Qt::CaseInsensitive) ||
            ret.contains(MA_EIGEN_BILIBILI, Qt::CaseInsensitive))
          break;
      }

  return isRemoteMrl(ret) ? ret : QString();
}

void
MainWindow::downloadCurrentUrl()
{
  QString url = currentUrl();
  if (!url.isEmpty())
    enterDownloadUrls(QStringList(url));
  else
    showDownloader();
}

void
MainWindow::copyCurrentUrl()
{
  QString url = currentUrl();
  QClipboard *c = QApplication::clipboard();
  if (!url.isEmpty() && c) {
    c->setText(url);
    emit message(tr("copied") + ": " +
      HTML_SS_OPEN(color:orange) + url + HTML_SS_CLOSE()
    );
  }
}

void
MainWindow::copyCurrentTitle()
{
  QString url = currentTitle();
  QClipboard *c = QApplication::clipboard();
  if (!url.isEmpty() && c) {
    c->setText(url);
    emit message(tr("copied") + ": " +
      HTML_SS_OPEN(color:orange) + url + HTML_SS_CLOSE()
    );
  }
}

void
MainWindow::clickProgressButton()
{
  if (player_->isDownloadFinished())
    AcLocationManager::globalInstance()->openDownloadsLocation();
  else
    downloadCurrentUrl();
}

void
MainWindow::openInWebBrowser()
{
  QString url = currentUrl();
  if (!url.isEmpty())
    browserDelegate_->openUrl(url);
    //QDesktopServices::openUrl(url);
}

// - Error handling -

void
MainWindow::handlePlayerError()
{
  DOUT("enter");
  DOUT("exit");
}

// - Helpers -

QString
MainWindow::shortenText(const QString &text)
{
  if (text.size() <= 40)
    return text;
  else
    return text.left(25) + "..." + text.right(15);
}

bool
MainWindow::isRemoteMrl(const QString& mrl)
{ return mrl.contains(QRegExp("[ts]p://", Qt::CaseInsensitive)); }

// - Resume -

void
MainWindow::loadSubtitlesLater()
{ loadSubtitlesTimer_->start(3); }

void
MainWindow::loadSubtitles()
{
  //DOUT("enter");
  if (player_->hasMedia() && !player_->isStopped()) {
    loadSubtitlesTimer_->stop();
    if (!player_->hasSubtitles())
      player_->loadExternalSubtitles();
  }
  //DOUT("exit");
}

void
MainWindow::rememberPlayPos()
{
  //DOUT("enter");
  enum { margin = 90 * 1000 }; // 1.5min
  if (player_->hasMedia() &&
      canResume(player_->mediaPath())) {

    qint64 hash = dataManager_->token().hashId();
    DOUT("hash =" << hash);
    if (!hash) {
      DOUT("exit: no hash");
      return;
    }

    qint64 pos = player_->time();
    DOUT("pos =" << pos);
    if (pos <= margin) { // 1.5min
      if (playPosHistory_.contains(hash))
        playPosHistory_.remove(hash);
      DOUT("exit: pos too small");
      return;
    }
    qint64 len = player_->mediaLength();
    DOUT("length =" << len);
    if (pos >= len - margin) {
      if (playPosHistory_.contains(hash))
        playPosHistory_.remove(hash);
      DOUT("exit: pos too large");
      return;
    }

    if (playPosHistory_.size() > HISTORY_SIZE) {
      // maximum play pos count to remember
      int i = qrand() % playPosHistory_.size();
      qint64 k = playPosHistory_.keys()[i];
      playPosHistory_.remove(k);
    }
    playPosHistory_[hash] = pos;
  }
  //DOUT("exit");
}

bool
MainWindow::canResume(const QString &url)
{ return !isRemoteMrl(url) || url.contains(".youtube.com"); }

void
MainWindow::resumePlayPos()
{
  //DOUT("enter");
  if (player_->hasMedia() && !player_->isStopped()) {
    if (!canResume(player_->mediaPath())) {
      resumePlayTimer_->stop();
      DOUT("exit: remote media is not resumed (since seek not implemented)");
      return;
    }
    if (player_->time() > 20 * 1000) { // 20 seconds, user manually seeked
      resumePlayTimer_->stop();
      DOUT("exit: user manually seeked");
      return;
    }
    qint64 hash = dataManager_->token().hashId();
    DOUT("hash =" << hash);
    if (!hash) {
      DOUT("exit: no hash");
      return;
    }

    resumePlayTimer_->stop();

    qint64 pos = 0;
    if (playPosHistory_.contains(hash))
      pos = playPosHistory_[hash];
    DOUT("pos =" << pos);
    if (pos > 0 && pos < player_->mediaLength()) {
      emit message(
        tr("resuming last play") + ": "
        HTML_SS_OPEN(color:orange)
        + qxTimeFromMsec(pos).toString("hh:mm:ss") +
        HTML_SS_CLOSE()
      );
      pos -= 5000; // seek back 5 seconds
      seek(pos);
    }
  }
  //DOUT("exit");
}

void
MainWindow::rememberSubtitle()
{
  //DOUT("enter");
  if (player_->hasMedia() && player_->subtitleCount() > 1) {
    int track = preferredSubtitleTrack_ =
        player_->isSubtitleVisible() ? player_->subtitleId() : -1;
    DOUT("track =" << track);

    qint64 hash = dataManager_->token().hashId();
    DOUT("hash =" << hash);
    if (!hash) {
      DOUT("exit: no hash");
      return;
    }

    if (track)
      subtitleHistory_[hash] = track;
    else
      subtitleHistory_.remove(hash);

    if (subtitleHistory_.size() > HISTORY_SIZE) {
      // maximum play pos count to remember
      int i = qrand() % subtitleHistory_.size();
      qint64 k = subtitleHistory_.keys()[i];
      subtitleHistory_.remove(k);
    }
  }
  //DOUT("exit");
}

void
MainWindow::resumeSubtitle()
{
  //DOUT("enter");
  if (player_->hasMedia() && player_->subtitleCount() > 1) {
    resumeSubtitleTimer_->stop();
    qint64 hash = dataManager_->token().hashId();
    DOUT("hash =" << hash);

    int track = hash && subtitleHistory_.contains(hash) ? subtitleHistory_[hash] :
                                                          preferredSubtitleTrack_;
    DOUT("track =" << track);
    if (track >= 0 && track < player_->subtitleCount() &&
        track != player_->subtitleId()) {
      if (track > 0) {
        QStringList desc = player_->subtitleDescriptions();
        if (track < desc.size())
          emit message(tr("loading last subtitle") + ": " + desc[track]);
        else
          emit message(tr("loading last subtitle"));
        player_->setSubtitleId(track);
      }
      player_->setSubtitleVisible(true);
    } else if (track == -1) {
      emit message(tr("hide last subtitle"));
      player_->setSubtitleVisible(false);
    }
  }
  //DOUT("exit");
}

void
MainWindow::rememberAudioTrack()
{
  //DOUT("enter");
  if (player_->hasMedia() && player_->audioTrackCount() > 1) {
    int track = preferredAudioTrack_ = player_->audioTrackId();

    DOUT("track =" << track);
    if (track < 0) {
      DOUT("exit: invalid track");
      return;
    }

    qint64 hash = dataManager_->token().hashId();
    DOUT("hash =" << hash);
    if (!hash) {
      DOUT("exit: no hash");
      return;
    }

    if (track)
      audioTrackHistory_[hash] = track;
    else
      audioTrackHistory_.remove(hash);

    if (audioTrackHistory_.size() > HISTORY_SIZE) {
      // maximum play pos count to remember
      int i = qrand() % audioTrackHistory_.size();
      qint64 k = audioTrackHistory_.keys()[i];
      audioTrackHistory_.remove(k);
    }
  }
  //DOUT("exit");
}

void
MainWindow::resumeAudioTrack()
{
  //DOUT("enter");
  if (player_->hasMedia() && player_->audioTrackCount() > 1) {
    resumeAudioTrackTimer_->stop();
    qint64 hash = dataManager_->token().hashId();
    DOUT("hash =" << hash);

    int track = hash && audioTrackHistory_.contains(hash) ? audioTrackHistory_[hash] :
                                                            preferredAudioTrack_;
    DOUT("track =" << track);
    if (track >= 0 && track < player_->audioTrackCount()
        && track != player_->audioTrackId()) {
      if (track > 0) {
        QStringList desc =  player_->audioTrackDescriptions();
        if (track < desc.size())
          emit message(tr("loading last audio track") + ": " + desc[track]);
        else
          emit message(tr("loading last audio track"));
      }
      player_->setAudioTrackId(track);
    }
  }
  //DOUT("exit");
}

void
MainWindow::rememberAudioChannel()
{
  //DOUT("enter");
  if (player_->hasMedia() && player_->audioChannelCount() > 1) {
    int ch = preferredAudioChannel_ = player_->audioChannel();

    DOUT("channel =" << ch);
    if (ch <= 0) {
      DOUT("exit: invalid channel");
      return;
    }

    qint64 hash = dataManager_->token().hashId();
    DOUT("hash =" << hash);
    if (!hash) {
      DOUT("exit: no hash");
      return;
    }

    if (ch && ch != Player::StereoChannel)
      audioChannelHistory_[hash] = ch;
    else
      audioChannelHistory_.remove(hash);

    if (audioChannelHistory_.size() > HISTORY_SIZE) {
      // maximum play pos count to remember
      int i = qrand() % audioChannelHistory_.size();
      qint64 k = audioChannelHistory_.keys()[i];
      audioChannelHistory_.remove(k);
    }
  }
  //DOUT("exit");
}

void
MainWindow::resumeAudioChannel()
{
  //DOUT("enter");
  if (player_->hasMedia() && player_->audioChannelCount() > 1) {
    resumeAudioChannelTimer_->stop();
    qint64 hash = dataManager_->token().hashId();
    DOUT("hash =" << hash);

    int ch = hash && audioChannelHistory_.contains(hash) ? audioChannelHistory_[hash] :
                                                           preferredAudioChannel_;
    DOUT("channel =" << ch);
    if (ch > 0 && ch <= Player::DolbysChannel && ch != Player::StereoChannel) {
      emit message(tr("loading last audio channel"));
      player_->setAudioChannel(ch);
    }
  }
  //DOUT("exit");
}

void
MainWindow::rememberAspectRatio()
{
  //DOUT("enter");
  if (player_->hasMedia()) {
    QString ratio = player_->aspectRatio();

    DOUT("ratio =" << ratio);

    qint64 hash = dataManager_->token().hashId();
    DOUT("hash =" << hash);
    if (!hash) {
      DOUT("exit: no hash");
      return;
    }

    if (ratio.isEmpty())
      aspectRatioHistory_.remove(hash);
    else
      aspectRatioHistory_[hash] = ratio;

    if (aspectRatioHistory_.size() > HISTORY_SIZE) {
      // maximum count to remember
      int i = qrand() % aspectRatioHistory_.size();
      qint64 k = aspectRatioHistory_.keys()[i];
      aspectRatioHistory_.remove(k);
    }
  }
  //DOUT("exit");
}

void
MainWindow::resumeAspectRatio()
{
  //DOUT("enter");
  //if (player_->hasMedia()) {
  if (player_->isValid()) {
    qint64 hash = dataManager_->token().hashId();
    DOUT("hash =" << hash);
    if (!hash) {
      DOUT("exit: empty hash");
      return;
    }

    resumeAspectRatioTimer_->stop();

    QString ratio;
    if (aspectRatioHistory_.contains(hash))
      ratio = aspectRatioHistory_[hash];

    DOUT("ratio =" << ratio);
    if (!ratio.isEmpty()) {
      emit message(tr("using last aspect ratio") + ": " + ratio);
      player_->setAspectRatio(ratio);
    }
  }
  //DOUT("exit");
}

// - Effect -

void
MainWindow::updateAnnotationEffectMenu()
{
  AnnotationGraphicsView::RenderHint hint = annotationView_->renderHint();
  setAnnotationEffectToDefaultAct_->setChecked(hint == AnnotationGraphicsView::DefaultRenderHint);
  setAnnotationEffectToTransparentAct_->setChecked(hint == AnnotationGraphicsView::TransparentHint);
  setAnnotationEffectToShadowAct_->setChecked(hint == AnnotationGraphicsView::ShadowHint);
  setAnnotationEffectToBlurAct_->setChecked(hint == AnnotationGraphicsView::BlurHint);
}

void
MainWindow::setAnnotationEffectToDefault()
{ annotationView_->setRenderHint(AnnotationGraphicsView::DefaultRenderHint); }
void
MainWindow::setAnnotationEffectToTransparent()
{ annotationView_->setRenderHint(AnnotationGraphicsView::TransparentHint); }
void
MainWindow::setAnnotationEffectToShadow()
{ annotationView_->setRenderHint(AnnotationGraphicsView::ShadowHint); }
void
MainWindow::setAnnotationEffectToBlur()
{ annotationView_->setRenderHint(AnnotationGraphicsView::BlurHint); }

// - Navigation -

bool
MainWindow::isNavigable() const
{
  if (!hub_->isMediaTokenMode() ||
      !player_->hasMedia())
    return false;
  if (!isRemoteMrl(player_->mediaPath()))
    return true;

  QString mrl = dataManager_->token().url();
  return mrl.contains(QRegExp("index_\\d+.html$", Qt::CaseInsensitive));
}

void
MainWindow::setPreviousEnabled(bool t)
{
  PlayerPanel *players[] = { mainPlayer_, miniPlayer_, embeddedPlayer_ };
  BOOST_FOREACH(PlayerPanel *p, players) {
    p->previousButton()->setEnabled(t);
#ifdef Q_OS_WIN
    QtWin::repaintWindow(mainPlayer_->previousButton()->winId());
#endif // Q_OS_WIN
  }

  previousAct_->setEnabled(t);
}

void
MainWindow::setNextEnabled(bool t)
{
  PlayerPanel *players[] = { mainPlayer_, miniPlayer_, embeddedPlayer_ };
  BOOST_FOREACH(PlayerPanel *p, players) {
    p->nextButton()->setEnabled(t);
#ifdef Q_OS_WIN
    QtWin::repaintWindow(mainPlayer_->nextButton()->winId());
#endif // Q_OS_WIN
  }

  nextAct_->setEnabled(t);
}
void
MainWindow::setNavigationEnabled(bool t)
{
  PlayerPanel *players[] = { mainPlayer_, miniPlayer_, embeddedPlayer_ };
  BOOST_FOREACH(PlayerPanel *p, players) {
    p->openButton()->setEnabled(t);
#ifdef Q_OS_WIN
    QtWin::repaintWindow(mainPlayer_->openButton()->winId());
#endif // Q_OS_WIN
  }
  if (t && !isNavigable())
    return;

  setPreviousEnabled(t);
  setNextEnabled(t);

  if (!t)
    navigationTimer_->start();
}

// - Clipboard -

void
MainWindow::enterAnnotationUrl(const QString &url)
{
  //if (downloadDialog_ && downloadDialog_->isAddingUrls()) {
  //  downloadDialog_->promptUrl(url);
  //  return;
  //}
  if (!hub_->isMediaTokenMode())
    return;
  if (isAnnotationUrlRegistered(url)) {
    emit message(tr("annot URL is already used") + ": "
        HTML_SS_OPEN(color:orange) + url + HTML_SS_CLOSE()
    );
    return;
  }
  if (url == dataManager_->token().url()) {
    emit message(tr("media URL is being played ") + ": "
      HTML_SS_OPEN(color:orange) + url + HTML_SS_CLOSE()
    );
    return;
  }
  foreach (const Alias &a, dataManager_->aliases())
    if (a.type() == Alias::AT_Url &&
        a.text() == url) {
      emit message(tr("annotation URL already imported") + ": "
        HTML_SS_OPEN(color:orange) + url + HTML_SS_CLOSE()
      );
      return;
    }
  if (player_->isStopped()) {
    enterMediaUrl(url);
    return;
  }
  UrlDialog *d = annotationUrlDialog();
  d->setText(url);
  d->show();
}

void
MainWindow::enterMediaUrl(const QString &url)
{
  //if (downloadDialog_ && downloadDialog_->isAddingUrls()) {
  //  downloadDialog_->promptUrl(url);
  //  return;
  //}
  if (!hub_->isMediaTokenMode())
    return;
  if (url == dataManager_->token().url()) {
    emit message(tr("media URL is being played ") + ": "
        HTML_SS_OPEN(color:orange) + url + HTML_SS_CLOSE()
    );
    return;
  }
  UrlDialog *d = mediaUrlDialog();
  d->setText(url);
  d->show();
}

void
MainWindow::enterDownloadUrls(const QStringList &urls)
{ downloaderDelegate_->openUrls(urls); }

// - Close menu -

void
MainWindow::nothingAfterFinished()
{
  nothingAfterFinishedAct_->setChecked(true);
  sleepAfterFinishedAct_->setChecked(false);
  shutdownAfterFinishedAct_->setChecked(false);

  emit message(tr("do nothings after finished playing all files"));
}

void
MainWindow::sleepAfterFinished()
{
  nothingAfterFinishedAct_->setChecked(false);
  sleepAfterFinishedAct_->setChecked(true);
  shutdownAfterFinishedAct_->setChecked(false);
  emit message(tr("put the computer to sleep after finished playing all files"));
}

void
MainWindow::shutdownAfterFinished()
{
  nothingAfterFinishedAct_->setChecked(false);
  sleepAfterFinishedAct_->setChecked(false);
  shutdownAfterFinishedAct_->setChecked(true);
  emit message(tr("shutdown the computer after finished playing all files"));
}

void
MainWindow::promptSleep()
{
  if (!sleepAfterFinishedAct_->isChecked())
    return;
  static CountdownDialog *w = nullptr;
  if (!w) {
    w = new SleepDialog(this);
    AC_CONNECT_MESSAGES(w, this, Qt::AutoConnection);
    windows_.append(w);
  }
  DOUT("show sleep dialog");
  w->show();
}

void
MainWindow::promptShutdown()
{
  if (!shutdownAfterFinishedAct_->isChecked())
    return;
  static CountdownDialog *w = nullptr;
  if (!w) {
    w = new ShutdownDialog(this);
    AC_CONNECT_MESSAGES(w, this, Qt::AutoConnection);
    windows_.append(w);
  }
  DOUT("show shutdown dialog");
  w->show();
}

// - Proxy browser

void
MainWindow::openProxyBrowser()
{
  Settings::globalSettings()->sync();
  AcSettings::globalSettings()->sync();
  browserDelegate_->open();
  /*
  bool ok = false;
#ifdef Q_OS_MAC
  ok = QtMac::open(AC_BROWSER);
#elif defined(Q_OS_WIN)
  QString exe = QCoreApplication::applicationDirPath() + "/" AC_BROWSER;
  ok = QProcess::startDetached('"' + exe + '"');
#else
  ok = QProcess::startDetached(AC_BROWSER);
#endif // Q_OS_
  if (!ok) {
    emit warning(tr("failed to launch external browser"));
    QDesktopServices::openUrl(QUrl("http://google.com"));
  }
  */
}

// - Single window -

void
MainWindow::setMultipleWindowsEnabled(bool t)
{
  Settings::globalSettings()->setMultipleWindowsEnabled(t);
  Settings::globalSettings()->sync();
  AcSettings::globalSettings()->sync();
  if (t)
    emit message(tr("allow multiple player windows"));
  else
    emit message(tr("allow single player window"));
}

void
MainWindow::newWindow()
{
  DOUT("enter");
  Settings *s = Settings::globalSettings();
  s->sync();
  bool t = s->isMultipleWindowsEnabled();
  DOUT("is multiple window enabled =" << t);
  if (!t) {
    emit notification(tr("please enable Multiple Window from context menu"));
    return;
  }

  AcSettings::globalSettings()->sync();

  QProcess::startDetached(QCoreApplication::applicationFilePath());

  emit message(tr("new window launched"));
  DOUT("exit");
}

// - Aspect ratio -

void
MainWindow::setDefaultAspectRatio()
{
  if (player_->hasMedia())
    player_->clearAspectRatio();
}

void
MainWindow::setStandardAspectRatio()
{
  if (player_->hasMedia())
    player_->setAspectRatio("4:3");
}

void
MainWindow::setWideScreenAspectRatio()
{
  if (player_->hasMedia())
    player_->setAspectRatio("16:9");
}

// - Annotation thread view -

void
MainWindow::showUserAnalytics(qint64 userId)
{
  if (!userAnalyticsView_) {
    userAnalyticsView_ = new UserAnalyticsView(dataManager_, hub_, this);
    windows_.append(userAnalyticsView_);
  }
  userAnalyticsView_->setUserId(userId);
  userAnalyticsView_->show();
}

AnnotationAnalyticsView*
MainWindow::annotationAnalyticsView()
{
  if (!annotationAnalyticsView_) {
    annotationAnalyticsView_ = new AnnotationAnalyticsView(dataManager_, hub_, this);
    windows_.append(annotationAnalyticsView_);
    AC_CONNECT_MESSAGE(annotationAnalyticsView_, this, Qt::AutoConnection);
  }
  return annotationAnalyticsView_;
}

void
MainWindow::showAnnotationAnalytics()
{ annotationAnalyticsView()->refresh(); }

// - Save media -

void
MainWindow::setSaveMedia(bool t)
{
  player_->setBufferSaved(t);
  if (t)
    emit message(tr("buffered video will be saved"));
  else
    emit message(tr("buffered video will not be saved"));
}

void
MainWindow::saveMedia()
{
  setSaveMedia(true);
  if (player_->hasMedia())
    player_->saveBuffer();
}

// - Rubber band -

void
MainWindow::pauseAnnotationsAt(const QRect &rect)
{
  QPoint globalPos = mapToGlobal(rect.topLeft());
  QPoint pos = annotationView_->mapFromGlobal(globalPos);
  QRect map(pos, rect.size());
  annotationView_->pauseItems(map);
}

void
MainWindow::resumeAnnotationsAt(const QRect &rect)
{
  QPoint globalPos = mapToGlobal(rect.topLeft());
  QPoint pos = annotationView_->mapFromGlobal(globalPos);
  QRect map(pos, rect.size());
  annotationView_->resumeItems(map);
}

void
MainWindow::removeAnnotationsAt(const QRect &rect)
{
  QPoint globalPos = mapToGlobal(rect.topLeft());
  QPoint pos = annotationView_->mapFromGlobal(globalPos);
  QRect map(pos, rect.size());
  annotationView_->removeItems(map);
}

void
MainWindow::resumeAnnotations()
{
  emit message(tr("resume annotations"));
  annotationView_->resume();
}

// - Adjustment -

void
MainWindow::contrastUp()
{
  if (player_->isValid()) {
    qreal value = player_->contrast();
    value *= 1.1;
    player_->setContrast(value);
  }
}

void
MainWindow::contrastDown()
{
  if (player_->isValid()) {
    qreal value = player_->contrast();
    value /= 1.1;
    player_->setContrast(value);
  }
}

void
MainWindow::contrastReset()
{
  if (player_->isValid())
    player_->resetContrast();
}

void
MainWindow::brightnessUp()
{
  if (player_->isValid()) {
    qreal value = player_->brightness();
    value *= 1.02;
    player_->setBrightness(value);
  }
}

void
MainWindow::brightnessDown()
{
  if (player_->isValid()) {
    qreal value = player_->brightness();
    value /= 1.02;
    player_->setBrightness(value);
  }
}

void
MainWindow::brightnessReset()
{
  if (player_->isValid())
    player_->resetBrightness();
}

void
MainWindow::hueUp()
{
  if (player_->isValid()) {
    int value = player_->hue();
    value += 2;
    player_->setHue(value);
  }
}

void
MainWindow::hueDown()
{
  if (player_->isValid()) {
    int value = player_->hue();
    value -= 2;
    player_->setHue(value);
  }
}

void
MainWindow::hueReset()
{
  if (player_->isValid())
    player_->resetHue();
}

void
MainWindow::saturationUp()
{
  if (player_->isValid()) {
    qreal value = player_->saturation();
    value *= 1.1;
    player_->setSaturation(value);
  }
}

void
MainWindow::saturationDown()
{
  if (player_->isValid()) {
    qreal value = player_->saturation();
    value /= 1.1;
    player_->setSaturation(value);
  }
}

void
MainWindow::saturationReset()
{
  if (player_->isValid())
    player_->resetSaturation();
}

void
MainWindow::gammaUp()
{
  if (player_->isValid()) {
    qreal value = player_->gamma();
    value *= 1.1;
    player_->setGamma(value);
  }
}

void
MainWindow::gammaDown()
{
  if (player_->isValid()) {
    qreal value = player_->gamma();
    value /= 1.1;
    player_->setGamma(value);
  }
}

void
MainWindow::gammaReset()
{
  if (player_->isValid())
    player_->resetGamma();
}

// - Annotation transforms -

void
MainWindow::annotationScaleUp()
{
  if (hub_->isFullScreenWindowMode()) {
    qreal value = AnnotationSettings::globalSettings()->fullscreenScale();
    value *= 1.05;
    if (value < 10)
      AnnotationSettings::globalSettings()->setFullscreenScale(value);
  } else {
    qreal value = AnnotationSettings::globalSettings()->scale();
    value *= 1.05;
    if (value < 10)
      AnnotationSettings::globalSettings()->setScale(value);
  }
}

void
MainWindow::annotationScaleDown()
{
  if (hub_->isFullScreenWindowMode()) {
    qreal value = AnnotationSettings::globalSettings()->fullscreenScale();
    value /= 1.05;
    if (value > 0.1)
      AnnotationSettings::globalSettings()->setFullscreenScale(value);
  } else {
    qreal value = AnnotationSettings::globalSettings()->scale();
    value /= 1.05;
    if (value > 0.1)
      AnnotationSettings::globalSettings()->setScale(value);
  }
}

void
MainWindow::resetAnnotationScale()
{
  if (hub_->isFullScreenWindowMode())
    AnnotationSettings::globalSettings()->resetFullscreenScale();
  else
    AnnotationSettings::globalSettings()->resetScale();
}

void
MainWindow::annotationRotateUp()
{
  enum { delta = 15 };
  AnnotationSettings::globalSettings()->setRotation(
    AnnotationSettings::globalSettings()->rotation() + delta
  );
}

void
MainWindow::annotationRotateDown()
{
  enum { delta = 15 };
  AnnotationSettings::globalSettings()->setRotation(
    AnnotationSettings::globalSettings()->rotation() - delta
  );
}

void
MainWindow::increaseAnnotationOffset()
{
  enum { delta = 5 };
  AnnotationSettings::globalSettings()->setOffset(
    AnnotationSettings::globalSettings()->offset() + delta
  );
}

void
MainWindow::decreaseAnnotationOffset()
{
  enum { delta = 5 };
  AnnotationSettings::globalSettings()->setOffset(
    AnnotationSettings::globalSettings()->offset() - delta
  );
}


// - Cursor -

//void
//MainWindow::autoHideCursor()
//{
//  if (hub_->isFullScreenWindowMode() &&
//      player_->hasMedia() && player_->isPlaying()) {
//    if (underMouse())
//      hideCursor();
//    else
//      resetAutoHideCursor();
//  }
//}
//
//void
//MainWindow::resetAutoHideCursor()
//{
//#ifdef Q_OS_MAC
//  if (hub_->isFullScreenWindowMode() &&
//      player_->hasMedia() && player_->isPlaying())
//    autoHideCursorTimer_->start();
//  showCursor();
//#endif // Q_OS_MAC
//}
//
//void
//MainWindow::setCursorVisible(bool t)
//{
//  if (cursorVisible_ != t) {
//    cursorVisible_ = t;
//    // See: http://lists.trolltech.com/qt-interest/2005-11/thread01003-0.html
//    //qApp->lock();
//    if (cursorVisible_)
//      QApplication::restoreOverrideCursor();
//    else
//      QApplication::setOverrideCursor(Qt::BlankCursor);
//    //qApp->unlock();
//  }
//}

// - Gestures -

void
MainWindow::gestureEvent(QGestureEvent *e)
{
  DOUT("enter");
  if (QGesture *swipe = e->gesture(Qt::SwipeGesture))
    swipeGesture(static_cast<QSwipeGesture *>(swipe));
  else if (QGesture *pan = e->gesture(Qt::PanGesture))
    panGesture(static_cast<QPanGesture *>(pan));
  else if (QGesture *pinch = e->gesture(Qt::PinchGesture))
    pinchGesture(static_cast<QPinchGesture *>(pinch));
  DOUT("exit");
}

void
MainWindow::panGesture(QPanGesture *g)
{
  DOUT("enter");
  Q_UNUSED(g)
  //QPinchGesture::ChangeFlags f = g->changeFlags();
  //if (f & QPinchGesture::ScaleFactorChanged) {
  //  if (g->scaleFactor() > 1)
  //    zoomIn();
  //  else
  //    zoomOut();
  //}
  DOUT("exit");
}

void
MainWindow::pinchGesture(QPinchGesture *g)
{
  DOUT("enter");
  QPinchGesture::ChangeFlags f = g->changeFlags();
  if (f & QPinchGesture::ScaleFactorChanged) {
    if (g->scaleFactor() > 1)
      annotationScaleUp();
    else
      annotationScaleDown();
  }
  DOUT("exit");
}

void
MainWindow::swipeGesture(QSwipeGesture *g)
{
  DOUT("enter");
  Q_UNUSED(g)
  if (g->state() == Qt::GestureFinished) {
    uint h = g->horizontalDirection(),
         d = g->verticalDirection();
    if (h ^ d)
      switch (h | d) {
      case QSwipeGesture::Left:   backward(); break;
      case QSwipeGesture::Right:  forward(); break;
      case QSwipeGesture::Up:     previous(); break;
      case QSwipeGesture::Down:   next(); break;
      default: Q_ASSERT(0);
      }
  }
  DOUT("exit");
}

// - Search -

void
MainWindow::searchWithEngine(int engine, const QString &key)
{
  if (engine >= 0 && engine < searchEngines_.size()) {
    const SearchEngine *e = searchEngines_[engine];
    if (e) {
      QString url = e->search(key);
      if (!url.isEmpty()) {
        emit message(tr("opening") + ": " HTML_SS_OPEN(color:orange) + url + HTML_SS_CLOSE());
        browserDelegate_->openUrl(url);
      }
    }
  }
}

// - Dialogs -

void
MainWindow::showPreferences()
{
  if (!preferences_) {
    preferences_ = new Preferences(this);
    windows_.append(preferences_);
  }
  preferences_->show();
  preferences_->raise();
}

// - Paint -

void
MainWindow::setRippleEnabled(bool t)
{
  if (t == rippleEnabled_)
    return;
  if (t)
    AcUi::globalInstance()->removeWindowBackground(this);
  else
    AcUi::globalInstance()->setWindowBackground(this, false); // persistent is false
  rippleEnabled_ = t;
  if (t) {
    if (rippleTimer_ && !rippleTimer_->isActive())
      rippleTimer_->start();
  } else {
    if (rippleTimer_)
      rippleTimer_->stop();
    if (rippleFilter_)
      rippleFilter_->clear();
  }
}

void
MainWindow::paintEvent(QPaintEvent *e)
{
  if (!rippleEnabled_) {
    Base::paintEvent(e);
    return;
  }
  //DOUT("enter");
  if (!rippleTimer_) {
    rippleTimer_ = new QTimer(this);
    rippleTimer_->setInterval(1000 / 30); // 30fps
    connect(rippleTimer_, SIGNAL(timeout()), SLOT(repaint()));
    rippleTimer_->start();
  }
  if (!rippleFilter_)
    rippleFilter_ = new RippleImageFilter(this);

  static const QImage image(AcUi::globalInstance()->backgroundImage());
  QPainter painter(this);
  if (rippleFilter_->needsDisplay())
    rippleFilter_->drawImage(painter, image);
  else
    painter.drawImage(0, 0, image);
  //DOUT("exit");
}

// - Magnifier -

void
MainWindow::setMagnifierVisible(bool visible)
{
  if (!magnifier_) {
    magnifier_ = new Magnifier(this);
    magnifier_->setWidget(QApplication::desktop());
    new QShortcut(QKeySequence("CTRL+E"), magnifier_, SLOT(hide()));
    windows_.append(magnifier_);

    QRect r = QApplication::desktop()->geometry();
    QPoint pos = r.bottomRight() - magnifier_->rect().bottomRight();
    magnifier_->move(pos);
  }
  if (visible) {
    magnifier_->show();
    magnifier_->raise();
  } else
    magnifier_->fadeOut();
}

void
MainWindow::toggleMagnifierVisible()
{
  bool v = magnifier_ && magnifier_->isVisible();
  setMagnifierVisible(!v);
}

void
MainWindow::updateAnnotationMetaVisible()
{
  bool v = hub_->isMediaTokenMode() && hub_->isFullScreenWindowMode() && AnnotationSettings::globalSettings()->preferFloat() && player_->isPaused() ||
           hub_->isSignalTokenMode() && hub_->isPaused() ||
           embeddedPlayer_->isVisible() && !embeddedCanvas_->isVisible() && AnnotationSettings::globalSettings()->preferFloat() &&
             !(hub_->isMediaTokenMode() && !hub_->isFullScreenWindowMode());
  annotationView_->setItemMetaVisible(v);
}

// - Channels -

void
MainWindow::setLeftChannel()
{
  if (player_->hasMedia())
    player_->setAudioChannel(Player::LeftChannel);
}

void
MainWindow::setRightChannel()
{
  if (player_->hasMedia())
    player_->setAudioChannel(Player::RightChannel);
}

void
MainWindow::setStereoChannel()
{
  if (player_->hasMedia())
    player_->setAudioChannel(Player::StereoChannel);
}

void
MainWindow::setReverseStereoChannel()
{
  if (player_->hasMedia())
    player_->setAudioChannel(Player::ReverseStereoChannel);
}

void
MainWindow::setDolbysChannel()
{
  if (player_->hasMedia())
    player_->setAudioChannel(Player::DolbysChannel);
}

void
MainWindow::showAudioDelayDialog()
{
  static AudioDelayDialog *w = nullptr;
  if (!w) {
    w = new AudioDelayDialog(this);
    connect(w, SIGNAL(timeChanged(qint64)), player_, SLOT(setAudioDelay(qint64)));
    windows_.append(w);
  }
  if (player_->hasMedia())
    w->setTime(player_->audioDelay());
  else
    w->resetTime();
  w->show();
  w->raise();
}

// - Save Settings -

void
MainWindow::saveSettings()
{
  DOUT("enter");
  //DOUT("enter: inet mutex locked");
  //QMutexLocker locker(&inetMutex_);

  Settings *settings = Settings::globalSettings();
  AcSettings *ac = AcSettings::globalSettings();
  AnnotationSettings *annotationSettings = AnnotationSettings::globalSettings();
  AcUi *ui = AcUi::globalInstance();

  //ac->dispose();

  //updateRecent();
#ifdef Q_OS_WIN
  //ac->setAeroEnabled(ui->isAeroEnabled());
#endif // Q_OS_WIN

  ac->setMenuThemeEnabled(ui->isMenuEnabled());

  settings->setRecentFiles(recentFiles_);
  settings->setRecentTitles(recentUrlTitles_);
  //settings->setGameEncodings(recentGameEncodings_, RECENT_COUNT);

  settings->setRecentPath(recentPath_);

  ac->setThemeId(ui->theme());

  settings->setMenuBarVisible(menuBar()->isVisible());

  settings->setAnnotationFilterEnabled(annotationFilter_->isEnabled());
  settings->setBlockedKeywords(annotationFilter_->blockedTexts());
  settings->setBlockedUserNames(annotationFilter_->blockedUserAliases());
  settings->setAnnotationLanguages(annotationFilter_->languages());
  //settings->setAnnotationCountHint(annotationFilter_->annotationCountHint());

  settings->setPreferLocalDatabase(dataServer_->preferCache());

  settings->setTranslationServices(translator_->services());

  settings->setSubtitleColor(subtitleColor());

  settings->setShowLibrary(libraryView_->autoRun());
  settings->setLibraryView(libraryView_->viewIndex());
  settings->setAnnotationBandwidthLimited(annotationView_->isItemCountLimited());
  settings->setAnnotationEffect(annotationView_->renderHint());
  settings->setTranslateSelection(annotationView_->isTranslateWordEnabled());
  settings->setAnnotationScale(annotationSettings->scale());
  settings->setAnnotationFullscreenScale(annotationSettings->fullscreenScale());
  settings->setAnnotationOpacityFactor(annotationSettings->opacityFactor());
  settings->setAnnotationBackgroundOpacityFactor(annotationSettings->backgroundOpacityFactor());
  settings->setAnnotationOffset(annotationSettings->offset());
  settings->setAnnotationFontFamily(annotationSettings->fontFamily());
  settings->setAnnotationJapaneseFontFamily(annotationSettings->japaneseFontFamily());
  settings->setAnnotationChineseFontFamily(annotationSettings->chineseFontFamily());
  settings->setAnnotationAvatarVisible(annotationSettings->isAvatarVisible());
  settings->setAnnotationMetaVisible(annotationSettings->isMetaVisible());
  settings->setPreferFloatAnnotation(annotationSettings->preferFloat());
  settings->setAnnotationOutlineColor(annotationSettings->outlineColor());
  settings->setAnnotationHighlightColor(annotationSettings->highlightColor());
  settings->setSubtitleOutlineColor(annotationSettings->subtitleColor());
  settings->setPreferTraditionalChinese(annotationSettings->preferTraditionalChinese());
  settings->setAnnotationPositionResolution(annotationSettings->positionResolution());
  settings->setAnnotationSpeedup(annotationSettings->speedup());
  settings->setSubtitleOnTop(annotationSettings->isSubtitleOnTop());

  settings->setBufferedMediaSaved(player_->isBufferSaved());
  settings->setAnnotationFileSaved(toggleSaveAnnotationFileAct_->isChecked());

  settings->setTranslateEnabled(isTranslateEnabled());

  settings->setEmbeddedPlayerOnTop(embeddedPlayer_->isOnTop());
  //settings->setPlayerLabelEnabled(embeddedCanvas_->isEnabled());

  //settings->setAutoPlayNext(isAutoPlayNext());

  settings->setAutoSubmit(submit_);

  settings->setWindowOnTop(isWindowOnTop());

  //if (siteAccountView_) { // Update account settings iff it is modified
  //  ac->setNicovideoAccount(siteAccountView_->nicovideoAccount().username,
  //                          siteAccountView_->nicovideoAccount().password);
  //  ac->setBilibiliAccount(siteAccountView_->bilibiliAccount().username,
  //                         siteAccountView_->bilibiliAccount().password);
  //}

  if (player_->isValid()) {
    settings->setHue(player_->hue());
    settings->setContrast(player_->contrast());
    settings->setSaturation(player_->saturation());
    settings->setBrightness(player_->brightness());
    settings->setGamma(player_->gamma());
  }

  settings->setAspectRatioHistory(aspectRatioHistory_);
  settings->setSubtitleHistory(subtitleHistory_);
  settings->setAudioTrackHistory(audioTrackHistory_);
  settings->setAudioChannelHistory(audioChannelHistory_);
  settings->setPlayPosHistory(playPosHistory_);

#ifdef WITH_WIN_ATLAS
  ac->setAtlasLocation(atlas_->location());
  settings->setAtlasEnabled(isAtlasEnabled());
#endif // WITH_WIN_ATLAS

#ifdef AC_ENABLE_GAME
  settings->setAppLocaleEnabled(isAppLocaleEnabled());
  settings->setGameTextVisible(isGameTextVisible());
  settings->setGameTextColorful(toggleGameTextColorfulAct_->isChecked());
  settings->setGameTextResizable(toggleGameTextResizableAct_->isChecked());

  gameLibrary_->save();
#endif // AC_ENABLE_GAME

  mediaLibrary_->save();

  settings->sync();
  ac->sync();

  //DOUT("exit: inet mutex unlocked");
  DOUT("exit");
}

void
MainWindow::saveSettingsLater()
{ saveSettingsTimer_->start(); }

// - Translation -

void
MainWindow::toggleFresheyeTranslator(bool t)
{
  translator_->setService(TranslatorManager::Fresheye, t);
  if (!translator_->hasServices())
    translator_->setServices(DEFAULT_TRANSLATORS);
  if (t)
    emit messageOnce(tr("freshEYE Honyaku On"));
  else
    emit messageOnce(tr("freshEYE Honyaku Off"));
}

void
MainWindow::toggleYahooTranslator(bool t)
{
  translator_->setService(TranslatorManager::Yahoo, t);
  if (!translator_->hasServices())
    translator_->setServices(DEFAULT_TRANSLATORS);
  if (t)
    emit messageOnce(tr("Yahoo! Honyaku On"));
  else
    emit messageOnce(tr("Yahoo! Honyaku Off"));
}

void
MainWindow::toggleOcnTranslator(bool t)
{
  translator_->setService(TranslatorManager::Ocn, t);
  if (!translator_->hasServices())
    translator_->setServices(DEFAULT_TRANSLATORS);
  if (t)
    emit messageOnce(tr("OCN Honyaku On"));
  else
    emit messageOnce(tr("OCN Honyaku Off"));
}

void
MainWindow::toggleSystranTranslator(bool t)
{
  translator_->setService(TranslatorManager::Systran, t);
  if (!translator_->hasServices())
    translator_->setServices(DEFAULT_TRANSLATORS);
  if (t)
    emit messageOnce(tr("SYSTRAN Translator On"));
  else
    emit messageOnce(tr("SYSTRAN Translator Off"));
}

void
MainWindow::toggleSdlTranslator(bool t)
{
  translator_->setService(TranslatorManager::Sdl, t);
  if (!translator_->hasServices())
    translator_->setServices(DEFAULT_TRANSLATORS);
  if (t)
    emit messageOnce(tr("SDL Translator On"));
  else
    emit messageOnce(tr("SDL Translator Off"));
}

void
MainWindow::toggleInfoseekTranslator(bool t)
{
  translator_->setService(TranslatorManager::Infoseek, t);
  if (!translator_->hasServices())
    translator_->setServices(DEFAULT_TRANSLATORS);
  if (t)
    emit messageOnce(tr("Infoseek Honyaku On"));
  else
    emit messageOnce(tr("Infoseek Honyaku Off"));
}

void
MainWindow::toggleNiftyTranslator(bool t)
{
  translator_->setService(TranslatorManager::Nifty, t);
  if (!translator_->hasServices())
    translator_->setServices(DEFAULT_TRANSLATORS);
  if (t)
    emit messageOnce(tr("@nifty Honyaku On"));
  else
    emit messageOnce(tr("@nifty Honyaku Off"));
}

void
MainWindow::toggleExciteTranslator(bool t)
{
  translator_->setService(TranslatorManager::Excite, t);
  if (!translator_->hasServices())
    translator_->setServices(DEFAULT_TRANSLATORS);
  if (t)
    emit messageOnce(tr("Excite Honyaku On"));
  else
    emit messageOnce(tr("Excite Honyaku off"));
}

void
MainWindow::toggleGoogleTranslator(bool t)
{
  translator_->setService(TranslatorManager::Google, t);
  if (!translator_->hasServices())
    translator_->setServices(DEFAULT_TRANSLATORS);
  if (t)
    emit messageOnce(tr("Google Translator On"));
  else
    emit messageOnce(tr("Google Translator Off"));
}

void
MainWindow::toggleMicrosoftTranslator(bool t)
{
  translator_->setService(TranslatorManager::Microsoft, t);
  if (!translator_->hasServices())
    translator_->setServices(DEFAULT_TRANSLATORS);
  if (t)
    emit messageOnce(tr("Microsoft Translator On"));
  else
    emit messageOnce(tr("Microsoft Translator Off"));
}

void
MainWindow::toggleRomajiTranslator(bool t)
{
  translator_->setService(TranslatorManager::Romaji, t);
  if (!translator_->hasServices())
    translator_->setServices(DEFAULT_TRANSLATORS);
  if (t)
    emit messageOnce(tr("Romaji On"));
  else
    emit messageOnce(tr("Romaji Off"));
}

// - Translator -

void
MainWindow::showTranslator()
{ translatorDelegate_->show(); }

void
MainWindow::translateUsingTranslator(const QString &text)
{
  enum { cooldown = 500 }; // half a second
  if (!text.isEmpty()) {
    static qint64 lastTime = 0;
    static QString lastText;
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (now < lastTime + cooldown && text == lastText)
      lastTime = now;
    else {
      lastTime = now;
      lastText = text;
      translatorDelegate_->translate(text);
    }
  }
}

// - Library -

void
MainWindow::showLibrary()
{
  if (!libraryWindow_) {
    libraryWindow_ = new MediaLibraryView(mediaLibrary_, this);
    libraryWindow_->setViewIndex(libraryView_->viewIndex());
    connect(libraryWindow_, SIGNAL(openRequested(QString)), SLOT(openSource(QString)));
    connect(libraryWindow_, SIGNAL(toggled()), SLOT(toggleMainLibrary()));
    connect(libraryWindow_, SIGNAL(showGameRequested(QString)), SLOT(showGameWithDigest(QString)));
    connect(libraryWindow_, SIGNAL(autoRunChanged(bool)), libraryView_, SLOT(setAutoRun(bool)));
    connect(libraryView_, SIGNAL(autoRunChanged(bool)), libraryWindow_, SLOT(setAutoRun(bool)));
#ifdef AC_ENABLE_GAME
  connect(libraryWindow_, SIGNAL(syncGameRequested()), SLOT(openCurrentGame()));
#endif // AC_ENABLE_GAME
    windows_.append(libraryWindow_);
  }
  libraryWindow_->setAutoRun(libraryView_->autoRun());
  libraryWindow_->show();
  libraryWindow_->raise();
}

void
MainWindow::showGameWithDigest(const QString &digest)
{
  static GameView *w = nullptr;
  if (!w) {
    w = new GameView(gameLibrary_, this);
    windows_.append(w);
  }

  w->setDigest(digest);

  w->show();
  w->raise();
}

void
MainWindow::toggleMainLibrary()
{
  bool v = !libraryView_->isVisible();
  bool adjust = !hub_->isFullScreenWindowMode() && !player_->isStopped();
  if (adjust && !v) {
    QSize sz = size();
    sz.rwidth() -= libraryView_->width();
    if (sz.isValid())
      resize(sz);
  }
  libraryView_->setVisible(v);
  if (adjust && v) {
    QSize sz = size();
    sz.rwidth() += libraryView_->width();
    if (sz.isValid())
      resize(sz);
  }
}

void
MainWindow::toggleMediaLibrary()
{
  bool visible = libraryWindow_ && libraryWindow_->isVisible();
  if (visible)
    libraryWindow_->fadeOut();
  else
    showLibrary();
}

void
MainWindow::toggleLibrary()
{
  if (isVisible())
    toggleMainLibrary();
  else
    toggleMediaLibrary();
}

void
MainWindow::showMainLibrary()
{
  if (mediaLibrary_->exists())
    libraryView_->show();
}

// - Atlas Translator -

#ifdef WITH_WIN_ATLAS

bool
MainWindow::isAtlasEnabled() const
{ return toggleAtlasEnabledAct_->isChecked(); }

void
MainWindow::createAtlas()
{
  atlas_ = new Atlas(this);
  QString location = AcSettings::globalSettings()->atlasLocation();
  if (location.isEmpty() || !Atlas::isValidLocation(location))
    location = Atlas::findLocation();

  if (Atlas::isValidLocation(location)) {
    atlas_->setLocation(location);
    AcSettings::globalSettings()->setAtlasLocation(location);
  }
}

void
MainWindow::showAtlasAdditionalTranslation(const QString &text)
{
  if (text.isEmpty())
    return;
  showAtlasTranslation(CORE_CMD_VIEW_FLOAT " " + text);
}

void
MainWindow::showAtlasTranslation(const QString &text)
{
  if (text.isEmpty())
    return;
  QString prefix = CORE_CMD_LATEX_SMALL " ";
  if (isTranslateEnabled() && translator_->services())
    prefix.append("ATLAS: ");
  showSubtitle(text, Traits::English, prefix);
}

void
MainWindow::translateWithAtlas(const QString &text, int role)
{
  if (text.isEmpty() || !isAtlasEnabled())
    return;
  QString t = atlas_->translate(text);
  if (!t.isEmpty()) {
    bool extra = role == TextThread::SupportRole;
    if (extra)
      showAtlasAdditionalTranslation(t);
    else
      showAtlasTranslation(t);
  }
}
#endif // WITH_WIN_ATLAS

// EOF

/*
//! [updateFormatsTable() part1]
void DropSiteWindow::updateFormatsTable(const QMimeData *mimeData)
{
    formatsTable->setRowCount(0);
    if (!mimeData)
        return;
//! [updateFormatsTable() part1]

//! [updateFormatsTable() part2]
    foreach (const QString &format, mimeData->formats()) {
        QTableWidgetItem *formatItem = new QTableWidgetItem(format);
        formatItem->setFlags(Qt::ItemIsEnabled);
        formatItem->setTextAlignment(Qt::AlignTop | Qt::AlignLeft);
//! [updateFormatsTable() part2]

//! [updateFormatsTable() part3]
        QString text;
        if (format == "text/plain") {
            text = mimeData->text().simplified();
        } else if (format == "text/html") {
            text = mimeData->html().simplified();
        } else if (format == "text/uri-list") {
            QList<QUrl> urlList = mimeData->urls();
            for (int i = 0; i < urlList.size() && i < 32; ++i)
                text.append(urlList[i].toString() + " ");
        } else {
            QByteArray data = mimeData->data(format);
            for (int i = 0; i < data.size() && i < 32; ++i) {
                QString hex = QString("%1").arg(uchar(data[i]), 2, 16,
                                                QChar('0'))
                                           .toUpper();
                text.append(hex + " ");
            }
        }
//! [updateFormatsTable() part3]

//! [updateFormatsTable() part4]
        int row = formatsTable->rowCount();
        formatsTable->insertRow(row);
        formatsTable->setItem(row, 0, new QTableWidgetItem(format));
        formatsTable->setItem(row, 1, new QTableWidgetItem(text));
    }

    formatsTable->resizeColumnToContents(0);
}
//! [updateFormatsTable() part4]
*/
