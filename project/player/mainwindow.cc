// mainwindow.cc
// 6/30/2011

#include "mainwindow.h"
#include "mainwindow_p.h"
#include "application.h"
#include "preferences.h"

#include "datamanager.h"
#include "dataserver.h"
#include "logger.h"
#include "eventlogger.h"
#include "grabber.h"
#include "osdconsole.h"
#include "osdwindow.h"
#include "playerui.h"
#include "videoview.h"
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
#include "tokenview.h"
#include "mediainfoview.h"
//#include "commentview.h"
#include "tray.h"
#include "annotationbrowser.h"
#include "stylesheet.h"
#include "rc.h"
#include "tr.h"
#include "translatormanager.h"
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
#include "livedialog.h"
#include "syncdialog.h"
#include "mediaurldialog.h"
#include "suburldialog.h"
#include "shutdowndialog.h"
#include "sleepdialog.h"
#include "userview.h"
#include "useranalyticsview.h"
#include "global.h"
#ifdef Q_WS_WIN
#  include "windowsregistry.h"
#endif // Q_WS_WIN
#ifdef USE_MODE_SIGNAL
#  include "signalview.h"
#  include "messageview.h"
#  include "processview.h"
#  include "messagehandler.h"
#endif // USE_MODE_SIGNAL
#include "module/annotdb/annotdb.h"
#include "module/player/player.h"
#include "module/player/playerdefs.h"
#include "module/annotcodec/annotationcodecmanager.h"
#include "module/animation/fadeanimation.h"
#include "module/mrlanalysis/mrlanalysis.h"
#include "module/mrlresolver/mrlresolvermanager.h"
#include "module/mrlresolver/mrlresolversettings.h"
#include "module/translator/translator.h"
#include "module/download/downloader.h"
#include "module/searchengine/searchenginerc.h"
#include "module/magnifier/magnifier.h"
#include "module/qtext/algorithm.h"
#include "module/qtext/string.h"
#include "module/qtext/actionwithid.h"
#include "module/qtext/countdowntimer.h"
#include "module/qtext/datetime.h"
#include "module/qtext/htmltag.h"
#include "module/qtext/rubberband.h"
#include "module/mediacodec/flvcodec.h"
#include "module/imagefilter/rippleimagefilter.h"
#ifdef WITH_MODULE_SERVERAGENT
#  include "module/serveragent/serveragent.h"
#endif // WITH_MODULE_SERVERAGENT
#ifdef WITH_MODULE_CLIENTAGENT
#  include "module/clientagent/clientagent.h"
#endif // WITH_MODULE_CLIENTAGENT
#ifdef WITH_MODULE_DOLL
#  include "module/doll/doll.h"
#endif // WITH_MODULE_DOLL
#ifdef WITH_WIN_HOOK
#  include "win/hook/hook.h"
#endif // WITH_WIN_HOOK
#ifdef WITH_WIN_MOUSEHOOK
#  include "win/mousehook/mousehook.h"
#endif // WITH_WIN_MOUSEHOOK
//#ifdef WITH_WIN_DWM
//#  include "win/dwm/dwm.h"
//#endif // WITH_WIN_DWM
#ifdef WITH_WIN_TEXTHOOK
#  include "win/texthook/texthook.h"
#endif // WITH_WIN_TEXTHOOK
#ifdef Q_WS_WIN
#  include "win/qtwin/qtwin.h"
#  include "win/qtwinnt/qtwinnt.h"
#endif // Q_WS_WIN
#ifdef WITH_WIN_PICKER
#  include "win/picker/picker.h"
#endif // WITH_WIN_PICKER
#ifdef Q_OS_MAC
#  include "mac/qtmac/qtmac.h"
//#  include "mac/qtstep/qtstep.h"
//#  include "mac/qtvlc/qtvlc.h"
#endif // Q_OS_MAC
#ifdef Q_WS_X11
#  include "unix/qtx/qtx.h"
#endif // Q_WS_X11
#ifdef Q_OS_UNIX
#  include "unix/qtunix/qtunix.h"
#endif // Q_OS_UNIX
#include "project/common/acabout.h"
#include "project/common/acbrowser.h"
#include "project/common/acdownloader.h"
#include "project/common/acpaths.h"
#include "project/common/acplayer.h"
#include "project/common/acsettings.h"
#include "project/common/acui.h"
#include "module/annotcloud/annottag.h"
#include "module/annotcloud/annothtml.h"
#include <QtGui>
#include <QtNetwork/QNetworkAccessManager>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/typeof/typeof.hpp>
#include <ctime>

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses around && within ||
#endif // __GNUC__

#ifdef Q_OS_MAC
#  define K_CMD         "CTRL"
#  define K_ALT         "CTRL+ALT"
#  define K_CTRL        "cmd"
#  define K_SHIFT       "shift"
#  define K_OPT         "opt"
#else
#  define K_CMD         "ALT"
#  define K_ALT         "ALT"
#  define K_CTRL        "Ctrl"
#  define K_SHIFT       "Shift"
#  define K_OPT         "Alt"
#endif // Q_OS_MAC

using namespace AnnotCloud;
using namespace Logger;

#define DEBUG "mainwindow"
#include "module/debug/debug.h"

enum { DEFAULT_LIVE_INTERVAL = 3000 }; // 3 seconds
enum { HISTORY_SIZE = 100 };   // Size of playPos/Sub/AudioTrack history
enum { HOLD_TIMEOUT = 2000 };   // Size of playPos/Sub/AudioTrack history
enum { RECENT_COUNT = 39 };

#define PREFERRED_WINDOW_SIZE   QSize(840, 480)
#define PREFERRED_MIN_WIDTH     400
#define PREFERRED_MIN_HEIGHT    300

// - Focus -

void
MainWindow::onFocusedWidgetChanged(QWidget *w_old, QWidget *w_new)
{
  Q_UNUSED(w_old);
  Q_UNUSED(w_new);
//#ifdef Q_WS_WIN
//  // When EmbeddedPlayer's lineEdit cleared its focus.
//  if (!w_new && w_old == embeddedPlayer_->inputComboBox())
//    QtWin::sendMouseClick(QPoint(0, 0), Qt::LeftButton);
//#else
//  Q_UNUSED(w_old);
//  Q_UNUSED(w_new);
//#endif // Q_WS_WIN
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

  player_->setDownloadPath(G_PATH_DOWNLOADS);

#ifdef Q_WS_WIN
  player_->setMouseEventEnabled();
#endif // Q_WS_WIN

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
  DOUT("exit");
}

void
MainWindow::setupWindowStyle()
{
#ifdef Q_WS_WIN
  if (!AcUi::isAeroAvailable())
#endif // Q_WS_WIN
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

MainWindow::MainWindow(bool unique, QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f), disposed_(false), submit_(true),
    liveInterval_(DEFAULT_LIVE_INTERVAL),
    preferences_(0), tray_(0), magnifier_(0),
    tokenView_(0),
    annotationAnalyticsView_(0), userAnalyticsView_(0), annotationBrowser_(0), annotationEditor_(0), blacklistView_(0), backlogDialog_(0), consoleDialog_(0),
    annotationCountDialog_(0), deviceDialog_(0), helpDialog_(0), loginDialog_(0), liveDialog_(0),
    processPickDialog_(0), seekDialog_(0), syncDialog_(0), windowPickDialog_(0),
    mediaUrlDialog_(0), annotationUrlDialog_(0), mediaInfoView_(0), userView_(0),
    dragPos_(BAD_POS), windowModeUpdateTime_(0),
    tokenType_(0), recentSourceLocked_(false), cancelContextMenu_(false),
    rippleEnabled_(false), rippleFilter_(0), rippleTimer_(0),
    preferredSubtitleTrack_(0), preferredAudioTrack_(0), lastTrackedWindow_(0)
{
  DOUT("unique =" << unique);
#ifndef Q_WS_MAC
  // Broken on Windows 7
  //if (Settings::globalSettings()->isWindowOnTop())
  //  setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
#endif // Q_WS_MAC

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
  aspectRatioHistory_= settings->aspectRatioHistory();

  setTranslateEnabled(settings->isTranslateEnabled());
  setSubtitleOnTop(settings->isSubtitleOnTop());

  embeddedPlayer_->setOnTop(settings->isEmbeddedPlayerOnTop());
  //embeddedCanvas_->setEnabled(settings->isPlayerLabelEnabled());

  annotationView_->setItemCountLimited(settings->isAnnotationBandwidthLimited());
  annotationView_->setRenderHint(settings->annotationEffect());
  AnnotationSettings::globalInstance()->setScale(settings->annotationScale());
  AnnotationSettings::globalInstance()->setOffset(settings->annotationOffset());
  AnnotationSettings::globalInstance()->setAvatarVisible(settings->isAnnotationAvatarVisible());
  AnnotationSettings::globalInstance()->setPreferMotionless(settings->preferMotionlessAnnotation());

  dataServer_->setPreferLocal(settings->preferLocalDatabase());

  annotationFilter_->setEnabled(settings->isAnnotationFilterEnabled());
  annotationFilter_->setBlockedTexts(settings->blockedKeywords());
  annotationFilter_->setBlockedUserAliases(settings->blockedUserNames());
  //annotationFilter_->setAnnotationCountHint(settings->annotationCountHint());
  {
    qint64 l = settings->annotationLanguages();
    annotationFilter_->setLanguages(l);

    toggleAnnotationLanguageToAnyAct_->setChecked(l & Traits::AnyLanguageBit);
    toggleAnnotationLanguageToUnknownAct_->setChecked(l & Traits::UnknownLanguageBit);
    toggleAnnotationLanguageToEnglishAct_->setChecked(l & Traits::EnglishBit);
    toggleAnnotationLanguageToJapaneseAct_->setChecked(l & Traits::JapaneseBit);
    toggleAnnotationLanguageToChineseAct_->setChecked(l & Traits::ChineseBit);
    toggleAnnotationLanguageToKoreanAct_->setChecked(l & Traits::KoreanBit);
    invalidateAnnotationLanguages();
  }

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

#ifdef Q_WS_WIN
  //connect(annotationView_, SIGNAL(posChanged()), SLOT(ensureWindowOnTop()));
#endif // Q_WS_WIN

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

  // Enable keyboard shortcuts
  //updateContextMenu();

  //grabGesture(Qt::PanGesture);
  grabGesture(Qt::SwipeGesture);
  grabGesture(Qt::PinchGesture);

  if (unique)
    appServer_->start();
}

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
  connect(grabber_, SIGNAL(message(QString)), SLOT(showMessage(QString)));
  connect(grabber_, SIGNAL(warning(QString)), SLOT(warn(QString)));


  // Server agents
  server_ = new ServerAgent(this);
#ifdef WITH_MODULE_CLIENTAGENT
  client_ = new ClientAgent(this);

  server_->setClientAgent(client_);
  client_->setServerAgent(server_);
#endif // WITH_MODULE_CLIENTAGENT

  // Player
  player_ = new Player(this);

  // Signal hub
  hub_ = new SignalHub(player_, this);

  // Logger
  logger_ = new EventLogger(player_, hub_, this);

  // Caches
  cache_ = new AnnotationDatabase(this);
  if (unique && !cache_->open(G_PATH_CACHEDB))
    cache_->clear();
  queue_ = new AnnotationDatabase(this);
  if (unique && !queue_->open(G_PATH_QUEUEDB))
    queue_->clear();

  // Data server
  dataServer_ = new DataServer(hub_, server_, cache_, queue_, this);

  // Data manager
  dataManager_ = new DataManager(this);

  // Clipboard monitor
  clipboardMonitor_ = new ClipboardMonitor(this);
  clipboardMonitor_->setEnabled(false);

  // Mrl resolver
  mrlResolver_ = new MrlResolverManager(this);

  // Players and graphic views
  videoView_ = new VideoView(this);
  osdWindow_ = new OsdWindow(this);
  //windows_.append(osdWindow_);

  annotationView_ = new AnnotationGraphicsView(hub_, dataManager_, player_, videoView_, osdWindow_);
  annotationView_->setFullScreenView(osdWindow_);

  // Rubber bands
  attractRubberBand_ = new QtExt::CircularRubberBand(QRubberBand::Line, annotationView_);
  expelRubberBand_ = new QtExt::CircularRubberBand(QRubberBand::Line, annotationView_);

  pauseRubberBand_ = new QtExt::MouseRubberBand(QRubberBand::Rectangle, annotationView_);
  resumeRubberBand_ = new QtExt::MouseRubberBand(QRubberBand::Rectangle, annotationView_);
  removeRubberBand_ = new QtExt::MouseRubberBand(QRubberBand::Rectangle, annotationView_);

  pauseRubberBand_->setColor(Qt::cyan);
  removeRubberBand_->setColor(Qt::red);

  attractRubberBand_->setColor(Qt::cyan);
  expelRubberBand_->setColor(Qt::red);
  attractRubberBand_->setRadius(400);
  expelRubberBand_->setRadius(400);

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
  osdWindow_->setEventListener(annotationView_);

  embeddedPlayer_ = new EmbeddedPlayerUi(hub_, player_, server_, dataManager_);
  embeddedCanvas_ = embeddedPlayer_->canvas();

//#ifdef Q_OS_MAC // remove background in Osd player
//  embeddedPlayer_->setStyleSheet(
//    SS_BEGIN(QWidget)
//      SS_TRANSPARENT
//    SS_END
//  );
//#endif // Q_OS_MAC

  globalOsdConsole_ = new OsdConsole(annotationView_);
  globalOsdConsole_->setAutoClearInterval(G_CONSOLE_AUTOClEAR_TIMEOUT);
  OsdConsole::setGlobalInstance(globalOsdConsole_);

  // Show OSD window
  osdWindow_->showInOsdMode(); // this must go after all children of osdWindow_ are created
  embeddedPlayer_->setParent(osdWindow_); // Set parent after osdWindow_ become visible.
  embeddedPlayer_->setAutoHideEnabled();
  embeddedPlayer_->setContainerWidget(this);

  annotationView_->updateGeometry();
  annotationView_->show();
  globalOsdConsole_->show();

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
  translator_ = new Translator(this);

  // Dialogs
  //userPanel_ = new UserPanel(this);
  //userPanel_->hide(); // TODO!!!!!!

#ifdef USE_MODE_SIGNAL
  messageHandler_ = new MessageHandler(this);
  signalView_ = new SignalView(this);
  windows_.append(signalView_);

  recentMessageView_ = new MessageView(this);
  windows_.append(recentMessageView_);
#endif // USE_MODE_SIGNAL

#ifdef WITH_MODULE_DOLL
  doll_ = new Doll(this);
  connect(doll_, SIGNAL(said(QString)), SLOT(respond(QString)));
#endif // WITH_MODULE_DOLL

  holdTimer_ = new QTimer(this);
  holdTimer_->setInterval(HOLD_TIMEOUT);
  holdTimer_->setSingleShot(true);
  connect(holdTimer_, SIGNAL(timeout()), SLOT(toggleMagnifierVisible()));

  windowStaysOnTopTimer_ = new QTimer(this);
  windowStaysOnTopTimer_->setInterval(G_WINDOWONTOP_TIMEOUT);
  connect(windowStaysOnTopTimer_, SIGNAL(timeout()), SLOT(setWindowOnTop()));

  //autoHideCursorTimer_ = new QTimer(this);
  //autoHideCursorTimer_->setInterval(G_AUTOHIDE_TIMEOUT);
  //autoHideCursorTimer_->setSingleShot(true);
  //connect(autoHideCursorTimer_, SIGNAL(timeout()), SLOT(autoHideCursor()));

  liveTimer_ = new QTimer(this);
  connect(liveTimer_, SIGNAL(timeout()), SLOT(updateLiveAnnotations()));

  loadSubtitlesTimer_ = new QtExt::CountdownTimer;
  loadSubtitlesTimer_->setInterval(3000); // 3 seconds

  resumePlayTimer_ = new QtExt::CountdownTimer;
  resumePlayTimer_->setInterval(3000); // 3 seconds

  resumeSubtitleTimer_ = new QtExt::CountdownTimer;
  resumeSubtitleTimer_->setInterval(3000); // 3 seconds

  resumeAudioTrackTimer_ = new QtExt::CountdownTimer;
  resumeAudioTrackTimer_->setInterval(3000); // 3 seconds

  resumeAspectRatioTimer_ = new QtExt::CountdownTimer;
  resumeAspectRatioTimer_->setInterval(3000); // 3 seconds

  navigationTimer_ = new QTimer(this);
  navigationTimer_->setInterval(5000); // 5 seconds
  navigationTimer_->setSingleShot(true);
  connect(navigationTimer_, SIGNAL(timeout()), SLOT(enableNavigation()));

  browserDelegate_ = new AcBrowser(this);
  downloaderDelegate_ = new AcDownloader(this);
  appServer_ = new AcPlayerServer(this);
}

void
MainWindow::createConnections()
{
#ifdef Q_WS_WIN
  connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), SLOT(onFocusedWidgetChanged(QWidget*, QWidget*)));
#endif // Q_WS_WIN

  AnnotationSettings *annotationSettings = AnnotationSettings::globalInstance();

  connect(this, SIGNAL(posChanged()), embeddedPlayer_, SLOT(hide()));

  // Settings
  connect(AcSettings::globalSettings(), SIGNAL(nicovideoAccountChanged(QString,QString)),
          MrlResolverSettings::globalSettings(), SLOT(setNicovideoAccount(QString,QString)));
  connect(AcSettings::globalSettings(), SIGNAL(bilibiliAccountChanged(QString,QString)),
          MrlResolverSettings::globalSettings(), SLOT(setBilibiliAccount(QString,QString)));

  // Player
  connect(player_, SIGNAL(titleIdChanged(int)), SLOT(invalidateToken()));
  connect(player_, SIGNAL(errorEncountered()), SLOT(handlePlayerError()));
  connect(player_, SIGNAL(trackNumberChanged(int)), SLOT(invalidateMediaAndPlay()));
  connect(player_, SIGNAL(endReached()), SLOT(autoPlayNext()));
  connect(player_, SIGNAL(mediaTitleChanged(QString)), SLOT(setWindowTitle(QString)));
  connect(player_, SIGNAL(error(QString)), SLOT(warn(QString)), Qt::QueuedConnection);
  connect(player_, SIGNAL(message(QString)), SLOT(showMessage(QString)), Qt::QueuedConnection);
  connect(player_, SIGNAL(warning(QString)), SLOT(warn(QString)), Qt::QueuedConnection);
  connect(player_, SIGNAL(fileSaved(QString)), SLOT(signFile(QString)), Qt::QueuedConnection);
  connect(player_, SIGNAL(downloadProgress(qint64,qint64)), SLOT(updateDownloadProgress(qint64,qint64)));

  connect(player_, SIGNAL(fileSaved(QString)), AcLocationManager::globalInstance(), SLOT(createDownloadsLocation()), Qt::QueuedConnection);
  connect(AcLocationManager::globalInstance(), SIGNAL(downloadsLocationChanged(QString)), player_, SLOT(setDownloadPath(QString)));

  // Resume
  connect(player_, SIGNAL(stopping()), SLOT(rememberAspectRatio()));
  connect(player_, SIGNAL(stopping()), SLOT(rememberPlayPos()));
  connect(player_, SIGNAL(stopping()), SLOT(rememberSubtitle()));
  connect(player_, SIGNAL(stopping()), SLOT(rememberAudioTrack()));
  connect(player_, SIGNAL(endReached()), SLOT(rememberAspectRatio()));
  connect(player_, SIGNAL(endReached()), SLOT(rememberPlayPos()));
  connect(player_, SIGNAL(endReached()), SLOT(rememberSubtitle()));
  connect(player_, SIGNAL(endReached()), SLOT(rememberAudioTrack()));

  if (rippleEnabled_) {
    connect(player_, SIGNAL(opening()), SLOT(disableRipple()));
    connect(player_, SIGNAL(playing()), SLOT(disableRipple()));
    connect(player_, SIGNAL(stopped()), SLOT(enableRipple()));
    connect(player_, SIGNAL(mediaClosed()), SLOT(enableRipple()));
  }

  connect(loadSubtitlesTimer_, SIGNAL(timeout()), SLOT(loadSubtitles()));
  connect(resumePlayTimer_, SIGNAL(timeout()), SLOT(resumePlayPos()));
  connect(resumeSubtitleTimer_, SIGNAL(timeout()), SLOT(resumeSubtitle()));
  connect(resumeAudioTrackTimer_, SIGNAL(timeout()), SLOT(resumeAudioTrack()));
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
    connect(_playerui, SIGNAL(loginRequested()), SLOT(showLoginDialog())); \
    connect(_playerui, SIGNAL(showPositionPanelRequested()), SLOT(showSeekDialog())); \
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

  // invalidateMediaAndPlay
  connect(this, SIGNAL(playMediaRequested()), SLOT(playMedia()), Qt::QueuedConnection);

  // Queued connections
  connect(this, SIGNAL(response(QString)), SLOT(respond(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(said(QString,QString)), SLOT(say(QString,QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(showTextRequested(QString)), SLOT(showText(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(windowTitleToChange(QString)), SLOT(setWindowTitle(QString)), Qt::QueuedConnection);

  connect(this, SIGNAL(windowMaximized()), SLOT(maximizedToFullScreen()), Qt::QueuedConnection);
  connect(this, SIGNAL(openAnnotationUrlRequested(QString)), SLOT(openAnnotationUrl(QString)), Qt::QueuedConnection);

  // - Message -
  connect(this, SIGNAL(message(QString)), SLOT(showMessage(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(warning(QString)), SLOT(warn(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(errorMessage(QString)), SLOT(error(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(notification(QString)), SLOT(notify(QString)), Qt::QueuedConnection);

  DownloaderController *dc = DownloaderController::globalController();
  connect(dc, SIGNAL(message(QString)), SLOT(showMessage(QString)), Qt::QueuedConnection);
  connect(dc, SIGNAL(warning(QString)), SLOT(warn(QString)), Qt::QueuedConnection);
  connect(dc, SIGNAL(error(QString)), SLOT(error(QString)), Qt::QueuedConnection);
  connect(dc, SIGNAL(notification(QString)), SLOT(notify(QString)), Qt::QueuedConnection);

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

  connect(annotationView_, SIGNAL(searchRequested(int,QString)), SLOT(searchWithEngine(int,QString)));
  connect(annotationView_, SIGNAL(translateRequested(QString,int)), SLOT(translate(QString,int)));

  connect(annotationView_, SIGNAL(message(QString)), SLOT(showMessage(QString)));

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

  connect(player_, SIGNAL(seeked()), annotationView_, SLOT(removeAllItems()));
  connect(player_, SIGNAL(rateChanged(qreal)), annotationView_, SLOT(removeAllItems()));

  connect(embeddedPlayer_, SIGNAL(visibleChanged(bool)), SLOT(updateAnnotationMetaVisible()));
  connect(player_, SIGNAL(playing()), SLOT(updateAnnotationMetaVisible()));
  connect(player_, SIGNAL(paused()), SLOT(updateAnnotationMetaVisible()));
  connect(player_, SIGNAL(stopped()), SLOT(updateAnnotationMetaVisible()));

  // Data manager
  //connect(dataManager_, SIGNAL(aliasesChanged(AliasList)), SLOT(openAnnotationUrlFromAliases(AliasList)));

  connect(dataManager_, SIGNAL(aliasRemovedWithId(qint64)), dataServer_, SLOT(deleteAliasWithId(qint64)));
  connect(dataManager_, SIGNAL(annotationRemovedWithId(qint64)), dataServer_, SLOT(deleteAnnotationWithId(qint64)));
  connect(dataManager_, SIGNAL(annotationTextUpdatedWithId(QString,qint64)), dataServer_, SLOT(updateAnnotationTextWithId(QString,qint64)));

  connect(annotationView_, SIGNAL(annotationAdded(Annotation)), dataManager_, SLOT(addAnnotation(Annotation)));
  connect(annotationView_, SIGNAL(annotationsRemoved()), dataManager_, SLOT(removeAnnotations()));

  connect(annotationView_, SIGNAL(annotationsRemoved()), SLOT(clearAnnotationUrls()));

  connect(annotationView_, SIGNAL(annotationDeletedWithId(qint64)), dataManager_, SLOT(removeAnnotationWithId(qint64)));

  connect(annotationView_, SIGNAL(annotationTextUpdatedWithId(QString,qint64)), dataManager_, SLOT(updateAnnotationTextWithId(QString,qint64)));

  // Forward drag/drop event
  //connect(tokenView_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  //connect(tokenView_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  //connect(tokenView_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  //connect(tokenView_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));

#ifdef USE_MODE_SIGNAL
  //connect(signalView_->tokenView(), SIGNAL(aliasSubmitted(Alias)), SLOT(submitAlias(Alias)));

  //connect(signalView_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  //connect(signalView_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  //connect(signalView_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  //connect(signalView_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));
#endif // USE_MODE_SIGNAL


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

#ifdef Q_WS_WIN
  connect(annotationView_, SIGNAL(trackedWindowChanged(WId)), SLOT(setEmbeddedWindow(WId)));
#endif // Q_WS_WIN
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

#ifdef Q_WS_WIN
  //if (!QtWin::isWindowsVistaOrLater())
#endif // Q_WS_WIN
  {
    connect(player_, SIGNAL(playing()), SLOT(disableWindowTransparency()));
    connect(player_, SIGNAL(stopped()), SLOT(enableWindowTransparency()));
  }

  // Agents:
#ifdef WITH_MODULE_CLIENTAGENT
  connect(client_, SIGNAL(chatReceived(QString)), SLOT(respond(QString)));
#endif // WITH_MODULE_CLIENTAGENT

  // Clipboard
  connect(clipboardMonitor_, SIGNAL(annotationUrlEntered(QString)), SLOT(enterAnnotationUrl(QString)));
  connect(clipboardMonitor_, SIGNAL(mediaUrlEntered(QString)), SLOT(enterMediaUrl(QString)));

  // Language:
  connect(TranslatorManager::globalInstance(), SIGNAL(languageChanged()), SLOT(invalidateAppLanguage()));

  // Translator:
  connect(translator_, SIGNAL(translated(QString)), SLOT(showTextAsSubtitle(QString)));

  // Logger:

  connect(cache_, SIGNAL(cleared()), logger_, SLOT(logCacheCleared()));

  connect(server_, SIGNAL(connectedChanged(bool)), logger_, SLOT(logInternetConnectionChanged(bool)));
  connect(server_, SIGNAL(loginRequested(QString)), logger_, SLOT(logLoginRequested(QString)));
  connect(server_, SIGNAL(loginSucceeded(QString)), logger_, SLOT(logLoginSucceeded(QString)));
  connect(server_, SIGNAL(loginFailed(QString)), logger_, SLOT(logLoginFailed(QString)));
  connect(server_, SIGNAL(logoutRequested()), logger_, SLOT(logLogoutRequested()));
  connect(server_, SIGNAL(logoutFinished()), logger_, SLOT(logLogoutFinished()));
  connect(server_, SIGNAL(unknownError()), logger_, SLOT(logServerAgentUnknownError()));
  connect(server_, SIGNAL(connectionError()), logger_, SLOT(logServerAgentConnectionError()));
  connect(server_, SIGNAL(serverError()), logger_, SLOT(logServerAgentServerError()));
  connect(server_, SIGNAL(error404()), logger_, SLOT(logServerAgentError404()));

  connect(translator_, SIGNAL(networkError(QString)), logger_, SLOT(logTranslatorNetworkError(QString)));

  connect(annotationSettings, SIGNAL(scaleChanged(qreal)), logger_, SLOT(logAnnotationScaleChanged(qreal)));
  connect(annotationSettings, SIGNAL(rotationChanged(qreal)), logger_, SLOT(logAnnotationRotationChanged(qreal)));
  connect(annotationSettings, SIGNAL(offsetChanged(int)), logger_, SLOT(logAnnotationOffsetChanged(int)));
  connect(annotationSettings, SIGNAL(preferMotionlessChanged(bool)), logger_, SLOT(logPreferMotionlessAnnotationChanged(bool)));

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

  connect(dataServer_, SIGNAL(preferLocalChanged(bool)), logger_, SLOT(logPreferLocalDatabaseChanged(bool)));

#ifdef WITH_MODULE_CLIENTAGENT
  connect(client_, SIGNAL(authorized()), logger_, SLOT(logClientAgentAuthorized()));
  connect(client_, SIGNAL(deauthorized()), logger_, SLOT(logClientAgentDeauthorized()));
  connect(client_, SIGNAL(authorizationError()), logger_, SLOT(logClientAgentAuthorizationError()));
#endif // WITH_MODULE_CLIENTAGENT

#ifdef USE_MODE_SIGNAL
  connect(this, SIGNAL(windowPicked(WId)), annotationView_, SLOT(setTrackedWindow(WId)));
  connect(this, SIGNAL(windowPicked(WId)), SLOT(setWindowOnTop()));
  //connect(this, SIGNAL(windowPicked(WId)), osdWindow_, SLOT(setWindowOnTop()));

  connect(this, SIGNAL(attached(ProcessInfo)), signalView_->processView(), SIGNAL(attached(ProcessInfo)));
  connect(this, SIGNAL(detached(ProcessInfo)), signalView_->processView(), SIGNAL(detached(ProcessInfo)));

  connect(signalView_->processView(), SIGNAL(attached(ProcessInfo)), recentMessageView_, SLOT(setProcessNameFromProcessInfo(ProcessInfo)));
  connect(signalView_->processView(), SIGNAL(detached(ProcessInfo)), recentMessageView_, SLOT(clearProcessName()));
  connect(signalView_, SIGNAL(message(QString)), SLOT(showMessage(QString)));
  connect(signalView_, SIGNAL(warning(QString)), SLOT(warn(QString)));
  connect(signalView_, SIGNAL(hookSelected(ulong,ProcessInfo)), SLOT(backlogDialog()));
  connect(signalView_, SIGNAL(hookSelected(ulong,ProcessInfo)), SLOT(openProcessHook(ulong,ProcessInfo)));

  connect(recentMessageView_, SIGNAL(hookSelected(ulong)), SLOT(openProcessHook(ulong)));
  connect(recentMessageView_, SIGNAL(hookSelected(ulong)), recentMessageView_, SLOT(hide()));
  connect(messageHandler_, SIGNAL(messageReceivedWithId(qint64)), annotationView_, SLOT(showAnnotationsAtPos(qint64)));
  connect(messageHandler_, SIGNAL(messageReceivedWithText(QString)), SLOT(translate(QString)));
#endif // USE_MODE_SIGNAL
  //connect(player_, SIGNAL(opening()), SLOT(backlogDialog()));
  // MRL resolver
  connect(mrlResolver_, SIGNAL(mediaResolved(MediaInfo,QNetworkCookieJar*)), SLOT(openRemoteMedia(MediaInfo,QNetworkCookieJar*)));
  connect(mrlResolver_, SIGNAL(subtitleResolved(QString)), SLOT(importAnnotationsFromUrl(QString)));
  connect(mrlResolver_, SIGNAL(message(QString)), SLOT(showMessage(QString)), Qt::QueuedConnection);
  connect(mrlResolver_, SIGNAL(error(QString)), SLOT(warn(QString)), Qt::QueuedConnection);

#ifdef Q_WS_WIN
  connect(AcUi::globalInstance(), SIGNAL(aeroEnabledChanged(bool)), AcSettings::globalSettings(), SLOT(setAeroEnabled(bool)));
#endif // Q_WS_WIN

  // Annotation codec
  {
    AnnotationCodecManager *acm = AnnotationCodecManager::globalInstance();
    connect(acm, SIGNAL(message(QString)), SLOT(showMessage(QString)), Qt::QueuedConnection);
    connect(acm, SIGNAL(error(QString)), SLOT(warn(QString)), Qt::QueuedConnection);
    connect(acm, SIGNAL(fetched(AnnotationList,QString)), SLOT(addRemoteAnnotations(AnnotationList,QString)));
  }

  // Streaming service
  //{
  //  StreamService *ss = StreamService::globalInstance();
  //  connect(ss, SIGNAL(message(QString)), SLOT(showMessage(QString)));
  //  connect(ss, SIGNAL(error(QString)), SLOT(warn(QString)));
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

bool
MainWindow::isValid() const
{ return player_ && player_->isValid();}

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
          SIGNAL(triggered()), SLOT(showLoginDialog()));
  connect(logoutAct_ = new QAction(tr("Logout"), this),
          SIGNAL(triggered()), SLOT(logout()));

  connect(updateAnnotationsAct_ = new QAction(tr("Update Annotations"), this),
          SIGNAL(triggered()), SLOT(updateAnnotations()));
          updateAnnotationsAct_->setShortcut(QKeySequence("CTRL+U"));
          addAction(updateAnnotationsAct_);

  connect(downloadCurrentUrlAct_ = new QAction(QIcon(ACRC_IMAGE_DOWNLOADER), TR(T_MENUTEXT_DOWNLOADCURRENT), this),
          SIGNAL(triggered()), SLOT(downloadCurrentUrl()));
          downloadCurrentUrlAct_->setShortcut(QKeySequence("CTRL+D"));
          addAction(downloadCurrentUrlAct_);

  connect(openProxyBrowserAct_ = new QAction(QIcon(ACRC_IMAGE_BROWSER), TR(T_MENUTEXT_PROXYVIEW), this),
          SIGNAL(triggered()), SLOT(openProxyBrowser()));
          openProxyBrowserAct_->setShortcut(QKeySequence("CTRL+B"));
          addAction(openProxyBrowserAct_);

  connect(toggleDownloaderVisibleAct_ = new QAction(QIcon(ACRC_IMAGE_DOWNLOADER), TR(T_MENUTEXT_DOWNLOAD), this),
          SIGNAL(triggered()), SLOT(showDownloader()));

  connect(openInWebBrowserAct_ = new QAction(QIcon(ACRC_IMAGE_BROWSER), TR(T_MENUTEXT_OPENINWEBBROWSER), this),
          SIGNAL(triggered()), SLOT(openInWebBrowser()));

  connect(showWindowPickDialogAct_ = new QAction(TR(T_MENUTEXT_WINDOWPICKDIALOG), this),
          SIGNAL(triggered()), SLOT(showWindowPickDialog()));
  connect(showProcessDialogAct_ = new QAction(TR(T_MENUTEXT_PROCESSPICKDIALOG), this),
          SIGNAL(triggered()), SLOT(showProcessPickDialog()));

  connect(toggleMagnifierVisibleAct_ = new QAction(tr("Image Filter") + " (" + TR(T_EXPERIMENTAL) + ")", this),
          SIGNAL(triggered(bool)), SLOT(setMagnifierVisible(bool)));
          toggleMagnifierVisibleAct_->setCheckable(true);
          toggleMagnifierVisibleAct_->setShortcut(QKeySequence("CTRL+E"));
          addAction(toggleMagnifierVisibleAct_);

  connect(toggleAnnotationBandwidthLimitedAct_ = new QAction(tr("Limit Bandwidth"), this),
          SIGNAL(triggered(bool)), annotationView_, SLOT(setItemCountLimited(bool)));
          toggleAnnotationBandwidthLimitedAct_->setCheckable(true);

  connect(toggleAnnotationAvatarVisibleAct_ = new QAction(tr("Always Show Avatar"), this),
          SIGNAL(triggered(bool)), AnnotationSettings::globalInstance(), SLOT(setAvatarVisible(bool)));
          toggleAnnotationAvatarVisibleAct_->setCheckable(true);

  connect(togglePreferMotionlessAnnotationAct_ = new QAction(tr("Reduce Movement"), this),
          SIGNAL(triggered(bool)), AnnotationSettings::globalInstance(), SLOT(setPreferMotionless(bool)));
          togglePreferMotionlessAnnotationAct_->setCheckable(true);

  connect(toggleWindowTrackingAct_ = new QAction(QIcon(RC_IMAGE_TRACE), tr("Trace Window"), this),
          SIGNAL(triggered(bool)), SLOT(setWindowTrackingEnabled(bool)));
          toggleWindowTrackingAct_->setCheckable(true);

  connect(aboutAct_ = new QAction(tr("About"), this), SIGNAL(triggered()), SLOT(about()));
  connect(updateAct_ = new QAction(tr("Update"), this), SIGNAL(triggered()), SLOT(update()));

  connect(preferencesAct_ = new QAction(tr("Preferences"), this), SIGNAL(triggered()), SLOT(showPreferences()));
#ifndef Q_WS_MAC
    preferencesAct_->setShortcut(QKeySequence("CTRL+,"));
    addAction(preferencesAct_);
#endif // !Q_WS_MAC
#ifdef Q_WS_WIN
    new QShortcut(QKeySequence("ALT+O"), this, SLOT(showPreferences()));
#endif // Q_WS_WIN

  connect(helpAct_ = new QAction(tr("Help"), this), SIGNAL(triggered()), SLOT(help()));
    helpAct_->setShortcuts(QKeySequence::HelpContents);
#ifndef Q_WS_MAC
    addAction(helpAct_);
#endif // Q_WS_MAC

  connect(quitAct_ = new QAction(tr("Quit"), this), SIGNAL(triggered()), SLOT(close()));
  quitAct_->setShortcuts(QKeySequence::Quit);

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
  MAKE_ACTION(openAnnotationUrlAct_, OPENANNOTATIONURL, this,   SLOT(openAnnotationUrl()))
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
  MAKE_ACTION(resetAnnotationOffsetAct_, RESETANNOTOFFSET, AnnotationSettings::globalInstance(),  SLOT(resetOffset()))
  MAKE_ACTION(increaseAnnotationOffsetAct_, INCREASEANNOTOFFSET, this,  SLOT(increaseAnnotationOffset()))
  MAKE_ACTION(decreaseAnnotationOffsetAct_, DECREASEANNOTOFFSET, this,  SLOT(decreaseAnnotationOffset()))
  MAKE_ACTION(resetAnnotationScaleAct_, RESETANNOTSCALE, AnnotationSettings::globalInstance(),  SLOT(resetScale()))
  MAKE_ACTION(increaseAnnotationScaleAct_, INCREASEANNOTSCALE, this,  SLOT(annotationScaleUp()))
  MAKE_ACTION(decreaseAnnotationScaleAct_, DECREASEANNOTSCALE, this,  SLOT(annotationScaleDown()))
  MAKE_ACTION(resetAnnotationRotationAct_, RESETANNOTROTATION, AnnotationSettings::globalInstance(),  SLOT(resetRotation()))
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
  MAKE_TOGGLE(toggleAutoPlayNextAct_, AUTOPLAYNEXT, this,  SLOT(setAutoPlayNext()))
  MAKE_TOGGLE(toggleRepeatCurrentAct_, AUTOPLAYCURRENT, this,  SLOT(setRepeatCurrent()))
  MAKE_TOGGLE(toggleNoRepeatAct_, NOAUTOPLAY, this,  SLOT(setNoRepeat()))
  MAKE_TOGGLE(actualSizeAct_, ACTUALSIZE, this,  SLOT(actualSize()))
  MAKE_TOGGLE(togglePreferLocalDatabaseAct_, PREFERLOCALDB, dataServer_,  SLOT(setPreferLocal(bool)))
  MAKE_TOGGLE(toggleAnnotationAnalyticsViewVisibleAct_, ANNOTANALYTICS, this,  SLOT(setAnnotationAnalyticsViewVisible(bool)))
  MAKE_TOGGLE(toggleSaveMediaAct_, AUTOSAVEMEDIA, this, SLOT(setSaveMedia(bool)))
  MAKE_TOGGLE(toggleSubmitAct_, AUTOSUBMIT, this, SLOT(setSubmit(bool)))
  MAKE_TOGGLE(nothingAfterFinishedAct_, NOTHINGAFTERFINISHED, this, SLOT(nothingAfterFinished()))
  MAKE_TOGGLE(sleepAfterFinishedAct_, SLEEPAFTERFINISHED, this, SLOT(sleepAfterFinished()))
  MAKE_TOGGLE(shutdownAfterFinishedAct_, SHUTDOWNAFTERFINISHED, this, SLOT(shutdownAfterFinished()))
  MAKE_TOGGLE(toggleMultipleWindowsEnabledAct_, MULTIWINDOW, this, SLOT(setMultipleWindowsEnabled(bool)))
  MAKE_TOGGLE(toggleClipboardMonitorEnabledAct_, MONITORCLIPBOARD, clipboardMonitor_, SLOT(setEnabled(bool)))
  //MAKE_TOGGLE(toggleNetworkProxyDialogVisibleAct_, NETWORKPROXY, this, SLOT(setNetworkProxyDialogVisible(bool)))
  MAKE_TOGGLE(toggleAeroEnabledAct_, ENABLEAERO, AcUi::globalInstance(), SLOT(setAeroEnabled(bool)))
  MAKE_TOGGLE(toggleMenuThemeEnabledAct_, MENUTHEME, AcUi::globalInstance(), SLOT(setMenuEnabled(bool)))
  MAKE_TOGGLE(toggleFullScreenModeAct_, FULLSCREEN, hub_,       SLOT(setFullScreenWindowMode(bool)))
  MAKE_TOGGLE(toggleMenuBarVisibleAct_, SHOWMENUBAR, menuBar(), SLOT(setVisible(bool)))
  MAKE_TOGGLE(toggleAnnotationCountDialogVisibleAct_, ANNOTATIONLIMIT, this, SLOT(setAnnotationCountDialogVisible(bool)))
  MAKE_TOGGLE(toggleEmbeddedModeAct_, EMBED,    hub_,           SLOT(setEmbeddedPlayerMode(bool)))
  MAKE_TOGGLE(toggleMiniModeAct_, MINI,         hub_,           SLOT(setMiniPlayerMode(bool)))
  MAKE_TOGGLE(toggleLiveModeAct_, LIVE,         hub_,           SLOT(setLiveTokenMode(bool)))
  MAKE_TOGGLE(toggleSyncModeAct_, SYNC,         hub_,           SLOT(setSyncPlayMode(bool)))
  MAKE_TOGGLE(toggleAnnotationVisibleAct_, SHOWANNOT, annotationView_, SLOT(setItemVisible(bool)))
  MAKE_TOGGLE(toggleSubtitleVisibleAct_, SHOWSUBTITLE, player_, SLOT(setSubtitleVisible(bool)))
  MAKE_TOGGLE(toggleSubtitleAnnotationVisibleAct_, SUBANNOT, annotationView_, SLOT(setSubtitleVisible(bool)))
  MAKE_TOGGLE(toggleNonSubtitleAnnotationVisibleAct_, NONSUBANNOT, annotationView_, SLOT(setNonSubtitleVisible(bool)))
  MAKE_TOGGLE(toggleWindowOnTopAct_, WINDOWSTAYSONTOP, this, SLOT(setWindowOnTop(bool)))
  MAKE_TOGGLE(toggleUserAnonymousAct_,  ANONYMOUS,       this,         SLOT(setUserAnonymous(bool)))
  MAKE_TOGGLE(toggleUserViewVisibleAct_, USER,          this,         SLOT(setUserViewVisible(bool)))
  MAKE_TOGGLE(toggleBlacklistViewVisibleAct_, BLACKLIST,   this,    SLOT(setBlacklistViewVisible(bool)))
  MAKE_TOGGLE(toggleAnnotationFilterEnabledAct_, ENABLEBLACKLIST,   annotationFilter_,    SLOT(setEnabled(bool)))
  MAKE_TOGGLE(toggleBacklogDialogVisibleAct_, BACKLOG,   this,    SLOT(setBacklogDialogVisible(bool)))
  MAKE_TOGGLE(toggleConsoleDialogVisibleAct_, CONSOLE,   this,    SLOT(setConsoleDialogVisible(bool)))
  MAKE_TOGGLE(toggleLoginDialogVisibleAct_, LOGINDIALOG, this,         SLOT(setLoginDialogVisible(bool)))
  MAKE_TOGGLE(toggleSeekDialogVisibleAct_,  SEEKDIALOG,  this,         SLOT(setSeekDialogVisible(bool)))
  MAKE_TOGGLE(toggleLiveDialogVisibleAct_,  LIVEDIALOG,  this,         SLOT(setLiveDialogVisible(bool)))
  MAKE_TOGGLE(toggleSyncDialogVisibleAct_,  SYNCDIALOG,  this,         SLOT(setSyncDialogVisible(bool)))
  //MAKE_TOGGLE(toggleSiteAccountViewVisibleAct_, SITEACCOUNT, this, SLOT(setSiteAccountViewVisible(bool)))
  MAKE_TOGGLE(toggleMediaInfoViewVisibleAct_, MEDIAINFO, this, SLOT(setMediaInfoViewVisible(bool)))
  MAKE_TOGGLE(toggleAnnotationBrowserVisibleAct_, ANNOTATIONBROWSER, this, SLOT(setAnnotationBrowserVisible(bool)))
  MAKE_TOGGLE(toggleAnnotationEditorVisibleAct_, ANNOTATIONEDITOR, this, SLOT(setAnnotationEditorVisible(bool)))
  MAKE_TOGGLE(toggleTokenViewVisibleAct_, TOKENVIEW, this, SLOT(setTokenViewVisible(bool)))
  //MAKE_TOGGLE(toggleCommentViewVisibleAct_, COMMENTVIEW, this, SLOT(setCommentViewVisible(bool)))
  MAKE_ACTION(openHomePageAct_, HOMEPAGE, this, SLOT(openHomePage()))
  MAKE_TOGGLE(toggleTranslateAct_, TRANSLATE,   this,           SLOT(setTranslateEnabled(bool)))
  MAKE_TOGGLE(toggleSubtitleOnTopAct_, SUBTITLEONTOP,   this,  SLOT(setSubtitleOnTop(bool)))
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
#ifdef USE_MODE_SIGNAL
  MAKE_TOGGLE(toggleSignalViewVisibleAct_, SIGNALVIEW, this, SLOT(setSignalViewVisible(bool)))
  MAKE_TOGGLE(toggleRecentMessageViewVisibleAct_, RECENTMESSAGES, this, SLOT(setRecentMessageViewVisible(bool)))
#endif // USE_MODE_SIGNAL

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

  MAKE_ACTION(clearRecentAct_,  CLEARRECENT,    this,   SLOT(clearRecent()))
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

  MAKE_TOGGLE(setAppLanguageToEnglishAct_,  ENGLISH, this, SLOT(setAppLanguageToEnglish()))
  MAKE_TOGGLE(setAppLanguageToJapaneseAct_, JAPANESE,this, SLOT(setAppLanguageToJapanese()))
  MAKE_TOGGLE(setAppLanguageToTraditionalChineseAct_,TRADITIONALCHINESE, this, SLOT(setAppLanguageToTraditionalChinese()))
  MAKE_TOGGLE(setAppLanguageToSimplifiedChineseAct_, SIMPLIFIEDCHINESE, this, SLOT(setAppLanguageToSimplifiedChinese()))

  MAKE_TOGGLE(setUserLanguageToEnglishAct_,  ENGLISH, this, SLOT(setUserLanguageToEnglish()))
  MAKE_TOGGLE(setUserLanguageToJapaneseAct_, JAPANESE,this, SLOT(setUserLanguageToJapanese()))
  MAKE_TOGGLE(setUserLanguageToChineseAct_, CHINESE, this, SLOT(setUserLanguageToChinese()))
  MAKE_TOGGLE(setUserLanguageToKoreanAct_, KOREAN, this, SLOT(setUserLanguageToKorean()))
  MAKE_TOGGLE(setUserLanguageToUnknownAct_, UNKNOWNLANGUAGE, this, SLOT(setUserLanguageToUnknown()))

  MAKE_TOGGLE(toggleAnnotationLanguageToEnglishAct_,  ENGLISH, this, SLOT(invalidateAnnotationLanguages()))
  MAKE_TOGGLE(toggleAnnotationLanguageToJapaneseAct_, JAPANESE,this, SLOT(invalidateAnnotationLanguages()))
  MAKE_TOGGLE(toggleAnnotationLanguageToChineseAct_, CHINESE, this, SLOT(invalidateAnnotationLanguages()))
  MAKE_TOGGLE(toggleAnnotationLanguageToKoreanAct_, KOREAN, this, SLOT(invalidateAnnotationLanguages()))
  MAKE_TOGGLE(toggleAnnotationLanguageToUnknownAct_, UNKNOWNLANGUAGE, this, SLOT(invalidateAnnotationLanguages()))
  MAKE_TOGGLE(toggleAnnotationLanguageToAnyAct_, ANYLANGUAGE, this,SLOT(invalidateAnnotationLanguages()))

  MAKE_ACTION(showMaximizedAct_,        MAXIMIZE,       this,    SLOT(showMaximized()))
  MAKE_ACTION(showMinimizedAct_,        MINIMIZE,       this,    SLOT(showMinimizedAndPause()))
  MAKE_ACTION(showNormalAct_,           RESTORE,        this,    SLOT(showNormal()))

#ifdef Q_WS_WIN
  //if (!QtWin::isWindowsVistaOrLater())
#endif // Q_WS_WIN
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
    toggleSeekDialogVisibleAct_->setShortcut(QKeySequence("CTRL+F"));
    addAction(toggleSeekDialogVisibleAct_);

    nextFrameAct_->setShortcut(QKeySequence("CTRL+G"));
    addAction(nextFrameAct_);

    openFileAct_->setShortcuts(QKeySequence::Open);
    addAction(openFileAct_);

    openUrlAct_->setShortcuts(QKeySequence::New);
    addAction(openUrlAct_);

    openAnnotationUrlAct_->setShortcuts(QKeySequence::Italic);
    addAction(openAnnotationUrlAct_);

    snapshotAct_->setShortcuts(QKeySequence::Print);
    addAction(snapshotAct_);

    saveMediaAct_->setShortcuts(QKeySequence::Save);
    addAction(saveMediaAct_);

    resumeAnnotationAct_->setShortcut(QKeySequence("CTRL+R"));
    addAction(resumeAnnotationAct_);

#ifndef Q_WS_MAC
    toggleWindowOnTopAct_->setShortcuts(QKeySequence::AddTab);
    addAction(toggleWindowOnTopAct_);
#endif // !Q_WS_MAC

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

    toggleAnnotationEditorVisibleAct_->setShortcut(QKeySequence("CTRL+F1"));
    addAction(toggleAnnotationEditorVisibleAct_);
    toggleAnnotationBrowserVisibleAct_->setShortcut(QKeySequence("CTRL+F2"));
    addAction(toggleAnnotationBrowserVisibleAct_);
    toggleTokenViewVisibleAct_->setShortcut(QKeySequence("CTRL+F3"));
    addAction(toggleTokenViewVisibleAct_);
    toggleBlacklistViewVisibleAct_->setShortcut(QKeySequence("CTRL+F4"));
    addAction(toggleBlacklistViewVisibleAct_);
    toggleBacklogDialogVisibleAct_->setShortcut(QKeySequence("CTRL+F5"));
    addAction(toggleBacklogDialogVisibleAct_);
    toggleAnnotationAnalyticsViewVisibleAct_->setShortcut(QKeySequence("CTRL+F6"));
    addAction(toggleAnnotationAnalyticsViewVisibleAct_);

    QShortcut *pp = new QShortcut(QKeySequence("CTRL+SHIFT+LEFT"), this);
    connect(pp, SIGNAL(activated()), SLOT(previous()));
    QShortcut *nn = new QShortcut(QKeySequence("CTRL+SHIFT+RIGHT"), this);
    connect(nn, SIGNAL(activated()), SLOT(next()));

#ifndef Q_WS_MAC
    newWindowAct_->setShortcut(QKeySequence("CTRL+SHIFT+N"));
    addAction(newWindowAct_);
    //QShortcut *csn = new QShortcut(QKeySequence("CTRL+SHIFT+N"), this);
    //connect(csn, SIGNAL(activated()), SLOT(newWindow()));
#endif // Q_WS_MAC

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

#ifdef Q_WS_MAC
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
#ifdef Q_WS_MAC
    QShortcut *a3 = new QShortcut(QKeySequence(K_ALT "+3"), this);
    connect(a3, SIGNAL(activated()), SLOT(hueDown()));
#else
    QShortcut *ad = new QShortcut(QKeySequence(K_ALT "+D"), this);
    connect(ad, SIGNAL(activated()), SLOT(hueDown()));
#endif // Q_WS_MAC
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
#endif // Q_WS_MAC
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

    helpContextMenu_->addAction(toggleConsoleDialogVisibleAct_);
    helpContextMenu_->addAction(checkInternetConnectionAct_);
    helpContextMenu_->addAction(helpAct_);
    helpContextMenu_->addAction(openHomePageAct_);
    helpContextMenu_->addAction(updateAct_);

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
    appLanguageMenu_->addAction(setAppLanguageToJapaneseAct_);
    //appLanguageMenu_->addAction(setAppLanguageToTraditionalChineseAct_); // Temporarily disabled since no traditional chinese at this time
    appLanguageMenu_->addAction(setAppLanguageToSimplifiedChineseAct_);
  }

  userLanguageMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(userLanguageMenu_, true); // persistent = true

    userLanguageMenu_->setTitle(TR(T_MENUTEXT_USERLANGUAGE) + " ...");
    userLanguageMenu_->setToolTip(TR(T_TOOLTIP_USERLANGUAGE));

    userLanguageMenu_->addAction(setUserLanguageToEnglishAct_);
    userLanguageMenu_->addAction(setUserLanguageToJapaneseAct_);
    userLanguageMenu_->addAction(setUserLanguageToChineseAct_);
    userLanguageMenu_->addAction(setUserLanguageToKoreanAct_);
    userLanguageMenu_->addAction(setUserLanguageToUnknownAct_);
  }

  gameMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(gameMenu_, this);
    gameMenu_->setTitle(tr("Sync with Galgame")  + " ...");
    gameMenu_->setToolTip(tr("Sync with Galgame"));
#ifdef USE_MODE_SIGNAL

#ifdef WITH_WIN_PICKER
    gameMenu_->addAction(showProcessDialogAct_);
#endif // WITH_WIN_PICKER
    gameMenu_->addAction(toggleSignalViewVisibleAct_);
    gameMenu_->addSeparator();
    gameMenu_->addAction(toggleRecentMessageViewVisibleAct_);
    gameMenu_->addAction(toggleBacklogDialogVisibleAct_);
    gameMenu_->addSeparator();
    gameMenu_->addAction(toggleTranslateAct_);
    gameMenu_->addMenu(userLanguageMenu_);
#endif // USE_MODE_SIGNAL
  }

  annotationLanguageMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(annotationLanguageMenu_, true); // persistent = true

    annotationLanguageMenu_->setTitle(TR(T_MENUTEXT_ANNOTATIONLANGUAGE) + " ...");
    annotationLanguageMenu_->setToolTip(TR(T_TOOLTIP_ANNOTATIONLANGUAGE));

    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToAnyAct_);
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToUnknownAct_);
    annotationLanguageMenu_->addSeparator();
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToEnglishAct_);
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToJapaneseAct_);
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToChineseAct_);
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToKoreanAct_);
  }

//#ifdef Q_WS_WIN
//  if (!QtWin::isWindowsVistaOrLater())
//#endif // Q_WS_WIN
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

  subtitleStyleMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(subtitleStyleMenu_, true); // persistent = true

    subtitleStyleMenu_->setTitle(TR(T_MENUTEXT_SUBTITLESTYLE) + " ...");
    subtitleStyleMenu_->setToolTip(TR(T_TOOLTIP_SUBTITLESTYLE));

    subtitleStyleMenu_->addAction(setSubtitleColorToDefaultAct_);
    subtitleStyleMenu_->addSeparator();
    subtitleStyleMenu_->addAction(setSubtitleColorToWhiteAct_);
    subtitleStyleMenu_->addAction(setSubtitleColorToCyanAct_);
    subtitleStyleMenu_->addAction(setSubtitleColorToBlueAct_);
    subtitleStyleMenu_->addAction(setSubtitleColorToPurpleAct_);
    subtitleStyleMenu_->addAction(setSubtitleColorToRedAct_);
    subtitleStyleMenu_->addAction(setSubtitleColorToOrangeAct_);
    subtitleStyleMenu_->addAction(setSubtitleColorToBlackAct_);
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

  adjustMenu_ = new QMenu(this); {
    adjustMenu_->setTitle(tr("Adjust video") + " ...");
    adjustMenu_->setToolTip(tr("Adjust video"));
    ui->setContextMenuStyle(adjustMenu_, true); // persistent = true

    adjustMenu_->addAction(toggleMediaInfoViewVisibleAct_);
    adjustMenu_->addSeparator();

    adjustMenu_->addMenu(aspectRatioMenu_);
    adjustMenu_->addSeparator();

    adjustMenu_->addAction(saturationUpAct_);
    adjustMenu_->addAction(saturationDownAct_);
    adjustMenu_->addAction(saturationResetAct_);
    adjustMenu_->addSeparator();
    adjustMenu_->addAction(gammaUpAct_);
    adjustMenu_->addAction(gammaDownAct_);
    adjustMenu_->addAction(gammaResetAct_);
    adjustMenu_->addSeparator();
    adjustMenu_->addAction(hueUpAct_);
    adjustMenu_->addAction(hueDownAct_);
    adjustMenu_->addAction(hueResetAct_);
    adjustMenu_->addSeparator();
    adjustMenu_->addAction(contrastUpAct_);
    adjustMenu_->addAction(contrastDownAct_);
    adjustMenu_->addAction(contrastResetAct_);
    adjustMenu_->addSeparator();
    adjustMenu_->addAction(brightnessUpAct_);
    adjustMenu_->addAction(brightnessDownAct_);
    adjustMenu_->addAction(brightnessResetAct_);
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
    openMenu_->setTitle(TR(T_MENUTEXT_OPENCONTEXTMENU) + " ...");
    openMenu_->setToolTip(TR(T_TOOLTIP_OPENCONTEXTMENU));
    openMenu_->setIcon(QIcon(RC_IMAGE_OPENCONTEXTMENU));
    ui->setContextMenuStyle(openMenu_, true); // persistent = true
  }

  subtitleMenu_ = new QMenu(this); {
    subtitleMenu_->setIcon(QIcon(RC_IMAGE_SUBTITLE));
    subtitleMenu_->setTitle(TR(T_MENUTEXT_SUBTITLE) + " ...");
    subtitleMenu_->setToolTip(TR(T_TOOLTIP_SUBTITLE));
    ui->setContextMenuStyle(subtitleMenu_, true); // persistent = true
  }

  playMenu_ = new QMenu(this); {
    playMenu_->setTitle(tr("Play") + " ...");
    playMenu_->setToolTip(tr("Play menu"));
    ui->setContextMenuStyle(playMenu_, true); // persistent = true
  }

  audioTrackMenu_ = new QMenu(this); {
    audioTrackMenu_->setIcon(QIcon(RC_IMAGE_AUDIOTRACK));
    audioTrackMenu_->setTitle(TR(T_MENUTEXT_AUDIOTRACK) + " ...");
    audioTrackMenu_->setToolTip(TR(T_TOOLTIP_AUDIOTRACK));
    ui->setContextMenuStyle(audioTrackMenu_, true); // persistent = true
  }

  annotationSubtitleMenu_ = new QMenu(this); {
    annotationSubtitleMenu_->setIcon(QIcon(RC_IMAGE_ANNOTSUBTITLE));
    annotationSubtitleMenu_->setTitle(TR(T_MENUTEXT_ANNOTSUBTITLE) + " ...");
    annotationSubtitleMenu_->setToolTip(TR(T_TOOLTIP_ANNOTSUBTITLE));
    ui->setContextMenuStyle(annotationSubtitleMenu_, true); // persistent = true

    annotationSubtitleMenu_->addAction(toggleSubtitleAnnotationVisibleAct_);
    annotationSubtitleMenu_->addSeparator();
    annotationSubtitleMenu_->addAction(toggleNonSubtitleAnnotationVisibleAct_);
    annotationSubtitleMenu_->addAction(toggleSubtitleOnTopAct_);
  }

  sectionMenu_ = new QMenu(this); {
    sectionMenu_->setIcon(QIcon(RC_IMAGE_SECTION));
    sectionMenu_->setTitle(TR(T_MENUTEXT_SECTION) + " ...");
    sectionMenu_->setToolTip(TR(T_TOOLTIP_SECTION));
    ui->setContextMenuStyle(sectionMenu_, true); // persistent = true
  }

  closeMenu_ = new QMenu(this); {
    closeMenu_->setTitle(tr("After finished") + " ...");
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
    utilityMenu_->addAction(toggleDownloaderVisibleAct_);
    utilityMenu_->addAction(toggleMagnifierVisibleAct_);
    utilityMenu_->addAction(toggleAnnotationEditorVisibleAct_ );

#ifndef Q_WS_MAC
    utilityMenu_->addAction(newWindowAct_);
#endif // !Q_WS_MAC
  }

  settingsMenu_ = new QMenu(this); {
    settingsMenu_->setTitle(tr("Settings") + " ...");
    settingsMenu_->setToolTip(tr("Settings"));
    ui->setContextMenuStyle(settingsMenu_, true); // persistent = true

    //settingsMenu_->addAction(tr("Preferences"), this, SLOT(showPreferences()), QKeySequence("CTRL+,"));
    //settingsMenu_->addAction(toggleSiteAccountViewVisibleAct_);
    //settingsMenu_->addAction(toggleNetworkProxyDialogVisibleAct_);

    settingsMenu_->addAction(togglePreferLocalDatabaseAct_);
    settingsMenu_->addAction(toggleClipboardMonitorEnabledAct_);
    settingsMenu_->addAction(toggleSaveMediaAct_);
    settingsMenu_->addAction(toggleSubmitAct_);

    settingsMenu_->addSeparator();
#ifndef Q_WS_MAC
    settingsMenu_->addAction(toggleMultipleWindowsEnabledAct_);
#endif // Q_WS_MAC

#ifndef Q_WS_MAC
    settingsMenu_->addAction(toggleWindowOnTopAct_);
#endif // Q_WS_MAC
#ifndef Q_OS_LINUX
    settingsMenu_->addAction(toggleEmbeddedPlayerOnTopAct_); // FIXME: X11::zeroInput problem
#endif // Q_OS_LINUX
    //settingsMenu_->addAction(togglePlayerLabelEnabled_);

#ifndef Q_WS_MAC
    settingsMenu_->addAction(toggleMenuBarVisibleAct_);
#endif // !Q_WS_MAC
    settingsMenu_->addAction(toggleMenuThemeEnabledAct_);
#ifdef Q_WS_WIN
    if (QtWin::isWindowsVistaOrLater())
      settingsMenu_->addAction(toggleAeroEnabledAct_);
#endif // Q_WS_WIN
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
    annotationSettingsMenu_->addMenu(annotationSubtitleMenu_);
    annotationSettingsMenu_->addMenu(subtitleStyleMenu_);
    annotationSettingsMenu_->addSeparator();
    annotationSettingsMenu_->addAction(togglePreferMotionlessAnnotationAct_);
    annotationSettingsMenu_->addAction(toggleAnnotationAvatarVisibleAct_);
    annotationSettingsMenu_->addAction(toggleAnnotationBandwidthLimitedAct_);
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


//#ifndef Q_WS_WIN
  // Use menuBar_ instead MainWindow::menuBar() to enable customization.
  // It is important to create the menubar WITHOUT PARENT, so that diff windows could share the same menubar.
  // See: http://doc.qt.nokia.com/latest/mac-differences.html#menu-bar
  // See: http://doc.qt.nokia.com/stable/qmainwindow.html#menuBar

  // jichi 11/11/11:  It's important not to use act like quitAct_ or aboutAct_ with translations.

  //menuBar_ = new QMenuBar;
  QMenu *m;
  m = menuBar()->addMenu(tr("&File")); {
    m->addAction(openFileAct_);
    m->addAction(openUrlAct_);
    m->addSeparator();
    m->addAction(openDeviceAct_);
    m->addAction(openVideoDeviceAct_);
    m->addAction(openSubtitleAct_);
    m->addAction(openAnnotationUrlAct_);
#ifdef Q_WS_WIN // TODO add support for Mac/Linux
    m->addAction(openAudioDeviceAct_);
#endif // Q_WS_WIN
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
//#endif // !Q_WS_WIN

  openButtonMenu_ = new QMenu(this); {
    ui->setContextMenuStyle(openButtonMenu_, true); // persistent = true
    openButtonMenu_->setTitle(TR(T_OPEN));
    openButtonMenu_->setToolTip(TR(T_OPEN));
    openButtonMenu_->setIcon(QIcon(RC_IMAGE_OPEN));

    openButtonMenu_->addAction(openFileAct_);
    openButtonMenu_->addAction(openUrlAct_);
    openButtonMenu_->addSeparator();
    openButtonMenu_->addAction(openDeviceAct_);
    openButtonMenu_->addAction(openVideoDeviceAct_);
#ifdef Q_OS_WIN // TODO add support for Mac/Linux
    openButtonMenu_->addAction(openAudioDeviceAct_);
#endif // Q_OS_WIN
    openButtonMenu_->addSeparator();
    openButtonMenu_->addAction(openSubtitleAct_);
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
  MainPlayerDock *mainPlayerDock  = new MainPlayerDock(this);
  mainPlayerDock->setWidget(mainPlayer_);
  addDockWidget(Qt::BottomDockWidgetArea, mainPlayerDock);

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
  if (!hub_->isSignalTokenMode())
    messageHandler_->setActive(false);
#endif // WITH_WIN_TEXTHOOK

  if (!hub_->isLiveTokenMode())
    closeChannel();

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

  if (!hub_->isMediaTokenMode())
    hub_->play();
}

void
MainWindow::updatePlayMode()
{
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
    if (hub_->isNormalWindowMode())
      showNormal();
    else
      show();
    break;

  case SignalHub::MiniPlayerMode:
    //setVisible(v);
    mainPlayer_->hide();
    embeddedPlayer_->hide();
    miniPlayer_->show();
    if (!annotationView_->trackedWindow()) {
      if (hub_->isNormalWindowMode())
        showNormal();
      else
        show();
    }
    break;

  case SignalHub::EmbeddedPlayerMode:
    //setVisible(m);
    if (annotationView_->trackedWindow() ||
        hub_->isLiveTokenMode() && hub_->isFullScreenWindowMode())
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
#ifdef Q_WS_X11
    if (hub_->isMediaTokenMode() && player_->hasMedia())
      AcUi::globalInstance()->setBlackBackground(this);
#endif // Q_WS_X11

    if (mainPlayer_->isVisible())
      mainPlayer_->hide();

    switch (hub_->tokenMode()) {
    case SignalHub::MediaTokenMode:
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
      else if (hub_->isNormalWindowMode())
        showNormal();
      else
        show();

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
}

UrlDialog*
MainWindow::mediaUrlDialog()
{
  if (!mediaUrlDialog_) {
    mediaUrlDialog_ = new MediaUrlDialog(this);
    connect(mediaUrlDialog_, SIGNAL(urlEntered(QString,bool)), SLOT(openUrl(QString,bool)));
    windows_.append(mediaUrlDialog_);
  }
  mediaUrlDialog_->setSave(player_->isBufferSaved());
  return mediaUrlDialog_;
}

void
MainWindow::openUrl()
{
  UrlDialog *d = mediaUrlDialog();
  if (d->isEmpty()) {
    QString url = dataManager_->token().source();
    if (!url.isEmpty())
      d->setText(url);
  }
  d->show();
}


UrlDialog*
MainWindow::annotationUrlDialog()
{
  if (!annotationUrlDialog_) {
    annotationUrlDialog_ = new SubUrlDialog(this);
    connect(annotationUrlDialog_, SIGNAL(urlEntered(QString,bool)), SLOT(openAnnotationUrl(QString,bool)));
    connect(this, SIGNAL(annotationUrlAdded(QString)), annotationUrlDialog_, SLOT(addHistory(QString)), Qt::QueuedConnection);
    windows_.append(annotationUrlDialog_);
  }
  annotationUrlDialog_->setSave(submit_);
  return annotationUrlDialog_;
}

void
MainWindow::openAnnotationUrl()
{ annotationUrlDialog()->show(); }

void
MainWindow::openAnnotationUrlFromAliases(const AliasList &l, bool async)
{
  if (!l.isEmpty()) {
    int site;
    foreach (const Alias &a, l)
      if (a.type() == Alias::AT_Url &&
          (site = MrlAnalysis::matchSite(a.text())) && site < MrlAnalysis::AnnotationSite) {
        if (async)
          emit openAnnotationUrlRequested(a.text());
        else
          openAnnotationUrl(a.text());
      }
  }
}

void
MainWindow::openAnnotationUrl(const QString &url, bool save)
{
  showMessage(tr("analyzing URL ...") + " " + url);
  if (registerAnnotationUrl(url)) {
    bool ok = mrlResolver_->resolveSubtitle(url);
    if (!ok)
      warn(tr("failed to resolve URL") + ": " + url);
    else if (save)
      submitAliasText(url, Alias::AT_Url);

    emit annotationUrlAdded(url);
  }
}

void
MainWindow::openUrl(const QString &url, bool save)
{
  player_->setBufferSaved(save);
  openUrl(url);
}

void
MainWindow::openUrl(const QString &input)
{
  QString url = input;
  if (url.startsWith("ttp://"))
    url.prepend('h');
  if (mrlResolver_->resolveMedia(url))
    showMessage(tr("analyzing URL ...") + " " + url);
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
  if (!QDir(recentPath_).exists())
    recentPath_.clear();

  static const QString filter =
      TR(T_FORMAT_SUPPORTED) + ("(" G_FORMAT_SUPPORTED ")" ";;")
#ifdef USE_MODE_SIGNAL
    + TR(T_GALGAME)   + ("(" G_FORMAT_PROGRAM ")" ";;")
#endif // USE_MODE_SIGNAL
    + TR(T_FORMAT_VIDEO)     + ("(" G_FORMAT_VIDEO ")" ";;")
    + TR(T_FORMAT_AUDIO)     + ("(" G_FORMAT_AUDIO ")" ";;")
    + TR(T_FORMAT_PICTURE)   + ("(" G_FORMAT_PICTURE ")" ";;")
    + TR(T_FORMAT_ALL)       + ("(" G_FORMAT_ALL ")");

  QStringList l = QFileDialog::getOpenFileNames(
        this, TR(T_TITLE_OPENFILE), recentPath_, filter);

  if (!l.isEmpty()) {
    playlist_ = l;

    QString fileName = l.front();
    recentPath_ = QFileInfo(fileName).absolutePath();
    openSource(fileName);
  }
}

void
MainWindow::openProcess()
{
#ifdef USE_MODE_SIGNAL
  showSignalView();
#endif // USE_MODE_SIGNAL
}

void
MainWindow::openWindow()
{
#ifdef USE_MODE_SIGNAL
  setProcessPickDialogVisible(true);
#endif // USE_MODE_SIGNAL
}

bool
MainWindow::isDevicePath(const QString &path)
{
#ifdef Q_WS_WIN
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
  if (!deviceDialog_) {
    deviceDialog_ = new DeviceDialog(this);
    connect(deviceDialog_, SIGNAL(deviceSelected(QString,bool)), SLOT(openDevicePath(QString,bool)));
    windows_.append(deviceDialog_);
  }
  deviceDialog_->show();
}

void
MainWindow::openVideoDevice()
{
  if (!QDir(recentPath_).exists())
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
  if (!QDir(recentPath_).exists())
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
  //  warn(TR(T_ERROR_BAD_DEVICEPATH) + ": " + path);
  //  return;
  //}

  playlist_.clear();

  QString mrl = path;
  if (isAudioCD && !mrl.startsWith(PLAYER_URL_CD, Qt::CaseInsensitive))
    mrl = PLAYER_URL_CD + path;

  recentSourceLocked_ = false;
  openMrl(mrl);
}

void
MainWindow::openDirectory()
{
  if (!QDir(recentPath_).exists())
    recentPath_.clear();

  QString path = QFileDialog::getExistingDirectory(
        this, TR(T_TITLE_OPENAUDIODEVICE), recentPath_);

  if (!path.isEmpty()) {
    playlist_.clear();

    recentPath_ = path;
    recentSourceLocked_ = false;
    openMrl(path);
  }
}

void
MainWindow::openSubtitle()
{
  if (!QDir(recentPath_).exists())
    recentPath_.clear();

  static const QString filter =
      TR(T_FORMAT_SUBTITLE)  + ("(" G_FORMAT_SUBTITLE ")" ";;")
    + TR(T_FORMAT_ALL)       + ("(" G_FORMAT_ALL ")");

  QStringList l = QFileDialog::getOpenFileNames(
        this, TR(T_TITLE_OPENSUBTITLE), recentPath_, filter);

  if (!l.isEmpty()) {
    recentPath_ = QFileInfo(l.front()).absolutePath();
    foreach (const QString &fileName, l)
      openSubtitleFile(fileName);
  }
}

void
MainWindow::openLocalUrl(const QUrl &url)
{
  if (!url.isValid())
    return;

  recentSourceLocked_ = false;
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
      foreach (const QString &f, QtExt::revertList(subs))
        openSubtitleFile(f);
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
  recentSourceLocked_ = false;
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
  recentSourceLocked_ = false;
  playlist_.clear();
  if (mi.mrls.isEmpty()) {
    emit error(tr("failed to resolve URL") + ": " + mi.refurl);
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
  //  recentSource_.clear();
//
  //  MrlInfo mrl = mi.mrls.front();
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
  recentSource_.clear();
  if (!mi.refurl.isEmpty()) {
    addRecent(mi.refurl);
    recentSource_ = mi.refurl;
    recentSourceLocked_ = true;
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
      new slot_::ImportAnnotationsFromUrl(mi.suburl, this),
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

  dataManager_->token().setId(0);
  dataManager_->removeAliases();
  annotationView_->invalidateAnnotations();

}

void
MainWindow::openStreamUrl(const QString &rtsp)
{ openMrl(rtsp, false); }

void
MainWindow::openMrl(const QString &path, bool checkPath)
{
  DOUT("enter: path =" << path);

  disableNavigation();

  if (!annotationView_->trackedWindow()) {
    if (!isVisible())
      show();
    if (isMinimized())
      showNormal();
  }

  bool fullScreen = hub_->isLiveTokenMode() && hub_->isEmbeddedPlayerMode();
  hub_->setMediaTokenMode();
  if (fullScreen) {
    hub_->setFullScreenWindowMode();
    updateWindowMode();
  }

  recentDigest_.clear();
  if (!recentSourceLocked_)
    recentSource_.clear();

  setRecentOpenedFile(path);

  if (!recentSource_.isEmpty())
    setBrowsedUrl(recentSource_);
  else
    setBrowsedFile(path);

  if (checkPath && !isRemoteMrl(path)) {
    QFileInfo fi(removeUrlPrefix(path));
    if (!fi.exists()) {
      warn(TR(T_ERROR_BAD_FILEPATH) + ": " + path);
      DOUT("exit: path not exist: " + path);
      return;
    }
  }

  tokenType_ = fileType(path);

  if (!player_->isValid())
    resetPlayer();

  DOUT("invoke openMedia");
  if (player_->hasMedia())
    closeMedia();

  bool ok = player_->openMedia(path);
  if (!ok) {
    warn(tr("failed to open media") + ": " + path);
    DOUT("exit: openMedia failed");
    return;
  }

  if (!isRemoteMrl(path))
    addRecent(path);
  if (player_->hasPlaylist()) {
    DOUT("exit: media has playlist");
    return;
  }

  if (player_->hasMedia()) {
    if (player_->isMouseEventEnabled())
      player_->startVoutTimer();
    DOUT("invoke invalidateMediaAndPlay");
    invalidateMediaAndPlay();
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

//#ifdef Q_WS_WIN
//  if (path.endsWith(':') || path.endsWith(":\\") || path.endsWith(":/"))
//    if (path.startsWith(PLAYER_URL_CD) ||
//        dataManager_->token().type() == Token::TT_Audio)
//      return false;
//#endif // Q_WS_WIN

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
      showMessage(tr("analyzing media ...")); // might cause parallel contension
      QThreadPool::globalInstance()->start(new task_::invalidateMediaAndPlay(this));
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

#ifdef Q_WS_X11
    if (hub_->isFullScreenWindowMode())
      AcUi::globalInstance()->setBlackBackground(this);
#endif // Q_WS_X11

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
    showMessage(TR(T_ERROR_NO_MEDIA))   ;
    return;
  }

  if (!QFileInfo(path).exists()) {
    warn(TR(T_ERROR_BAD_FILEPATH) + ": " + path);
    return;
  }

  bool ok = player_->openSubtitle(path);
  if (!ok)
    warn(TR(T_ERROR_BAD_SUBTITLE) + ": " + path);
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
    break;
  }
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
    closeChannel();
    //hub_->setMediaTokenMode();
    break;

  case SignalHub::SignalTokenMode:
#ifdef WITH_WIN_TEXTHOOK
    messageHandler_->setActive(false);
    showMessage(": " + tr("detaching all processes ..."));
    TextHook::globalInstance()->detachAllProcesses();
#endif // WITH_WIN_TEXTHOOK
    hub_->setNormalPlayerMode();
    break;
  }
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
    if (!annotationView_->isStarted() || !liveTimer_->isActive())
      openChannel();
    break;

  case SignalHub::SignalTokenMode:
#ifdef USE_MODE_SIGNAL
    messageHandler_->setActive(true);
#endif // USE_MODE_SIGNAL
    break;
  }
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
      QTime time = QtExt::msecs2time(player_->time());

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
        showMessage(tr("saved") + ": " + savePath);
      } else
        warn(tr("failed to save snapshot") + ": " + savePath);
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
    showMessage(tr("snapshot saved on the destop"));
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
  QWidget *w = 0;
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
  if (!helpDialog_) {
    helpDialog_ = new HelpDialog(this);
    windows_.append(helpDialog_);
  }
  helpDialog_->show();
  helpDialog_->raise();
}

void
MainWindow::update()
{
  showMessage(tr("openning update URL ...") + " " G_UPDATEPAGE_URL);
  QDesktopServices::openUrl(QUrl(G_UPDATEPAGE_URL));
}

// - Update -

QString
MainWindow::downloadSpeedToString(int speed)
{
  if (speed < 1024)
    return QString::number(speed) + " B/s";
  else if (speed < 1024 * 1024)
    return QString::number(speed / 1024) + " KB/s";
  else
    return QString::number(speed / (1024.0 * 1024), 'f', 2) + " MB/s";
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

  QString title = newWindowTitle();
  bool finished = false;

  if (totalBytes > 0) {
    bool stopped = receivedBytes == totalBytes;
    title += " (";
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
        QString ts = QtExt::msecs2time(remainingTime).toString();
        if (!ts.isEmpty())
          message += ", " + ts;
      }
    }
    title += message + ")";
    emit progressMessageChanged(message);
  }
  emit windowTitleToChange(title);
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
MainWindow::newWindowTitle() const
{
  QString ret;
  switch (hub_->tokenMode()) {
  case SignalHub::SignalTokenMode:
#ifdef USE_MODE_SIGNAL
    ret = messageHandler_->processInfo().processName;
#endif // USE_MODE_SIGNAL
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
    }
    break;
  }

  if (ret.isEmpty())
    ret = TR(T_TITLE_PROGRAM);
  return ret;
}

void
MainWindow::updateWindowTitle()
{ setWindowTitle(newWindowTitle()); }

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
MainWindow::showLoginDialog()
{ setLoginDialogVisible(true); }

void
MainWindow::hideLoginDialog()
{ setLoginDialogVisible(false); }

void
MainWindow::setAnnotationCountDialogVisible(bool visible)
{
  if (!annotationCountDialog_) {
    annotationCountDialog_ = new AnnotationCountDialog(dataManager_, this);
    connect(annotationCountDialog_, SIGNAL(countChanged(int)), annotationFilter_, SLOT(setAnnotationCountHint(int)));
    annotationCountDialog_->setCount(annotationFilter_->annotationCountHint());
    windows_.append(annotationCountDialog_);
  }
  annotationCountDialog_->setVisible(visible);
  if (visible)
    annotationCountDialog_->raise();
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
MainWindow::setLoginDialogVisible(bool visible)
{
  if (!loginDialog_) {
    loginDialog_ = new LoginDialog(this);
    windows_.append(loginDialog_);
    connect(loginDialog_, SIGNAL(loginRequested(QString, QString)), SLOT(login(QString, QString)));
  }
  if (visible) {
    loginDialog_->setUserName(server_->user().name());
    if (server_->user().name() == User::guest().name())
      loginDialog_->setPassword(User::guest().password());
    else
      loginDialog_->setPassword(QString());
    loginDialog_->show();
    loginDialog_->raise();
  } else
    loginDialog_->hide();
}

void
MainWindow::setUserViewVisible(bool visible)
{
  if (!userView_) {
    userView_ = new UserView(this);
    userView_->setUser(server_->user());
    windows_.append(userView_);

    connect(this, SIGNAL(userChanged(User)), userView_, SLOT(setUser(User)));
  }
  //if (visible) {
  //  loginDialog_->setUserName(server_->user().name());
  //  loginDialog_->setPassword(server_->user().password());
  //  loginDialog_->show();
  //  loginDialog_->raise();
  //} else
  //  loginDialog_->hide();
  userView_->setVisible(visible);
  if (visible)
    userView_->raise();
}

void
MainWindow::setTokenViewVisible(bool t)
{
  if (!tokenView_) {
    tokenView_ = new TokenView(dataManager_, server_, this);
    windows_.append(tokenView_);

    connect(tokenView_, SIGNAL(aliasSubmitted(Alias)), SLOT(submitAlias(Alias)));
    connect(tokenView_, SIGNAL(tokenBlessedWithId(qint64)), SLOT(blessTokenWithId(qint64)));
    connect(tokenView_, SIGNAL(tokenCursedWithId(qint64)), SLOT(curseTokenWithId(qint64)));
  }

  tokenView_->setVisible(t);
  if (tokenView_->isVisible())
    tokenView_->raise();
}

void
MainWindow::toggleTokenViewVisible()
{
  bool v = tokenView_ && tokenView_->isVisible();
  setTokenViewVisible(!v);
}

void
MainWindow::setAnnotationBrowserVisible(bool t)
{
  if (!annotationBrowser_) {
    annotationBrowser_ = new AnnotationBrowser(hub_, this);
    windows_.append(annotationBrowser_);

    connect(player_, SIGNAL(mediaChanged()), annotationBrowser_, SLOT(clear()));
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
#ifdef USE_MODE_SIGNAL
    connect(messageHandler_, SIGNAL(messageReceivedWithId(qint64)), annotationBrowser_, SLOT(setAnnotationPos(qint64)));
#endif // USE_MODE_SIGNAL

    annotationBrowser_->setUserId(server_->user().id());
    if (dataManager_->hasAnnotations())
      annotationBrowser_->setAnnotations(dataManager_->annotations());
  }

  annotationBrowser_->setVisible(t);
  if (annotationBrowser_->isVisible())
    annotationBrowser_->raise();
}

void
MainWindow::toggleAnnotationBrowserVisible()
{
  bool v = annotationBrowser_ && annotationBrowser_->isVisible();
  setAnnotationBrowserVisible(!v);
}

void
MainWindow::setBlacklistViewVisible(bool visible)
{
  if (!blacklistView_) {
    blacklistView_ = new BlacklistView(annotationFilter_, this);
    windows_.append(blacklistView_);
  }
  blacklistView_->setVisible(visible);
  if (visible)
    blacklistView_->raise();
}

BacklogDialog*
MainWindow::backlogDialog()
{
  if (!backlogDialog_) {
    backlogDialog_ = new BacklogDialog(this);
    windows_.append(backlogDialog_);
    connect(hub_, SIGNAL(tokenModeChanged(SignalHub::TokenMode)), backlogDialog_, SLOT(clear()));
    connect(player_, SIGNAL(mediaChanged()), backlogDialog_, SLOT(clear()));
    connect(annotationView_, SIGNAL(subtitleAdded(QString)), backlogDialog_, SLOT(appendSubtitle(QString)));
    connect(annotationView_, SIGNAL(annotationAdded(QString)), backlogDialog_, SLOT(appendAnnotation(QString)));
#ifdef USE_MODE_SIGNAL
    connect(messageHandler_, SIGNAL(messageReceivedWithText(QString)), backlogDialog_, SLOT(appendText(QString)));
#endif // USE_MODE_SIGNAL
  }
  return backlogDialog_;
}

void
MainWindow::setBacklogDialogVisible(bool visible)
{
  backlogDialog()->setVisible(visible);
  if (visible)
    backlogDialog()->raise();
}

/*
DownloadDialog*
MainWindow::downloadDialog()
{
  if (!downloadDialog_) {
    QWidget *parent = 0; // orphan window
#ifndef Q_WS_MAC
    parent = this;
#endif // !Q_WS_MAC
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
    connect(LoggerSignals::globalInstance(), SIGNAL(message(QString)),
            consoleDialog_, SLOT(appendLogText(QString)), Qt::QueuedConnection);
    connect(LoggerSignals::globalInstance(), SIGNAL(warning(QString)),
            consoleDialog_, SLOT(appendLogText(QString)), Qt::QueuedConnection);
    connect(LoggerSignals::globalInstance(), SIGNAL(notification(QString)),
            consoleDialog_, SLOT(appendLogText(QString)), Qt::QueuedConnection);
    connect(LoggerSignals::globalInstance(), SIGNAL(errorMessage(QString)),
            consoleDialog_, SLOT(appendLogText(QString)), Qt::QueuedConnection);

    Application::globalInstance()->addMessageHandler(ConsoleDialog::messageHandler);
  }
  return consoleDialog_;
}

void
MainWindow::setConsoleDialogVisible(bool visible)
{
  consoleDialog()->setVisible(visible);
  if (visible)
    consoleDialog()->raise();
}

void
MainWindow::setAnnotationEditorVisible(bool visible)
{
  if (!annotationEditor_) {
    annotationEditor_ = new AnnotationEditor(this);
    windows_.append(annotationEditor_);
    connect(annotationEditor_, SIGNAL(textSaved(QString)), SLOT(eval(QString)));
  }
  annotationEditor_->setVisible(visible);
  if (visible)
    annotationEditor_->raise();
}

void
MainWindow::toggleAnnotationEditorVisible()
{
  bool v = annotationEditor_ && annotationEditor_->isVisible();
  setAnnotationEditorVisible(!v);
}

//void
//MainWindow::invalidateSiteAccounts()
//{
//  if (!siteAccountView_)
//    return;
//  showMessage(tr("site accounts updated"));
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
MainWindow::setMediaInfoViewVisible(bool visible)
{
  if (!mediaInfoView_) {
    mediaInfoView_ = new MediaInfoView(player_, dataManager_, hub_, this);
    windows_.append(mediaInfoView_);
  }
  mediaInfoView_->setVisible(visible);
  if (visible)
    mediaInfoView_->raise();
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
MainWindow::showSeekDialog()
{ setSeekDialogVisible(true); }

void
MainWindow::hideSeekDialog()
{ setSeekDialogVisible(false); }

void
MainWindow::setSeekDialogVisible(bool visible)
{
  if (!seekDialog_) {
    seekDialog_ = new SeekDialog(this);
    windows_.append(seekDialog_);
    connect(seekDialog_, SIGNAL(seekRequested(qint64)), SLOT(seek(qint64)));
    connect(seekDialog_, SIGNAL(warning(QString)), SLOT(warn(QString)));
  }

  if (visible) {
    qint64 t = player_->hasMedia() ? player_->time() : 0;
    seekDialog_->setTime(t);
    seekDialog_->show();
    seekDialog_->raise();
  } else
    seekDialog_->fadeOut();
}

void
MainWindow::showLiveDialog()
{ setLiveDialogVisible(true); }

void
MainWindow::hideLiveDialog()
{ setLiveDialogVisible(false); }

void
MainWindow::setLiveDialogVisible(bool visible)
{
  liveDialog_->setVisible(visible);
  if (visible)
    liveDialog_->raise();
}

void
MainWindow::showSyncDialog()
{ setSyncDialogVisible(true); }

void
MainWindow::hideSyncDialog()
{ setSyncDialogVisible(false); }

void
MainWindow::setSyncDialogVisible(bool visible)
{
  if (!syncDialog_) {
    syncDialog_ = new SyncDialog(this);
    windows_.append(syncDialog_);
  }

  syncDialog_->setVisible(visible);
  if (visible)
    syncDialog_->raise();
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
MainWindow::setWindowPickDialogVisible(bool visible)
{
  if (!windowPickDialog_) {
    windowPickDialog_ = new PickDialog(this);
    windowPickDialog_->setMessage(tr("Select annots window"));
    connect(windowPickDialog_, SIGNAL(windowPicked(WId)), SIGNAL(windowPicked(WId)));
    connect(windowPickDialog_, SIGNAL(cancelled()), SLOT(updateOsdWindowOnTop()));
    windows_.append(windowPickDialog_);

    // FIXME
    osdWindow_->setWindowOnTop();
  }
  windowPickDialog_->setVisible(visible);
  if (visible)
    windowPickDialog_->raise();
}

void
MainWindow::setProcessPickDialogVisible(bool visible)
{
  if (!processPickDialog_) {
    processPickDialog_ = new PickDialog(this);
    windows_.append(processPickDialog_);
    processPickDialog_->setMessage(tr("Select process window to open"));

    connect(processPickDialog_, SIGNAL(windowPicked(WId)), SIGNAL(windowPicked(WId)));
    connect(processPickDialog_, SIGNAL(cancelled()), SLOT(updateOsdWindowOnTop()));
#ifdef USE_MODE_SIGNAL
    connect(processPickDialog_, SIGNAL(windowPicked(WId)), SLOT(openProcessWindow(WId)));
#endif // USE_MODE_SIGNAL
    //connect(processPickDialog_, SIGNAL(windowPicked(WId)), osdWindow_, SLOT(setWindowOnTop()));

    // FIXME
    osdWindow_->setWindowOnTop();
  }
  processPickDialog_->setVisible(visible);
  if (visible)
    processPickDialog_->raise();
}

// - Annotations -

void
MainWindow::submitAliasText(const QString &text, qint32 type, bool async)
{
  if (!server_->isConnected() && server_->isAuthorized()) {
    warn(tr("please log in to save alias online") + ": " + text);
    return;
  }
  const Token &t = dataManager_->token();
  if (!t.isValid() && !t.hasDigest()) {
    warn(tr("alias not saved for unknown media token") + ": " + text);
    return;
  }
  showMessage(tr("saving alias ...") + ": " + text);
  Alias a;
  a.setTokenId(t.id());
  a.setTokenDigest(t.digest());
  a.setTokenPart(t.part());

  a.setText(text);
  a.setType(type);
  a.setUserId(server_->user().id());
  a.setLanguage(server_->user().language());
  a.setUpdateTime(::time(0));
  if (tokenView_) // FIXME: use signals here
    tokenView_->addAlias(a);
  submitAlias(a, async);
}

void
MainWindow::submitAlias(const Alias &input, bool async)
{
  DOUT("enter: async =" << async << ", text =" << input.text());
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (dataManager_->aliasConflicts(input)) {
    warn(tr("similar alias already exists") + ": " + input.text());
    DOUT("exit: alias conflics");
    return;
  }
  if (input.type() == Alias::AT_Url) {
    if (!isRemoteMrl(input.text())) {
      warn(tr("source alias is not a valid URL") + ": " + input.text());
      return;
    }
    openAnnotationUrl(input.text());
  }
  if (async) {
    showMessage(tr("connecting server to submit alias ..."));
    QThreadPool::globalInstance()->start(new task_::submitAlias(input, this));
    DOUT("exit: returned from async branch");
    return;
  }

  dataManager_->addAlias(input);

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  qint64 id = dataServer_->submitAlias(input);
  Q_UNUSED(id);
  DOUT("alias id =" << id);
  //if (id)
    showMessage(tr("alias saved") + ": " + input.text());
  //else
  //  warn(tr("failed to update alias text") + ": " + a.text());

  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");
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

#ifdef USE_MODE_SIGNAL
  static const QStringList supportedProgramSuffices =
      QStringList() G_FORMAT_PROGRAM_(<<);
  foreach (const QString &suffix, supportedProgramSuffices)
    if (fileName.endsWith("." + suffix, Qt::CaseInsensitive))
      return Token::TT_Program;
#endif // USE_MODE_SIGNAL

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
  DOUT("enter: async =" << async);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (!server_->isConnected()) {
    emit warning(tr("not connected to Internet"));
    DOUT("exit: not connected");
    return;
  }
  if (!dataManager_->token().hashId()) {
    emit message(tr("unknown token"));
    DOUT("exit: missing token ID");
    return;
  }
  if (async) {
    showMessage(tr("updating annotations ..."));
    annotationView_->removeAnnotations();
    QThreadPool::globalInstance()->start(new task_::updateAnnotations(this));
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");
  AnnotationList annots = dataServer_->selectAnnotationsWithToken(dataManager_->token(), true); // true = invalidateCache

  QString url = dataManager_->token().source();
  if (MrlAnalysis::matchSite(url)) {
    if (async)
      emit openAnnotationUrlRequested(url);
    else
      openAnnotationUrl(url);
  }

  openAnnotationUrlFromAliases(dataManager_->aliases());
  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");

  if (!annots.isEmpty()) {
    QString src = "http://annot.me";
    QString msg = QString("%1 :" HTML_STYLE_OPEN(color:red) "+%2" HTML_STYLE_CLOSE() ", %3")
      .arg(tr("annotations found"))
      .arg(QString::number(annots.size()))
      .arg(src);
    emit message(msg);
    emit setAnnotationsRequested(annots);
  }

  DOUT("exit");
}

void
MainWindow::invalidateToken(const QString &mrl)
{
  DOUT("enter: mrl =" << mrl);
  if (!recentDigest_.isEmpty() || isDigestReady(mrl)) {
    dataManager_->token().setId(0);
    dataManager_->removeAliases();
    annotationView_->invalidateAnnotations();

    if (recentDigest_.isEmpty())
      recentDigest_ = digestFromFile(removeUrlPrefix(mrl));
    if (!recentDigest_.isEmpty())
      setToken(mrl);

  } else if (!recentSource_.isEmpty()) {
    dataManager_->token().setId(0);
    dataManager_->removeAliases();
    annotationView_->invalidateAnnotations();
    setToken();

  } else if (player_->trackCount() != 1) {
    dataManager_->token().setId(0);
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
  QString url = dataManager_->token().source();
  DOUT("url =" << url);
  if (url.isEmpty())
    warn(tr("login is requied to to submit annotation URL"));
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
    warn(tr("URL is too long") + ": " + url);
    return;
  }
  if (async) {
    showMessage(tr("signing media ...") + " " + path);
    QThreadPool::globalInstance()->start(new task_::signFileWithUrl(path, url, this));
    DOUT("exit: returned from async branch");
    return;
  }

  QString digest = digestFromFile(path);
  if (digest.isEmpty()) {
    warn(tr("failed to analyze media") + ": " + path);
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
      fileName = fileName.mid(0, Traits::MAX_ALIAS_LENGTH);
      warn(TR(T_WARNING_LONG_STRING_TRUNCATED) + ": " + fileName);
    }
    Alias a;
    a.setUserId(server_->user().id());
    a.setType(Alias::AT_Source);
    a.setLanguage(server_->user().language());
    a.setText(fileName);
    a.setUpdateTime(now);
    aliases.append(a);
  }

  if (!url.isEmpty()) {
    Alias a;
    a.setType(Alias::AT_Url);
    a.setUserId(server_->user().id());
    qint32 lang = Alias::guessUrlLanguage(url, server_->user().language());
    a.setLanguage(lang);
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
    showMessage(tr("connecting server to query media/game token ..."));
    QThreadPool::globalInstance()->start(new task_::setToken(input, this));
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  //Token token = dataManager_->token();
  dataManager_->removeToken();

  Token token;
  Alias alias;

  if (!input.isEmpty()) {
    token.setType(tokenType_ = fileType(input));

    QString path = removeUrlPrefix(input);
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
            fileName = fileName.mid(0, Traits::MAX_ALIAS_LENGTH);
            warn(TR(T_WARNING_LONG_STRING_TRUNCATED) + ": " + fileName);
          }

          alias.setType(Alias::AT_Source);
          alias.setUserId(server_->user().id());
          alias.setLanguage(server_->user().language());
          alias.setText(fileName);
          alias.setUpdateTime(QDateTime::currentMSecsSinceEpoch() / 1000);
        }
      }
    }
  } else if (!recentDigest_.isEmpty()) {
    token.setDigest(recentDigest_);
  } else if (!recentSource_.isEmpty()) {
    token.setSource(recentSource_);
    token.setType(tokenType_ = Token::TT_Video);
    int part = currentPlaylistIndex();
    if (part < 0)
      part = 0;
    token.setPart(part);
    QString title = player_->mediaTitle();
    if (!title.isEmpty() && !isRemoteMrl(title)) {
      if (recentUrlTitles_.size() > RECENT_COUNT)
        recentUrlTitles_.erase(recentUrlTitles_.begin());
      recentUrlTitles_[recentSource_] = title;
      alias.setType(Alias::AT_Name);
      alias.setUserId(server_->user().id());
      alias.setLanguage(server_->user().language());
      alias.setText(title);
      alias.setUpdateTime(QDateTime::currentMSecsSinceEpoch() / 1000);
    }
  }

 if (//!token.hasType() ||
     !token.hasDigest() && !token.hasSource()) {
    DOUT("inetMutex unlocking");
    inetMutex_.unlock();
    DOUT("inetMutex unlocked");
    DOUT("exit from empty digest or tokenType branch");
    return;
  }

  token.setCreateTime(QDateTime::currentMSecsSinceEpoch() / 1000);

  if (hub_->isMediaTokenMode() && player_->hasMedia() && token.hasDigest()) {
    int part = 0;
    if (player_->titleCount() > 1)
      part = player_->titleId();
    //else if (player_->hasPlaylist())
    //  part = player_->trackNumber();

    if (part < 0)
      part = 0;
    token.setPart(part);
  }

  AliasList aliases;
  if (alias.hasText())
    aliases.append(alias);

#ifdef USE_MODE_SIGNAL
  if (hub_->isSignalTokenMode()) {
    WId hwnd = annotationView_->trackedWindow();
    if (hwnd && QtWin::getWindowProcessId(hwnd) != QCoreApplication::applicationPid()) {
      QString t = QtWin::getWindowText(hwnd).trimmed();
      if (!t.isEmpty()) {
        DOUT("galgame window title =" << t);
        Alias a;
        a.setType(Alias::AT_Name);
        a.setUserId(server_->user().id());
        a.setLanguage(server_->user().language());
        a.setText(t);
        a.setUpdateTime(QDateTime::currentMSecsSinceEpoch() / 1000);
        aliases.append(a);
      }
    }
  }
#endif // USE_MODE_SIGNAL

  qint64 tid = 0;
  if (submit_)
    tid = dataServer_->submitTokenAndAliases(token, aliases);
  //alias.setTokenId(tid)
  if (tid) {
    Token t = dataServer_->selectTokenWithId(tid);
    if (t.isValid()) {
      //aliases = dataServer_->selectAliasesWithTokenId(tid);
      token = t;
    } //else
      //warn(TR(T_ERROR_SUBMIT_TOKEN) + ": " + input);
  }

  if (!token.isValid() && cache_->isValid() && token.hasDigest()) {
    showMessage(tr("searching for token in cache ..."));
    Token t = cache_->selectTokenWithDigest(token.digest(), token.part());
    if (t.isValid())
      token = t;
  }

  aliases = dataServer_->selectAliasesWithToken(token);

  dataManager_->setToken(token);
  dataManager_->setAliases(aliases);

  DOUT("token type =" << token.type() << ", part =" << token.part() << ", source =" << token.source());
  DOUT("alias count =" << aliases.size());

//  if (commentView_)
//    commentView_->setTokenId(token.id());
#ifdef USE_MODE_SIGNAL
  //signalView_->tokenView()->setAliases(aliases);
#endif // USE_MODE_SIGNAL

  AnnotationList annots = dataServer_->selectAnnotationsWithToken(token);
  //annotationBrowser_->setAnnotations(annots);
  //annotationView_->setAnnotations(annots);

  if (token.hasSource() || !dataServer_->preferLocal() || annots.isEmpty())
    openAnnotationUrlFromAliases(aliases);

  //if (tokens.size() == 1) {
  //  Token token = tokens.front();
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
    QString src = dataServer_->preferLocal() ? tr("offline database") : "http://annot.me";
    QString msg = QString("%1 :" HTML_STYLE_OPEN(color:red) "+%2" HTML_STYLE_CLOSE() ", %3")
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
  annot.setLanguage(server_->isAuthorized() ?
    server_->user().language() :
    Traits::AnyLanguage
  );
  annot.setText(text);
  annotationView_->showAnnotation(annot);
  DOUT("exit");
}

void
MainWindow::showTextAsSubtitle(const QString &input, bool isSigned)
{
  if (input.isEmpty())
    return;
  QString text = input;
  QString rest;

  // Split long text
  int n = int(annotationView_->width() * 0.9 / G_ANNOT_CHAR_WIDTH);
  if (n && text.size() > n) {
    rest = text.mid(n);
    text = text.left(n);
  }

  Annotation annot;
  if (isSigned && server_->isAuthorized()) {
    annot.setUserId(server_->user().id());
    annot.setUserAlias(server_->user().name());
    annot.setUserAnonymous(server_->user().isAnonymous());
  }
  annot.setLanguage(server_->isAuthorized() ?
    server_->user().language() :
    Traits::AnyLanguage
  );
  QString sub = CORE_CMD_SUB " " + text;
  annot.setText(sub)    ;
  annotationView_->showAnnotation(annot, false); // showMeta = false

  if (!rest.isEmpty())
    showTextAsSubtitle(rest);
}

void MainWindow::submitLiveText(const QString &text, bool async)
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
    showMessage(tr("connecting server to submit annot ..."));
    QThreadPool::globalInstance()->start(new task_::submitLiveText(text, this));
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
      annot.setText(text.mid(0, Traits::MAX_ANNOT_LENGTH));
      warn(TR(T_WARNING_LONG_STRING_TRUNCATED) + ": " + annot.text());
    }
  }

  qint64 id = server_->submitLiveAnnotation(annot);

  if (id)
    annot.setId(id);
  else
    warn(TR(T_ERROR_SUBMIT_ANNOTATION) + ": " + text);

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

void
MainWindow::submitText(const QString &text, bool async)
{
  DOUT("enter: async =" << async << ", text =" << text);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (!server_->isAuthorized() ||
      !dataManager_->token().hasDigest() && !dataManager_->token().hasSource() ||
      hub_->isMediaTokenMode() && !player_->hasMedia()
#ifdef USE_MODE_SIGNAL
      || hub_->isSignalTokenMode() && !messageHandler_->lastMessageHash().isValid()
#endif // USE_MODE_SIGNAL
     ) {

    showText(text, true); // isSigned = true
    DOUT("exit: returned from showText branch");
    return;
  }

  Q_ASSERT(!hub_->isLiveTokenMode());

  if (async) {
    showMessage(tr("connecting server to submit annot ..."));
    QThreadPool::globalInstance()->start(new task_::submitText(text, this));
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");
  Annotation annot; {
    annot.setTokenId(dataManager_->token().id());
    annot.setTokenDigest(dataManager_->token().digest());
    annot.setTokenPart(dataManager_->token().part());
    annot.setUserId(server_->user().id());
    annot.setUserAlias(server_->user().name());
    annot.setUserAnonymous(server_->user().isAnonymous());
    annot.setLanguage(server_->user().language());
    annot.setCreateTime(QDateTime::currentMSecsSinceEpoch() / 1000);
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
  #ifdef USE_MODE_SIGNAL
      annot.setPos(messageHandler_->lastMessageHash().hash);
      annot.setPosType(messageHandler_->lastMessageHash().count);
  #endif // USE_MODE_SIGNAL
      break;
    }

    if (text.size() <= Traits::MAX_ANNOT_LENGTH)
      annot.setText(text);
    else {
      annot.setText(text.mid(0, Traits::MAX_ANNOT_LENGTH));
      warn(TR(T_WARNING_LONG_STRING_TRUNCATED) + ": " + annot.text());
    }
  }

  qint64 id = dataServer_->submitAnnotation(annot);
  if (id)
    annot.setId(id);
  //else
  //  warn(TR(T_ERROR_SUBMIT_ANNOTATION) + ": " + text);

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
  else if (hub_->isLiveTokenMode() && server_->isConnected() && server_->isAuthorized())
    submitLiveText(text);
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
    //log(tr("connecting server to submit chat text ..."));
    QThreadPool::globalInstance()->start(new task_::chat(input, this));
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  emit showTextRequested(CORE_CMD_COLOR_BLUE " " + text);
  emit said(text, "blue");

#ifdef WITH_MODULE_DOLL
  Q_ASSERT(doll_);
  doll_->chat(text);
#else
  if (server_->isConnected())
    emit response(server_->chat(text));
#endif // WITH_MODULE_DOLL

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
    showMessage(text);
  else
    showMessage(html_style(text, "color:" + color));
  DOUT("exit");
}

void
MainWindow::warn(const QString &text)
{ Logger::warn(text); }

void
MainWindow::notify(const QString &text)
{ Logger::notify(text); }

void
MainWindow::showMessage(const QString &text)
{ Logger::log(text); }

void
MainWindow::error(const QString &text)
{ Logger::error(text); }

// - Events -

bool
MainWindow::event(QEvent *e)
{
  Q_ASSERT(e);
  bool accept = true;
  switch (e->type()) {
  case QEvent::FileOpen: // See: http://www.qtcentre.org/wiki/index.php?title=Opening_documents_in_the_Mac_OS_X_Finder
    {
      QFileOpenEvent *fe = static_cast<QFileOpenEvent *>(e);
      Q_ASSERT(fe);
      QString url = fe->file();
      if (url.isEmpty()) {
        url = fe->url().toString();
        if (!url.isEmpty())
          url = QUrl::fromPercentEncoding(url.toLocal8Bit());
      }
      if (!url.isEmpty())
        QTimer::singleShot(0, new slot_::OpenSource(url, this), SLOT(trigger()));
    } break;
  case QEvent::Gesture: gestureEvent(static_cast<QGestureEvent *>(e)); break;
  default: accept = Base::event(e);
  }
  return accept;
}

void
MainWindow::setVisible(bool visible)
{
  //DOUT("enter: visible =" << visible);
  //if (visible == isVisible())
  //  return;
  if (!isMinimized() && !isFullScreen()) { // invoked by showNormal or showMaximized
#ifdef WITH_WIN_DWM
    if (AcUi::isAeroAvailable())
      AcUi::globalInstance()->setWindowDwmEnabled(this, true);
      //QTimer::singleShot(0,
      //  new slot_::SetWindowDwmEnabled(true, this), SLOT(trigger())
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
  //DOUT("exit");
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
                                  isGlobalPosOverEmbeddedPositionSlider(globalPos));
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
        showMessage(tr("remove annotations"));
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
        showMessage(tr("capture annotations"));
        pauseRubberBand_->press(gp);
      } else if (shiftPressed) {
        annotationView_->setNearbyItemExpelled(true);
        annotationView_->setHoveredItemRemoved(true);
        annotationView_->removeItems(gp);
        annotationView_->expelItems(gp);
        Application::globalInstance()->setCursor(Qt::OpenHandCursor);
      } else if (hub_->isMediaTokenMode() && annotationView_->isItemVisible() && (
          hub_->isFullScreenWindowMode() ||
          !player_->isStopped() && dataManager_->hasAnnotations() //&& annotationView_->hasItems()
        ))
        annotationView_->setNearbyItemAttracted(true);
      else {
#ifdef Q_WS_MAC
        if (videoView_->isViewVisible() && player_->titleCount() > 1)
          videoView_->setViewMousePressPos(globalPos);
#endif // Q_WS_MAC
        if (!isFullScreen() && dragPos_ == BAD_POS)
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
      AnnotationSettings::globalInstance()->resetOffset();
    else if (event->modifiers() == Qt::ControlModifier
#ifdef Q_OS_WIN
        || QtWin::isKeyControlPressed()
#endif // Q_OS_WIN
    )
      AnnotationSettings::globalInstance()->resetScale();
    else if (event->modifiers() == Qt::AltModifier
#ifdef Q_OS_WIN
        || QtWin::isKeyAltPressed()
#endif // Q_OS_WIN
    )
      AnnotationSettings::globalInstance()->resetRotation();
    else {
      //removeRubberBand_->press(gp);
      annotationView_->setHoveredItemRemoved(true);
#ifdef Q_WS_MAC
      annotationView_->setNearbyItemExpelled(true);
#endif // Q_WS_MAC
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
#ifndef Q_WS_MAC
    if ((event->modifiers() & Qt::ControlModifier)
#ifdef Q_OS_WIN
        || QtWin::isKeyControlPressed()
#endif // Q_OS_WIN
    ) {
      removeRubberBand_->press(gp);
      cancelContextMenu_ = true;
    } else {
      annotationView_->setHoveredItemRemoved(true);
      annotationView_->setNearbyItemExpelled(true);
      if (event->modifiers()) {
        annotationView_->removeItems(gp);
        annotationView_->expelAllItems(gp);
        Application::globalInstance()->setCursor(Qt::PointingHandCursor);
        cancelContextMenu_ = true;
      }
    }
#endif // !Q_WS_MAC
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
#ifdef Q_WS_WIN
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
#endif // Q_WS_WIN
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

#ifndef Q_WS_MAC
  if (event->buttons() == Qt::RightButton && !contextMenu_->isVisible()) {
    QContextMenuEvent cm(QContextMenuEvent::Mouse, event->pos(), event->globalPos(), event->modifiers());
    QCoreApplication::sendEvent(this, &cm);
  }
#endif // !Q_WS_MAC

#ifdef Q_WS_MAC
  if (event->button() == Qt::LeftButton && // event->buttons() is always zero, qt4 bug?
      videoView_->isViewVisible() && player_->titleCount() > 1)
    videoView_->setViewMouseReleasePos(event->globalPos());
#endif // Q_WS_MAC

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
  return !embeddedPlayer_->isOnTop() &&
         pos.x() > r.left() && pos.x() < r.right() &&
  (//pos.y() > r.bottom() - margin && pos.y() < r.bottom() + margin || // At the bottom
   pos.y() > r.top() - margin &&
   pos.y() < r.bottom() - embeddedPlayer_->inputComboBox()->height() -10
  );
}

bool
MainWindow::isGlobalPosOverOsdConsole(const QPoint &pos) const
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
        (embeddedCanvas_->underMouse() || isGlobalPosOverEmbeddedPositionSlider(event->globalPos())));
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
      notify(tr("use %1 with mouse or wheel to control annotations")
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
    else if (dragPos_ != BAD_POS && !isFullScreen()
#ifdef Q_WS_WIN
        &&
        (
          player_->isStopped() ||
          QDateTime::currentMSecsSinceEpoch() - windowModeUpdateTime_ > QtWin::getDoubleClickInterval()
        )
#endif // Q_WS_WIN
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
             !globalOsdConsole_->isEmpty() && globalOsdConsole_->isVisible() &&
#ifndef Q_WS_WIN
        globalOsdConsole_->underMouse() &&
#endif // !Q_WS_WIN
        isGlobalPosOverOsdConsole(event->globalPos()))
      setConsoleDialogVisible(true);
    else if (hub_->isFullScreenWindowMode() &&
             embeddedPlayer_->isVisible() && embeddedCanvas_->isVisible() &&
             dataManager_->hasAnnotations() &&
#ifdef Q_WS_WIN
             isGlobalPosOverEmbeddedPositionSlider(event->globalPos())
#else
             embeddedCanvas_->underMouse()
#endif // Q_WS_WIN
             )
      showAnnotationAnalyticsView();
    else
      hub_->toggleFullScreenWindowMode();
    event->accept();
    break;
  case Qt::MiddleButton:
    annotationView_->attractAllItems(mapFromGlobal(event->globalPos()));
    break;
//  case Qt::RightButton:
//#ifndef Q_WS_MAC
//    {
//      QPoint gp = mapFromGlobal(event->globalPos());
//      annotationView_->removeItems(gp);
//      annotationView_->expelAllItems(gp);
//      cancelContextMenu_ = true;
//    }
//#endif // Q_WS_MAC
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
#ifdef Q_WS_WIN
    || QtWin::isMouseLeftButtonPressed()
#endif // Q_WS_WIN
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
#ifdef Q_WS_WIN
    || QtWin::isMouseRightButtonPressed()
#endif // Q_WS_WIN
  ) {
    cancelContextMenu_ = true;
    //enum { zoom = -3 };
    //qreal angle = event->delta() / (8.0 * zoom);
    qreal angle = event->delta() > 0 ? -5 : 5;
    if (!annotationView_->isPaused() && annotationView_->hasPausedItems())
      annotationView_->rotatePausedItems(angle);
    else
      AnnotationSettings::globalInstance()->setRotation(
        AnnotationSettings::globalInstance()->rotation() + angle
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
#ifndef Q_WS_MAC
  if (!cancelContextMenu_ &&
      (!resumeRubberBand_->isPressed() || resumeRubberBand_->isEmpty()) &&
      (!removeRubberBand_->isPressed() || removeRubberBand_->isEmpty()) &&
      (!pauseRubberBand_->isPressed() || pauseRubberBand_->isEmpty()))
#endif // !Q_WS_MAC
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
  DOUT("exit");
}

void
MainWindow::updateAnnotationSettingsMenu()
{
  updateAnnotationSubtitleMenu();
  updateAnnotationEffectMenu();
  togglePreferMotionlessAnnotationAct_->setChecked(AnnotationSettings::globalInstance()->preferMotionless());
  toggleAnnotationAvatarVisibleAct_->setChecked(AnnotationSettings::globalInstance()->isAvatarVisible());
  toggleAnnotationBandwidthLimitedAct_->setChecked(annotationView_->isItemCountLimited());
}

void
MainWindow::updateAnnotationSubtitleMenu()
{
  toggleSubtitleAnnotationVisibleAct_->setChecked(annotationView_->isSubtitleVisible());
  toggleNonSubtitleAnnotationVisibleAct_->setChecked(annotationView_->isNonSubtitleVisible());
  toggleSubtitleOnTopAct_->setChecked(isSubtitleOnTop());

  bool t = toggleSubtitleAnnotationVisibleAct_->isChecked();
  toggleSubtitleOnTopAct_->setEnabled(t);
}

void
MainWindow::updateGameMenu()
{
#ifdef USE_MODE_SIGNAL
  toggleSignalViewVisibleAct_->setChecked(signalView_->isVisible());
  toggleRecentMessageViewVisibleAct_->setChecked(recentMessageView_->isVisible());
  toggleTranslateAct_->setEnabled(hub_->isSignalTokenMode() && server_->isConnected());
#endif // USE_MODE_SIGNAL
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

  bool online = server_->isConnected() && server_->isAuthorized();

  {
    // Live mode
    if (online &&
        (hub_->isLiveTokenMode() || hub_->isStopped() && player_->isStopped())) {
      toggleLiveModeAct_->setChecked(hub_->isLiveTokenMode());
      contextMenu_->addAction(toggleLiveModeAct_);

      //toggleLiveDialogVisibleAct_->setChecked(liveDialog_->isVisible());
      //contextMenu_->addAction(toggleLiveDialogVisibleAct_);
    }
#ifndef Q_WS_MAC
    //contextMenu_->addAction(newWindowAct_);
#endif // !Q_WS_MAC
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
  {
    //contextMenu_->addSeparator();

    //contextMenu_->addAction(openAct_);

    openMenu_->clear(); {
      openMenu_->addAction(openFileAct_);
      openMenu_->addAction(openUrlAct_);
      openMenu_->addAction(openDeviceAct_);
      openMenu_->addAction(openVideoDeviceAct_);
#ifdef Q_WS_WIN // TODO add support for Mac/Linux
      openMenu_->addAction(openAudioDeviceAct_);
#endif // Q_WS_WIN

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
        BOOST_AUTO(a, new QtExt::ActionWithId(tid, text, sectionMenu_));
        contextMenuActions_.append(a);
        if (tid == player_->titleId()) {
#ifdef Q_WS_WIN
          a->setIcon(QIcon(RC_IMAGE_CURRENTSECTION));
#else
          a->setCheckable(true);
          a->setChecked(true);
#endif // Q_WS_WIN
        }
        connect(a, SIGNAL(triggeredWithId(int)), player_, SLOT(setTitleId(int)));
        sectionMenu_->addAction(a);
      }

      contextMenu_->addMenu(sectionMenu_);
    }

#ifdef USE_MODE_SIGNAL
    contextMenu_->addSeparator();
    contextMenu_->addMenu(gameMenu_);
    updateGameMenu();
#endif // USE_MODE_SIGNAL

    contextMenu_->addSeparator();
    if (hub_->isMediaTokenMode() && player_->hasMedia()) {
      contextMenu_->addAction(openAnnotationUrlAct_);

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
          BOOST_AUTO(a, new QtExt::ActionWithId(id, subtitle, subtitleMenu_));
          contextMenuActions_.append(a);
          if (id == player_->subtitleId()) {
#ifdef Q_WS_WIN
            a->setIcon(QIcon(RC_IMAGE_CURRENTSUBTITLE));
#else
            a->setCheckable(true);
            a->setChecked(true);
#endif // Q_WS_WIN
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
          BOOST_AUTO(a, new QtExt::ActionWithId(id, name, audioTrackMenu_));
          contextMenuActions_.append(a);
          if (id == player_->audioTrackId()) {
#ifdef Q_WS_WIN
            a->setIcon(QIcon(RC_IMAGE_CURRENTTRACK));
#else
            a->setCheckable(true);
            a->setChecked(true);
#endif // Q_WS_WIN
          }
          connect(a, SIGNAL(triggeredWithId(int)), player_, SLOT(setAudioTrackId(int)));
          audioTrackMenu_->addAction(a);
          id++;
        }

        contextMenu_->addMenu(audioTrackMenu_);
      }

      updateAspectRatioMenu();
      toggleMediaInfoViewVisibleAct_->setChecked(mediaInfoView_ && mediaInfoView_->isVisible());
      contextMenu_->addMenu(adjustMenu_);
    }
  }

  // Basic control
  contextMenu_->addSeparator();
  if (!hub_->isMediaTokenMode() || player_->hasMedia()) {
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

    toggleSeekDialogVisibleAct_->setChecked(seekDialog_ && seekDialog_->isVisible());
    contextMenu_->addAction(toggleSeekDialogVisibleAct_);
  }

  // Modes
  {
    contextMenu_->addSeparator();

#ifdef WITH_WIN_PICKER
    contextMenu_->addAction(showWindowPickDialogAct_);

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

      toggleBlacklistViewVisibleAct_->setChecked(blacklistView_ && blacklistView_->isVisible());
      contextMenu_->addAction(toggleBlacklistViewVisibleAct_);

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

    toggleAnnotationBrowserVisibleAct_->setChecked(annotationBrowser_ && annotationBrowser_->isVisible());
    contextMenu_->addAction(toggleAnnotationBrowserVisibleAct_ );

    updateAnnotationsAct_->setEnabled(dataManager_->token().isValid() && server_->isConnected());
    contextMenu_->addAction(updateAnnotationsAct_);

    annotationMenu_->clear(); {
      toggleTokenViewVisibleAct_->setChecked(tokenView_ && tokenView_->isVisible());
      annotationMenu_->addAction(toggleTokenViewVisibleAct_ );

      toggleBacklogDialogVisibleAct_->setChecked(backlogDialog_ && backlogDialog_->isVisible());
      annotationMenu_->addAction(toggleBacklogDialogVisibleAct_);

      toggleAnnotationAnalyticsViewVisibleAct_->setChecked(annotationAnalyticsView_ && annotationAnalyticsView_->isVisible());
      annotationMenu_->addAction(toggleAnnotationAnalyticsViewVisibleAct_);

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

  // Network
  {
    contextMenu_->addSeparator();
    if (!server_->isAuthorized())
      contextMenu_->addAction(loginAct_);
  }

//  if (ALPHA)
//  if (player_->hasMedia()
//#ifdef USE_MODE_SIGNAL
//      && hub_->isMediaTokenMode()
//#endif // USE_MODE_SIGNAL
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

      //int l = TranslatorManager::globalInstance()->language();
      int l = AcSettings::globalSettings()->language();
      setAppLanguageToJapaneseAct_->setChecked(l == QLocale::Japanese);
      //setAppLanguageToTraditionalChineseAct_->setChecked(l == TranslatorManager::TraditionalChinese);
      setAppLanguageToSimplifiedChineseAct_->setChecked(l == QLocale::Chinese);

      switch (l) {
      case QLocale::Japanese: case QLocale::Taiwan: case QLocale::Chinese:
        setAppLanguageToEnglishAct_->setChecked(false);
        break;
      case QLocale::English: default:
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
    toggleDownloaderVisibleAct_->setChecked(false);
    toggleMagnifierVisibleAct_->setChecked(magnifier_ && magnifier_->isVisible());
    toggleAnnotationEditorVisibleAct_->setChecked(annotationEditor_ && annotationEditor_->isVisible());
    contextMenu_->addMenu(utilityMenu_);

    toggleConsoleDialogVisibleAct_->setChecked(consoleDialog_ && consoleDialog_->isVisible());
    contextMenu_->addMenu(helpContextMenu_);
    contextMenu_->addAction(quitAct_);
  }
  //DOUT("exit");
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
MainWindow::updateSettingsMenu()
{
  togglePlayerLabelEnabled_->setChecked(embeddedCanvas_->isEnabled());
  toggleEmbeddedPlayerOnTopAct_->setChecked(embeddedPlayer_->isOnTop());
  //toggleSiteAccountViewVisibleAct_->setChecked(siteAccountView_ && siteAccountView_->isVisible());
  //toggleNetworkProxyDialogVisibleAct_->setChecked(networkProxyDialog_ && networkProxyDialog_->isVisible());
  toggleWindowOnTopAct_->setChecked(isWindowOnTop());
  toggleClipboardMonitorEnabledAct_->setChecked(clipboardMonitor_->isEnabled());
  togglePreferLocalDatabaseAct_->setChecked(dataServer_->preferLocal());

#ifdef Q_OS_LINUX
  toggleMenuBarVisibleAct_->setChecked(menuBar()->isVisible());
#endif // Q_OS_LINUX

  toggleSaveMediaAct_->setChecked(player_->isBufferSaved());
  toggleSubmitAct_->setChecked(submit_);

  toggleMenuThemeEnabledAct_->setChecked(AcUi::globalInstance()->isMenuEnabled());

  toggleMultipleWindowsEnabledAct_->setChecked(Settings::globalSettings()->isMultipleWindowsEnabled());

  bool themeEnabled = true;
#ifdef Q_WS_WIN
  if (QtWin::isWindowsVistaOrLater()) {
    bool aero = AcUi::globalInstance()->isAeroEnabled();
    toggleAeroEnabledAct_->setChecked(aero);
    themeEnabled = !aero;
    if (themeEnabled != themeMenu_->isEnabled()) {
      themeMenu_->setEnabled(themeEnabled);
      themeMenu_->setVisible(themeEnabled);
     }
  }
#endif // Q_WS_WIN
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
  //userMenu_->clear();
  if (!server_->isAuthorized())
    return;

  // Menu

  if (userMenu_->isEmpty()) {
    userMenu_->addAction(toggleUserAnonymousAct_);
    userMenu_->addMenu(userLanguageMenu_);

    userMenu_->addSeparator();
    userMenu_->addAction(toggleUserViewVisibleAct_);
    userMenu_->addAction(logoutAct_);
  }

  toggleUserViewVisibleAct_->setChecked(userView_ && userView_->isVisible());

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
    setUserLanguageToChineseAct_->setChecked(server_->user().language() == Traits::Chinese);
    setUserLanguageToKoreanAct_->setChecked(server_->user().language() == Traits::Korean);
    setUserLanguageToUnknownAct_->setChecked(server_->user().language() == Traits::UnknownLanguage);
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
    BOOST_AUTO(a, new QtExt::ActionWithId(mi.track, text, trackMenu_));
    a->setToolTip(mi.path);
    if (mi.track == player_->trackNumber()) {
#ifdef Q_WS_X11
      a->setCheckable(true);
      a->setChecked(true);
#else
      a->setIcon(QIcon(RC_IMAGE_CURRENTTRACK));
#endif // Q_WS_X11
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
  globalOsdConsole_->resize(annotationView_->size());
  osdWindow_->raise();
}

void
MainWindow::changeEvent(QEvent *event)
{
  Q_ASSERT(event);
  holdTimer_->stop();
  if (event->type() == QEvent::WindowStateChange && isMaximized())
    emit windowMaximized();
  Base::changeEvent(event);

#ifdef Q_WS_X11
  if (event->type() == QEvent::WindowStateChange) {
    if (windowState() == Qt::WindowFullScreen)
      QtX::setWindowInputShape(osdWindow_->winId(), embeddedPlayer_->pos(), embeddedPlayer_->rect());
    else
      QtX::zeroWindowInputShape(osdWindow_->winId());
  }
#endif // Q_WS_X11
}

// - Keyboard events -

void
MainWindow::keyReleaseEvent(QKeyEvent *event)
{
  //DOUT("enter");
#ifdef Q_WS_WIN
  switch (event->key()) {
  case Qt::Key_Shift: case Qt::Key_Alt: case Qt::Key_Control:
    updateAnnotationHoverGesture();
  }
#endif // Q_WS_WIN
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

#ifdef Q_WS_WIN
  case Qt::Key_CapsLock:
    menuBar()->setVisible(!menuBar()->isVisible());
    break;
#endif // Q_WS_WIN

/*
    // Modifiers
  case Qt::Key_Control:
    if (!isEditing()) {
      PlayerUi *ui = visiblePlayer();
      if (ui)
        ui->clearFocus();
      setFocus();
    }
#ifdef Q_WS_WIN
    updateAnnotationHoverGesture();
#endif // Q_WS_WIN
    Base::keyPressEvent(event);
    break;
  case Qt::Key_Shift:
    if (!isEditing()) {
      PlayerUi *ui = visiblePlayer();
      if (ui)
        ui->clearFocus();
      setFocus();
    }
#ifdef Q_WS_WIN
    updateAnnotationHoverGesture();
#endif // Q_WS_WIN
    Base::keyPressEvent(event);
    break;
#ifdef Q_WS_WIN
  case Qt::Key_Alt:
    {
      PlayerUi *ui = visiblePlayer();
      if (ui)
        ui->clearFocus();
      setFocus();
    }
#ifdef Q_WS_WIN
    updateAnnotationHoverGesture();
#endif // Q_WS_WIN
    Base::keyPressEvent(event);
    break;
#endif // Q_WS_WIN
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
#ifdef Q_WS_WIN
      QtWin::setFocus(visiblePlayer()->inputComboBox());
#else
      visiblePlayer()->inputComboBox()->setFocus();
#endif // Q_WS_WIN
    }
  default:
    Base::keyPressEvent(event);
  }
}

void
MainWindow::maximizedToFullScreen()
{
  if (isMaximized()) {
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
  DOUT("enter");

  enum { wait_player_stop_timeout = 2000 }; // 2 seconds
  int timeout = player_->isStopped() ? 0 : wait_player_stop_timeout;

  if (player_->hasMedia()) {
    //rememberAspectRatio();
    rememberPlayPos();
    rememberSubtitle();
    rememberAudioTrack();
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
        //if (BOOST_AUTO(p, qobject_cast<AcWindow *>(w)))
        //  p->fadeOut();
        //else if (BOOST_AUTO(p, qobject_cast<AcMainWindow *>(w)))
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
  DOUT("enter: disposed =" << disposed_);
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
  Settings *settings = Settings::globalSettings();
  AcSettings *ac = AcSettings::globalSettings();
  AnnotationSettings *annotationSettings = AnnotationSettings::globalInstance();
  AcUi *ui = AcUi::globalInstance();

  ac->dispose();

  //updateRecent();
#ifdef Q_WS_WIN
  //ac->setAeroEnabled(ui->isAeroEnabled());
#endif // Q_WS_WIN

  ac->setMenuThemeEnabled(ui->isMenuEnabled());

  settings->setRecentFiles(recentFiles_);
  settings->setRecentTitles(recentUrlTitles_);

  settings->setRecentPath(recentPath_);

  ac->setThemeId(ui->theme());

  settings->setMenuBarVisible(menuBar()->isVisible());

  settings->setAnnotationFilterEnabled(annotationFilter_->isEnabled());
  settings->setBlockedKeywords(annotationFilter_->blockedTexts());
  settings->setBlockedUserNames(annotationFilter_->blockedUserAliases());
  settings->setAnnotationLanguages(annotationFilter_->languages());
  //settings->setAnnotationCountHint(annotationFilter_->annotationCountHint());

  settings->setPreferLocalDatabase(dataServer_->preferLocal());

  settings->setSubtitleColor(subtitleColor());

  settings->setAnnotationBandwidthLimited(annotationView_->isItemCountLimited());
  settings->setAnnotationEffect(annotationView_->renderHint());
  settings->setAnnotationScale(annotationSettings->scale());
  settings->setAnnotationOffset(annotationSettings->offset());
  settings->setAnnotationFontFamily(annotationSettings->fontFamily());
  settings->setAnnotationAvatarVisible(annotationSettings->isAvatarVisible());
  settings->setPreferMotionlessAnnotation(annotationSettings->preferMotionless());

  // Disabled for saving closing time orz
  //if (server_->isConnected() && server_->isAuthorized())
  //  server_->logout();

  settings->setBufferedMediaSaved(player_->isBufferSaved());

  settings->setTranslateEnabled(isTranslateEnabled());

  settings->setEmbeddedPlayerOnTop(embeddedPlayer_->isOnTop());
  //settings->setPlayerLabelEnabled(embeddedCanvas_->isEnabled());

  //settings->setAutoPlayNext(isAutoPlayNext());

  settings->setLive(hub_->isLiveTokenMode());

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
  settings->setPlayPosHistory(playPosHistory_);

  settings->sync();

  ac->sync();

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
#if QT_VERSION >= 0x040800
    // wait for at most 5 seconds ant kill all threads
    enum { CloseTimeout = 5000 };
    QThreadPool::globalInstance()->waitForDone(CloseTimeout);
#else
    //DOUT("WARNING: killing active threads; will be fixed in Qt 4.8");
    QThreadPool::globalInstance()->waitForDone();
#endif  // QT_VERSION
  }

  //if (parentWidget())
  //  QTimer::singleShot(0, parentWidget(), SLOT(close()));

#ifdef Q_WS_WIN
  QTimer::singleShot(0, qApp, SLOT(quit())); // ensure quit app and clean up zombie threads
#endif // Q_WS_WIN

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
      if (hub_->isFullScreenWindowMode())
        showFullScreen();
      else
        show();
    }

//#ifdef Q_OS_MAC
//    if (t)
//      osdWindow_->ensureStaysOnTop();
//#endif // Q_OS_MAC

    if (t)
      showMessage(tr("always on top enabled"));
    else
      showMessage(tr("always on top disabled"));
  }

#ifdef Q_WS_WIN
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
#endif // Q_WS_WIN
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
    warn(tr("invalid cast id"));
    DOUT("exit from invalid parameter branch");
    return;
  }

  if (!server_->isConnected() || !server_->isAuthorized()) {
    warn(tr("cannot perform cast when offline"));
    DOUT("exit from offline branch");
    return;
  }

  if (async) {
    showMessage(tr("submit bless cast to token ..."));
    QThreadPool::globalInstance()->start(new task_::blessTokenWithId(id, this));
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  bool ok = server_->blessTokenWithId(id);
  if (ok) {
    showMessage(tr("token blessed"));
    if (dataManager_->token().id() == id) {
      dataManager_->token().blessedCount()++;
      //dataManager_->invalidateToken();
    }
  } else
    warn(tr("failed to bless token"));

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
    warn(tr("invalid cast id"));
    DOUT("exit from invalid parameter branch");
    return;
  }

  if (!server_->isConnected() || !server_->isAuthorized()) {
    warn(tr("cannot perform cast when offline"));
    DOUT("exit from offline branch");
    return;
  }

  if (async) {
    showMessage(tr("submit curse cast to token ..."));
    QThreadPool::globalInstance()->start(new task_::curseTokenWithId(id, this));
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  bool ok = server_->curseTokenWithId(id);
  if (ok) {
    showMessage(tr("token cursed"));
    if (dataManager_->token().id() == id) {
      dataManager_->token().cursedCount()++;
      //dataManager_->invalidateToken();
    }
  } else
    warn(tr("failed to curse token"));

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
    warn(tr("invalid cast id"));
    DOUT("exit from invalid parameter branch");
    return;
  }

  if (!server_->isConnected() || !server_->isAuthorized()) {
    warn(tr("cannot perform cast when offline"));
    DOUT("exit from offline branch");
    return;
  }
  if (id == server_->user().id()) {
    warn(tr("cannot perform cast to yourself"));
    DOUT("exit from self branch");
    return;
  }

  if (async) {
    showMessage(tr("blessing user ..."));
    QThreadPool::globalInstance()->start(new task_::blessUserWithId(id, this));
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  server_->blessUserWithId(id);
  showMessage(tr("user blessed"));

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
    warn(tr("invalid cast id"));
    DOUT("exit from invalid parameter branch");
    return;
  }

  if (!server_->isConnected() || !server_->isAuthorized()) {
    warn(tr("cannot perform cast when offline"));
    DOUT("exit from offline branch");
    return;
  }
  if (id == server_->user().id()) {
    warn(tr("cannot perform cast to yourself"));
    DOUT("exit from self branch");
    return;
  }

  if (async) {
    showMessage(tr("cursing user ..."));
    QThreadPool::globalInstance()->start(new task_::curseUserWithId(id, this));
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  server_->curseUserWithId(id);
  showMessage(tr("user cursed"));

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
    warn(tr("invalid cast id"));
    DOUT("exit from invalid parameter branch");
    return;
  }

  if (!server_->isConnected() || !server_->isAuthorized()) {
    warn(tr("cannot perform cast when offline"));
    DOUT("exit from offline branch");
    return;
  }
  if (id == server_->user().id()) {
    warn(tr("cannot perform cast to yourself"));
    DOUT("exit from self branch");
    return;
  }

  if (async) {
    showMessage(tr("blocking user ..."));
    QThreadPool::globalInstance()->start(new task_::blockUserWithId(id, this));
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  server_->blockUserWithId(id);
  showMessage(tr("user blocked"));

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
      warn(tr("invalid cast id")); \
      DOUT("exit from invalid parameter branch"); \
      return; \
    } \
\
    if (!server_->isConnected() || !server_->isAuthorized()) { \
      warn(tr("cannot perform cast when offline")); \
      DOUT("exit from offline branch"); \
      return; \
    } \
\
    if (async) { \
      showMessage(tr("casting spell to target ...")); \
      QThreadPool::globalInstance()->start(new task_::blockUserWithId(id, this)); \
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
  Q_UNUSED(async);
  //if (async) {
  //  showMessage(tr("connecting server to logout ..."));
  //  QThreadPool::globalInstance()->start(new task_logout(this));
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
    showMessage(tr("connecting to server ..."));
    QThreadPool::globalInstance()->start(new task_::checkInternetConnection(this));
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
    QThreadPool::globalInstance()->start(new task_::login(userName, encryptedPassword, this));
    DOUT("exit: returned from async branch");
    return;
  }

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
    showMessage(TR(T_MESSAGE_TRY_LOGINFROMCACHE));
    User user = cache_->selectUserWithNameAndPassword(userName, encryptedPassword);
    if (user.isValid()) {
      settings->setUserName(userName);
      settings->setPassword(encryptedPassword);

      server_->setAuthorized(true);
      server_->setUser(user);
      emit userIdChanged(user.id());
      showMessage(TR(T_SUCCEED_LOGINFROMCACHE));
    } else
      warn(TR(T_ERROR_LOGINFROMCACHE_FAILURE));

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
    liveInterval_ = server_->selectLiveTokenInterval();
    if (disposed_) {
      DOUT("inetMutex unlocking"); inetMutex_.unlock(); DOUT("inetMutex unlocked");
      DOUT("exit: returned from disposed branch");
      return;
    }
    if (liveInterval_ <= 0)
      liveInterval_ = DEFAULT_LIVE_INTERVAL;

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
      bool updated = server_->isSoftwareUpdated();
      if (!updated)
        notify(tr("new version released, please check Help/Update menu"));

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
      showMessage(tr("Sync mode started"));
    else
      showMessage(tr("Sync mode stopped"));
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
      showMessage(tr("Live mode started"));
    else
      showMessage(tr("Live mode stopped"));
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
    showMessage(tr("found process id for window") + QString(" (pid = %1)").arg(QString::number(pid)));
    openProcessId(pid);
  } else
    warn(tr("process id for window was not found"));
  openProcess();
}

void
MainWindow::openProcessId(ulong pid)
{
  if (!pid)
    return;

  if (TextHook::globalInstance()->isProcessAttached(pid))
    showMessage(tr("process was attached") + QString(" (pid = %1)").arg(pid));
  else {
    bool ok = TextHook::globalInstance()->attachProcess(pid);
    if (ok) {
      showMessage(tr("process attached") + QString(" (pid = %1)").arg(QString::number(pid)));
      signalView_->processView()->refresh();
      ProcessInfo pi = signalView_->processView()->attachedProcessInfo();
      if (pi.isValid() && pid == pi.processId)
        emit attached(pi);
      if (!annotationView_->trackedWindow() ||
          pi.processId != QtWin::getWindowProcessId(annotationView_->trackedWindow())) {
        WId hwnd = QtWin::getGoodWindowWithProcessId(pi.processId);
        if (hwnd)
          emit windowPicked(hwnd);
      }
    } else {
      error(tr("failed to attach process ") + QString(" (pid = %1)").arg(QString::number(pid)));
      if (!QtWin::isProcessActiveWithId(pid))
        notify(tr("Is the process running now?"));
      else if (!QtWinnt::isElevated())
        notify(tr("Run me as administrator and try again (o^^o)"));
      else
        notify(tr("Restart the target process might help -_-"));
    }
  }
}

void
MainWindow::openProcessPath(const QString &path)
{
  QString processName = QFileInfo(path).fileName();
  ulong pid = QtWinnt::getProcessIdByName(processName);
  if (pid)
    showMessage(tr("process was started") + QString(" (pid = %1)").arg(pid));
  else {
    QtWin::createProcessWithExecutablePath(path);
    showMessage(tr("told process to start") + QString(" (name = %1)").arg(processName));
    pid = QtWinnt::getProcessIdByName(processName);
  }

  addRecent(path);

  signalView_->processView()->refresh();
  if (!pid) {
    warn(tr("failed to start process") + QString(" (name = %1)").arg(processName));
    openProcess();
  } else {
    enum { seconds = G_OPENPROCESS_TIMEOUT / 1000 }; // msecs / 1000
    showMessage(tr("wait %1 seconds for process to start ...").arg(QString::number(seconds)));
    QTimer::singleShot(
      G_OPENPROCESS_TIMEOUT,
      new slot_::OpenProcessId(pid, this),
      SLOT(trigger())
    );
  }
}

void
MainWindow::openProcessHook(ulong hookId, const ProcessInfo &pi)
{
  DOUT("enter: hookId =" << hookId << ", pid =" << pi.processId << ", proc =" << pi.processName);
  if (player_->hasMedia() && !player_->isStopped())
    stop();

  if (pi.isValid()) {
    showMessage(tr("openning process") + ": " + pi.processName);

    if (grabber_)
      grabber_->setBaseName(pi.processName);

    QString title = pi.processName;
    setWindowTitle(title);
    miniPlayer_->setWindowTitle(title); // TODO: move to invalidateTitle!!! or hub_();

    addRecent(pi.executablePath);
    setRecentOpenedFile(pi.executablePath);

    if (hookId != messageHandler_->hookId())
      messageHandler_->clearRecentMessages();

    messageHandler_->setProcessInfo(pi);
    messageHandler_->setHookId(hookId);

    if (!annotationView_->trackedWindow() ||
        pi.processId != QtWin::getWindowProcessId(annotationView_->trackedWindow())) {
      WId hwnd = QtWin::getGoodWindowWithProcessId(pi.processId);
      if (hwnd)
        emit windowPicked(hwnd);
    }
  }

  hub_->setSignalTokenMode();

  QString mrl;
  if (pi.isValid()) {
    mrl = pi.executablePath;

    WId hwnd = annotationView_->trackedWindow();
    if (hwnd && QtWin::getWindowProcessId(hwnd) == pi.processId) {
      QString title = QtWin::getWindowText(hwnd).trimmed();
      if (!title.isEmpty()) {
        if (recentUrlTitles_.size() > RECENT_COUNT)
          recentUrlTitles_.erase(recentUrlTitles_.begin());
        recentUrlTitles_[mrl] = title;
      }
    }
  }
  recentDigest_.clear();
  recentSource_.clear();
  invalidateToken(mrl);

  hub_->play();
  DOUT("exit");
}

#endif // WITH_WIN_TEXTHOOK

#ifdef USE_MODE_SIGNAL

void
MainWindow::showSignalView()
{ setSignalViewVisible(true); }

void
MainWindow::hideSignalView()
{ setSignalViewVisible(false); }

void
MainWindow::setSignalViewVisible(bool visible)
{
  //if (visible) {
  //  if (hub_->isMediaTokenMode() && hub_->isNormalPlayerMode() &&
  //      !player_->isPlaying() && !player_->isPaused())
  //    hide();
  //} else {
  //  if (hub_->isMediaTokenMode() && !isVisible())
  //    show();
  //}
  //signalView_->tokenView()->setToken(tokenView_->token());
  //signalView_->tokenView()->setAliases(tokenView_->aliases());
  //signalView_->tokenView()->setUserId(tokenView_->userId());
  signalView_->setVisible(visible);
  if (visible) {
    // FIXME
    osdWindow_->setWindowOnTop();
    signalView_->raise();
  }
}

void
MainWindow::showRecentMessageView()
{ setRecentMessageViewVisible(true); }

void
MainWindow::hideRecentMessageView()
{ setRecentMessageViewVisible(false); }

void
MainWindow::setRecentMessageViewVisible(bool visible)
{
  if (visible) {
    recentMessageView_->clear();
    recentMessageView_->addMessages(QtExt::revertList(messageHandler_->recentMessages()), messageHandler_->hookId());
    recentMessageView_->setCurrentIndex(1);
  }
  recentMessageView_->setVisible(visible);
  if (visible)
    recentMessageView_->raise();
}

#endif // USE_MODE_SIGNAL

void
MainWindow::setWindowTrackingEnabled(bool t)
{
#ifdef WITH_WIN_PICKER
  if (lastTrackedWindow_ && (
      lastTrackedWindow_ == annotationView_->trackedWindow() ||
      !QtWin::windowHasText(lastTrackedWindow_)))
    lastTrackedWindow_ = 0;

  if (!annotationView_->trackedWindow() && !lastTrackedWindow_) {
    showWindowPickDialog();
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
  Q_UNUSED(t);
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
      showMessage(tr("connecting server to change anonymous status ..."));
      QThreadPool::globalInstance()->start(new task_::setUserAnonymous(t, this));
      DOUT("exit: returned from async branch");
      return;
    }

    DOUT("inetMutex locking");
    inetMutex_.lock();
    DOUT("inetMutex locked");

    bool ok = server_->setUserAnonymous(t);
    if (!ok)
      warn(tr("failed to change user anonymous state"));

    DOUT("inetMutex unlocking");
    inetMutex_.unlock();
    DOUT("inetMutex unlocked");

  } else
    server_->user().setAnonymous(t);

  if (server_->user().isAnonymous())
    showMessage(tr("you are anonymous now"));
  else
    showMessage(tr("you are not anonymous now"));

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
      showMessage(tr("connecting server to change language ..."));
      QThreadPool::globalInstance()->start(new task_::setUserLanguage(lang, this));
      DOUT("exit: returned from async branch");
      return;
    }

    DOUT("inetMutex locking");
    inetMutex_.lock();
    DOUT("inetMutex locked");

    bool ok = server_->setUserLanguage(lang);
    if (!ok)
      warn(tr("failed to change user language"));

    DOUT("inetMutex unlocking");
    inetMutex_.unlock();
    DOUT("inetMutex unlocked");

  } else
    server_->user().setLanguage(lang);

  showMessage(tr("your language is ") + languageToString(server_->user().language()));

  DOUT("exit");
}

void MainWindow::setUserLanguageToEnglish()     { setUserLanguage(Traits::English); }
void MainWindow::setUserLanguageToJapanese()    { setUserLanguage(Traits::Japanese); }
void MainWindow::setUserLanguageToChinese()     { setUserLanguage(Traits::Chinese); }
void MainWindow::setUserLanguageToKorean()      { setUserLanguage(Traits::Korean); }
void MainWindow::setUserLanguageToUnknown()     { setUserLanguage(Traits::UnknownLanguage); }

void
MainWindow::invalidateAnnotationLanguages()
{
  qint64 languages = 0;
  if (toggleAnnotationLanguageToAnyAct_->isChecked()) {
    languages = Traits::AnyLanguageBit;

    toggleAnnotationLanguageToEnglishAct_->setEnabled(false);
    toggleAnnotationLanguageToJapaneseAct_->setEnabled(false);
    toggleAnnotationLanguageToChineseAct_->setEnabled(false);
    toggleAnnotationLanguageToKoreanAct_->setEnabled(false);
    toggleAnnotationLanguageToUnknownAct_->setEnabled(false);

  } else {
    toggleAnnotationLanguageToEnglishAct_->setEnabled(true);
    toggleAnnotationLanguageToJapaneseAct_->setEnabled(true);
    toggleAnnotationLanguageToChineseAct_->setEnabled(true);
    toggleAnnotationLanguageToKoreanAct_->setEnabled(true);
    toggleAnnotationLanguageToUnknownAct_->setEnabled(true);

    if (toggleAnnotationLanguageToUnknownAct_->isChecked())       languages |= Traits::UnknownLanguageBit;
    if (toggleAnnotationLanguageToEnglishAct_->isChecked())       languages |= Traits::EnglishBit;
    if (toggleAnnotationLanguageToJapaneseAct_->isChecked())      languages |= Traits::JapaneseBit;
    if (toggleAnnotationLanguageToChineseAct_->isChecked())       languages |= Traits::ChineseBit;
    if (toggleAnnotationLanguageToKoreanAct_->isChecked())        languages |= Traits::KoreanBit;
  }

  // Use at least unknown language
  if (!languages) {
    toggleAnnotationLanguageToUnknownAct_->setChecked(true);
    languages = Traits::UnknownLanguageBit;
  }

  annotationFilter_->setLanguages(languages);
}

// - Language -

void
MainWindow::setAppLanguageToEnglish()
{ AcSettings::globalSettings()->setLanguage(QLocale::English); invalidateAppLanguage(); }

void
MainWindow::setAppLanguageToJapanese()
{ AcSettings::globalSettings()->setLanguage(QLocale::Japanese); invalidateAppLanguage(); }

// TODO: Simplified Chinese and Traditional Chinese are not differed until qt 4.8
void
MainWindow::setAppLanguageToTraditionalChinese()
{ AcSettings::globalSettings()->setLanguage(TranslatorManager::TraditionalChinese); invalidateAppLanguage(); }

void
MainWindow::setAppLanguageToSimplifiedChinese()
{ AcSettings::globalSettings()->setLanguage(QLocale::Chinese); invalidateAppLanguage(); }

void
MainWindow::invalidateAppLanguage()
{
  Settings::globalSettings()->setApplicationFilePath(QString());
  Settings::globalSettings()->sync();
  AcSettings::globalSettings()->sync();
  notify(tr("restart the app to use the new language"));
}

// - Helpers -

QString
MainWindow::languageToString(int lang)
{
  switch(lang) {
  case Traits::AnyLanguage:     return TR(T_ANYLANGUAGE);
  case Traits::English:         return TR(T_ENGLISH);
  case Traits::Japanese:        return TR(T_JAPANESE);
  case Traits::Chinese:         return TR(T_CHINESE);
  case Traits::Korean:          return TR(T_KOREAN);

  case Traits::UnknownLanguage:
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
  bool update = false;

  BOOST_AUTO(p, playlist_.begin());
  while (p != playlist_.end()) {
    QFileInfo fi(removeUrlPrefix(*p));
    if (fi.exists())
      ++p;
    else {
      p = playlist_.erase(p);
      update = true;
    }
  }
  if (update)
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
      BOOST_AUTO(a, new QtExt::ActionWithId(index++, text, playlistMenu_));
      if (f == recentOpenedFile_) {
#ifdef Q_WS_X11
        a->setCheckable(true);
        a->setChecked(true);
#else
        a->setIcon(QIcon(RC_IMAGE_CURRENTFILE));
#endif // Q_WS_X11
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
  bool update = false;

  BOOST_AUTO(p, recentFiles_.begin());
  while (p != recentFiles_.end()) {
    if (isRemoteMrl(*p) ||
        QFileInfo(removeUrlPrefix(*p)).exists())
      ++p;
    else {
      p = recentFiles_.erase(p);
      update = true;
    }
  }

  if (!recentFiles_.isEmpty()) {
     QStringList unique = QtExt::uniqueList(recentFiles_);
     if (unique.size() != recentFiles_.size()) {
       recentFiles_ = unique;
       update = true;
     }
  }

  if (update)
    updateRecentMenu();
}

void
MainWindow::updateRecentMenu()
{
  QList<QAction*> l = recentMenu_->actions();
  foreach (QAction *a, l)
    if (a && a != clearRecentAct_)
      QTimer::singleShot(0, a, SLOT(deleteLater()));
  recentMenu_->clear();

  if (!recentFiles_.isEmpty()) {
    int i = 0;
    foreach (const QString &f, recentFiles_) {
      if (i >= RECENT_COUNT)
        break;

      QString text;
      BOOST_AUTO(p, recentUrlTitles_.find(f));
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
#ifdef Q_WS_WIN
          else if (text.size() > 3) {
            if (char d = QtWin::getFileDriveLetter(path))
              text.prepend(": ")
                  .prepend(QChar(d).toUpper());
          }
#endif // Q_WS_WIN
        }
#ifdef Q_WS_WIN
        else if (char d = QtWin::getFileDriveLetter(path))
          text.prepend(": ")
              .prepend(QChar(d).toUpper());
#endif // Q_WS_WIN
        if (p != recentUrlTitles_.end()) {
          QString title = p.value();
          if (title != fi.completeBaseName())
            text.append(": " + p.value());
        }
      }
      Q_ASSERT(!text.isEmpty());
      text = QString::number(i+1) + ". " + shortenText(text);
      BOOST_AUTO(a, new QtExt::ActionWithId(i++, text, recentMenu_));
      connect(a, SIGNAL(triggeredWithId(int)), SLOT(openRecent(int)));

      recentMenu_->addAction(a);
    }

    recentMenu_->addSeparator();
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
    warn(TR(T_ERROR_BAD_FILEPATH) + ": " + path);
    return;
  }

#ifdef Q_WS_WIN
  if (fi.suffix().compare("lnk", Qt::CaseInsensitive) == 0) {
    QString lnk = QtWin::resolveLink(path, winId());
    if (!lnk.isEmpty()) {
      addRecent(path);
      openSource(lnk);
    } else
      warn(tr("invalid lnk") + ": " + path );
  } else
#endif // Q_WS_WIN
#ifdef USE_MODE_SIGNAL
  if (fi.suffix().compare("exe", Qt::CaseInsensitive) == 0)
    openProcessPath(path);
  else
#endif // USE_MODE_SIGNAL
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
  } else
    openMrl(path); // check existence = true
}

void
MainWindow::addRecent(const QString &path)
{
  //if (recentFiles_.indexOf(path) >= 0)
  if (!recentFiles_.isEmpty())
    recentFiles_.removeAll(path);
  recentFiles_.prepend(path);
#ifdef Q_WS_WIN
  if (!isRemoteMrl(path) &&
      WindowsRegistry::globalInstance()->containsType(QFileInfo(path).suffix())) {
    QString fileName = path;
    fileName.remove(QRegExp("^file://"));
    fileName.remove(QRegExp("^[/]+"));
    QtWin::addRecentDocument(fileName);
  }
#endif // Q_WS_WIN

  // SingleShot to avoid multi-threading issue.
  QTimer::singleShot(0, this, SLOT(updateRecentMenu()));
}

// - Translate -

bool
MainWindow::isTranslateEnabled() const
{ return hub_->isSignalTokenMode() && toggleTranslateAct_->isChecked(); }

void
MainWindow::setTranslateEnabled(bool enabled)
{ toggleTranslateAct_->setChecked(enabled); }

void
MainWindow::translate(const QString &text)
{
  if (!isTranslateEnabled() || text.isEmpty() || !server_->isConnected())
    return;

  int l = server_->user().language();
  translate(text, l);
}

void
MainWindow::translate(const QString &text, int lang)
{
  if (!server_->isConnected())
    return;
  QString lcode;
  switch (lang) {
  case Traits::Japanese: lcode = Translator::Japanese; break;
  case Traits::Chinese:  lcode = Translator::SimplifiedChinese; break;
  case Traits::Korean:   lcode = Translator::Korean; break;
  case Traits::English:
  default:               lcode = Translator::English; break;
  }
  translator_->translate(text, lcode);
}

// - Subtitle -

void
MainWindow::setSubtitleOnTop(bool t)
{
  annotationView_->setSubtitlePosition(
        t ? AnnotationGraphicsView::AP_Top
          : AnnotationGraphicsView::AP_Bottom);

  Settings::globalSettings()->setSubtitleOnTop(t);
}

bool
MainWindow::isSubtitleOnTop() const
{ return annotationView_->subtitlePosition() == AnnotationGraphicsView::AP_Top; }

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
#ifdef Q_WS_WIN
  path.replace('\\', '/');
  path.prepend("file:///");
#else
  path.prepend("file://");
#endif // Q_WS_WIN
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
    QString fileName = recentSource_.isEmpty() ? player_->mediaPath()
                                               : recentSource_;
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
#ifdef Q_WS_WIN
    if (!t.contains(':')) {
      if (char c = QtWin::getFileDriveLetter(path))
        t.prepend(':')
         .prepend(QChar(c).toUpper());
    }
#endif // Q_WS_WIN
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
    BOOST_AUTO(a, new QtExt::ActionWithId(id++, text, browseMenu_));
    if (f.fileName() == fi.fileName()) {
#ifdef Q_WS_X11
      a->setCheckable(true);
      a->setChecked(true);
#else
      a->setIcon(QIcon(RC_IMAGE_CURRENTFILE));
#endif // Q_WS_X11
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

  showMessage(tr("less files to browse"));
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
    showMessage(tr("more files added"));
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
  QString mrl = dataManager_->token().source();
  if (!mrl.isEmpty()) {
    QString n = QtExt::decreaseString(mrl);
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
  QString mrl = dataManager_->token().source();
  if (!mrl.isEmpty()) {
    if (mrl.endsWith("/")) {
      mrl.append("index_2.html");
      openSource(mrl);
    } else {
      QString n = QtExt::increaseString(mrl);
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
    player_->embed(0);
    player_->setEmbeddedWindow(winId);
#endif // !Q_OS_MAC
  }
}

// - Live mode -

void
MainWindow::openChannel()
{
  Q_ASSERT(liveInterval_ > 0);
  annotationView_->setInterval(liveInterval_);
  liveTimer_->setInterval(liveInterval_);

  annotationView_->start();
  liveTimer_->start();
}

void
MainWindow::closeChannel()
{
  liveTimer_->stop();
  annotationView_->stop();
}

void
MainWindow::updateLiveAnnotations(bool async)
{
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (!server_->isConnected() || !server_->isAuthorized())
    return;
  //DOUT("enter: async =" << async);

  if (async) {
    //log(tr("connecting server to update annot ..."));
    QThreadPool::globalInstance()->start(new task_::updateLiveAnnotations(this));
    //DOUT("exit: returned from async branch");
    return;
  }

  //DOUT("inetMutex locking");
  inetMutex_.lock();
  //DOUT("inetMutex locked");

  AnnotationList l = server_->selectLiveAnnotations();
  //DOUT("annots.count =" << l.size());

  if (!l.isEmpty())
    //annotationView_->appendAnnotations(l);
    emit appendAnnotationsRequested(l);

  //DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  //DOUT("inetMutex unlocked");

  //DOUT("exit");
}

// - Remote annotation -

void
MainWindow::addRemoteAnnotations(const AnnotationList &l, const QString &url)
{
  DOUT("enter: annot.count =" << l.size() << ", url =" << url);
  //bool remote = isRemoteMrl(url);
  //if (remote && !registerAnnotationUrl(url)) {
  //  DOUT("exit: already imported");
  //  return;
  //}
  const Token &t = dataManager_->token();
  AnnotationList annots;
  foreach (Annotation a, l) {
    a.setTokenId(t.id());
    a.setTokenPart(t.part());
    a.setTokenDigest(t.digest());
    annots.append(a);
  }
  if (!annots.isEmpty()) {
    QString src = isRemoteMrl(url) ? url : "http://nicovideo.jp";
    QString msg = QString("%1 :" HTML_STYLE_OPEN(color:red) "+%2" HTML_STYLE_CLOSE() ", %3")
      .arg(tr("annotations found"))
      .arg(QString::number(annots.size()))
      .arg(src);
    emit message(msg);
    emit addAnnotationsRequested(annots);
    DOUT("token source =" << t.source());
    if (t.hasId() && cache_->isValid() &&
        !t.hasSource())
      cache_->insertAnnotations(annots);
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
MainWindow::importAnnotationsFromUrl(const QString &suburl)
{
  DOUT("enter: import annot from URL:" << suburl);
  if (!suburl.isEmpty()) {
    QString url = isRemoteMrl(suburl) ? suburl : "http://flapi.nicovideo.jp";
    showMessage(tr("analyzing annotation URL ...") + ": " + url);
    if (registerAnnotationUrl(suburl))
      AnnotationCodecManager::globalInstance()->fetch(suburl);
  }
  DOUT("exit");
}

bool
MainWindow::isAnnotationUrlRegistered(const QString &suburl) const
{
  QMutexLocker lock(&annotMutex_);
  QString k = suburl;
  if (k.endsWith('/'))
    k.chop(1);
  else if (k.endsWith("/index.html", Qt::CaseInsensitive))
    k.remove(QRegExp("/index.html$"));
  else if (k.endsWith("index_1.html", Qt::CaseInsensitive))
    k.remove(QRegExp("/index_1.html$"));
  return annotationUrls_.contains(k, Qt::CaseInsensitive);
}

bool
MainWindow::registerAnnotationUrl(const QString &suburl)
{
  QMutexLocker lock(&annotMutex_);
  if (suburl.isEmpty() ||
      annotationUrls_.contains(suburl, Qt::CaseInsensitive))
    return false;
  QString k = suburl;
  if (k.endsWith('/'))
    k.chop(1);
  else if (k.endsWith("/index.html", Qt::CaseInsensitive))
    k.remove(QRegExp("/index.html$"));
  else if (k.endsWith("index_1.html", Qt::CaseInsensitive))
    k.remove(QRegExp("/index_1.html$"));
  if (annotationUrls_.contains(k, Qt::CaseInsensitive))
    return false;
  annotationUrls_.append(k);
  return true;
}

void
MainWindow::clearAnnotationUrls()
{ annotationUrls_.clear(); }

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
    ret = QFileInfo(ret).baseName();
  return ret;
}

QString
MainWindow::currentUrl() const
{
  QString ret;
  if (dataManager_->token().hasSource())
    ret = dataManager_->token().source();
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
    showMessage(tr("copied") + ": " +
      HTML_STYLE_OPEN(color:orange) + url + HTML_STYLE_CLOSE()
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
    showMessage(tr("copied") + ": " +
      HTML_STYLE_OPEN(color:orange) + url + HTML_STYLE_CLOSE()
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
      showMessage(
        tr("resuming last play") + ": "
        HTML_STYLE_OPEN(color:orange)
        + QtExt::msecs2time(pos).toString("hh:mm:ss") +
        HTML_STYLE_CLOSE()
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
          showMessage(tr("loading last subtitle") + ": " + desc[track]);
        else
          showMessage(tr("loading last subtitle"));
        player_->setSubtitleId(track);
      }
      player_->setSubtitleVisible(true);
    } else if (track == -1) {
      showMessage(tr("hide last subtitle"));
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

    int track =  hash && audioTrackHistory_.contains(hash) ? audioTrackHistory_[hash] :
                                                             preferredAudioTrack_;
    DOUT("track =" << track);
    if (track >= 0 && track < player_->audioTrackCount()
        && track != player_->audioTrackId()) {
      if (track > 0) {
        QStringList desc =  player_->audioTrackDescriptions();
        if (track < desc.size())
          showMessage(tr("loading last audio track") + ": " + desc[track]);
        else
          showMessage(tr("loading last audio track"));
      }
      player_->setAudioTrackId(track);
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
      showMessage(tr("using last aspect ratio") + ": " + ratio);
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

  QString mrl = dataManager_->token().source();
  return mrl.contains(QRegExp("index_\\d+.html$", Qt::CaseInsensitive));
}

void
MainWindow::setPreviousEnabled(bool t)
{
  PlayerPanel *players[] = { mainPlayer_, miniPlayer_, embeddedPlayer_ };
  BOOST_FOREACH(PlayerPanel *p, players) {
    p->previousButton()->setEnabled(t);
#ifdef Q_WS_WIN
    QtWin::repaintWindow(mainPlayer_->previousButton()->winId());
#endif // Q_WS_WIN
  }

  previousAct_->setEnabled(t);
}

void
MainWindow::setNextEnabled(bool t)
{
  PlayerPanel *players[] = { mainPlayer_, miniPlayer_, embeddedPlayer_ };
  BOOST_FOREACH(PlayerPanel *p, players) {
    p->nextButton()->setEnabled(t);
#ifdef Q_WS_WIN
    QtWin::repaintWindow(mainPlayer_->nextButton()->winId());
#endif // Q_WS_WIN
  }

  nextAct_->setEnabled(t);
}
void
MainWindow::setNavigationEnabled(bool t)
{
  PlayerPanel *players[] = { mainPlayer_, miniPlayer_, embeddedPlayer_ };
  BOOST_FOREACH(PlayerPanel *p, players) {
    p->openButton()->setEnabled(t);
#ifdef Q_WS_WIN
    QtWin::repaintWindow(mainPlayer_->openButton()->winId());
#endif // Q_WS_WIN
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
    showMessage(tr("annot URL is already used") + ": "
        HTML_STYLE_OPEN(color:orange) + url + HTML_STYLE_CLOSE()
    );
    return;
  }
  if (url == dataManager_->token().source()) {
    showMessage(tr("media URL is being played ") + ": "
        HTML_STYLE_OPEN(color:orange) + url + HTML_STYLE_CLOSE()
    );
    return;
  }
  foreach (const Alias &a, dataManager_->aliases())
    if (a.type() == Alias::AT_Url &&
        a.text() == url) {
      showMessage(tr("annotation URL already imported") + ": "
        HTML_STYLE_OPEN(color:orange) + url + HTML_STYLE_CLOSE()
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
  if (url == dataManager_->token().source()) {
    showMessage(tr("media URL is being played ") + ": "
        HTML_STYLE_OPEN(color:orange) + url + HTML_STYLE_CLOSE()
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

  showMessage(tr("do nothings after finished playing all files"));
}

void
MainWindow::sleepAfterFinished()
{
  nothingAfterFinishedAct_->setChecked(false);
  sleepAfterFinishedAct_->setChecked(true);
  shutdownAfterFinishedAct_->setChecked(false);
  showMessage(tr("put the computer to sleep after finished playing all files"));
}

void
MainWindow::shutdownAfterFinished()
{
  nothingAfterFinishedAct_->setChecked(false);
  sleepAfterFinishedAct_->setChecked(false);
  shutdownAfterFinishedAct_->setChecked(true);
  showMessage(tr("shutdown the computer after finished playing all files"));
}

void
MainWindow::promptSleep()
{
  if (!sleepAfterFinishedAct_->isChecked())
    return;
  static SleepDialog *d = 0;
  if (!d) {
    d = new SleepDialog(this);
    windows_.append(d);
  }
  DOUT("show sleep dialog");
  d->show();
}

void
MainWindow::promptShutdown()
{
  if (!shutdownAfterFinishedAct_->isChecked())
    return;
  static ShutdownDialog *d = 0;
  if (!d) {
    d = new ShutdownDialog(this);
    windows_.append(d);
  }
  DOUT("show shutdown dialog");
  d->show();
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
#ifdef Q_WS_MAC
  ok = QtMac::open(AC_BROWSER);
#elif defined Q_WS_WIN
  QString exe = QCoreApplication::applicationDirPath() + "/" AC_BROWSER;
  ok = QProcess::startDetached('"' + exe + '"');
#else
  ok = QProcess::startDetached(AC_BROWSER);
#endif // Q_WS_
  if (!ok) {
    warn(tr("failed to launch external browser"));
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
    showMessage(tr("allow multiple player windows"));
  else
    showMessage(tr("allow single player window"));
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
    notify(tr("please enable Multiple Window from context menu"));
    return;
  }

  AcSettings::globalSettings()->sync();

  QProcess::startDetached(QCoreApplication::applicationFilePath());

  showMessage(tr("new window launched"));
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
    userAnalyticsView_ = new UserAnalyticsView(dataManager_, this);
    windows_.append(userAnalyticsView_);
  }
  userAnalyticsView_->setUserId(userId);
  userAnalyticsView_->show();
}

AnnotationAnalyticsView*
MainWindow::annotationAnalyticsView()
{
  if (!annotationAnalyticsView_) {
    annotationAnalyticsView_ = new AnnotationAnalyticsView(dataManager_, this);
    windows_.append(annotationAnalyticsView_);
  }
  return annotationAnalyticsView_;
}

void
MainWindow::setAnnotationAnalyticsViewVisible(bool visible)
{
  if (visible)
    annotationAnalyticsView()->refresh();
  else
    annotationAnalyticsView()->hide();
}

void
MainWindow::toggleAnnotationAnalyticsViewVisible()
{
  QWidget *w = annotationAnalyticsView();
  w->setVisible(!w->isVisible());
}

// - Save media -

void
MainWindow::setSaveMedia(bool t)
{
  player_->setBufferSaved(t);
  if (t)
    showMessage(tr("buffered video will be saved on Desktop"));
  else
    showMessage(tr("buffered video will not be saved"));
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
  showMessage(tr("resume annotations"));
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
  qreal value = AnnotationSettings::globalInstance()->scale();
  value *= 1.1;
  if (value < 5)
    AnnotationSettings::globalInstance()->setScale(value);
}

void
MainWindow::annotationScaleDown()
{
  qreal value = AnnotationSettings::globalInstance()->scale();
  value /= 1.1;
  if (value > 0.2)
    AnnotationSettings::globalInstance()->setScale(value);
}

void
MainWindow::annotationRotateUp()
{
  enum { delta = 15 };
  AnnotationSettings::globalInstance()->setRotation(
    AnnotationSettings::globalInstance()->rotation() + delta
  );
}

void
MainWindow::annotationRotateDown()
{
  enum { delta = 15 };
  AnnotationSettings::globalInstance()->setRotation(
    AnnotationSettings::globalInstance()->rotation() - delta
  );
}

void
MainWindow::increaseAnnotationOffset()
{
  enum { delta = 5 };
  AnnotationSettings::globalInstance()->setOffset(
    AnnotationSettings::globalInstance()->offset() + delta
  );
}

void
MainWindow::decreaseAnnotationOffset()
{
  enum { delta = 5 };
  AnnotationSettings::globalInstance()->setOffset(
    AnnotationSettings::globalInstance()->offset() - delta
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
//#ifdef Q_WS_MAC
//  if (hub_->isFullScreenWindowMode() &&
//      player_->hasMedia() && player_->isPlaying())
//    autoHideCursorTimer_->start();
//  showCursor();
//#endif // Q_WS_MAC
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
  Q_UNUSED(g);
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
  Q_UNUSED(g);
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
        showMessage(tr("openning") + ": " HTML_STYLE_OPEN(color:orange) + url + HTML_STYLE_CLOSE());
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
  bool v = embeddedPlayer_->isVisible() && !embeddedCanvas_->isVisible() ||
           hub_->isMediaTokenMode() && player_->isPaused() ||
           hub_->isSignalTokenMode() && hub_->isPaused();
  annotationView_->setItemMetaVisible(v);
}

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
