// mainwindow.cc
// 6/30/2011

#include "mainwindow.h"
#include "mainwindowprivate.h"

#include "db.h"
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
#include "signalhub.h"
#include "mainplayer.h"
#include "miniplayer.h"
#include "annotationgraphicsview.h"
#include "annotationeditor.h"
#include "annotationfilter.h"
#include "tokenview.h"
#include "commentview.h"
#include "tray.h"
#include "cloudview.h"
#include "annotationbrowser.h"
#include "stylesheet.h"
#include "uistyle.h"
#include "rc.h"
#include "tr.h"
#include "translatormanager.h"
#include "aboutdialog.h"
#include "blacklistview.h"
#include "devicedialog.h"
#include "helpdialog.h"
#include "logindialog.h"
#include "pickdialog.h"
#include "seekdialog.h"
#include "livedialog.h"
#include "syncdialog.h"
#include "settings.h"
#include "defines.h"
#ifdef USE_MODE_SIGNAL
  #include "signalview.h"
  #include "messageview.h"
  #include "processview.h"
  #include "messagehandler.h"
#endif // USE_MODE_SIGNAL
#include "module/player/player.h"
#include "module/translator/translator.h"
#ifdef USE_MODULE_SERVERAGENT
  #include "module/serveragent/serveragent.h"
#endif // USE_MODULE_SERVERAGENT
#ifdef USE_MODULE_CLIENTAGENT
  #include "module/clientagent/clientagent.h"
#endif // USE_MODULE_CLIENTAGENT
#ifdef USE_MODULE_DOLL
  #include "module/doll/doll.h"
#endif // USE_MODULE_DOLL
#ifdef USE_WIN_HOOK
  #include "win/hook/hook.h"
#endif // USE_WIN_HOOK
#ifdef USE_WIN_MOUSEHOOK
  #include "win/mousehook/mousehook.h"
#endif // USE_WIN_MOUSEHOOK
//#ifdef USE_WIN_DWM
//  #include "win/dwm/dwm.h"
//#endif // USE_WIN_DWM
#ifdef USE_WIN_QTH
  #include "win/qth/qth.h"
#endif // USE_WIN_QTH
#ifdef Q_WS_WIN
  #include "win/qtwin/qtwin.h"
#endif // Q_WS_WIN
#ifdef USE_WIN_PICKER
  #include "win/picker/picker.h"
#endif // USE_WIN_PICKER
#ifdef Q_WS_MAC
  #include "mac/qtmac/qtmac.h"
  //#include "mac/qtstep/qtstep.h"
  //#include "mac/qtvlc/qtvlc.h"
#endif // Q_WS_MAC
#ifdef Q_WS_X11
  #include "unix/qtx/qtx.h"
#endif // Q_WS_X11
#ifdef Q_OS_UNIX
  #include "unix/qtunix/qtunix.h"
#endif // Q_OS_UNIX
#include "core/os.h"
#include "core/cmd.h"
#include "core/annotationparser.h"
#include "core/util/datetime.h"
#include "core/htmltag.h"
#include "core/gui/actionwithid.h"
#include <QtGui>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/typeof/typeof.hpp>
#include <cstring>

#ifndef USE_MODULE_SERVERAGENT
  #error "Module server agent is indispensible."
#endif // USE_MODULE_SERVERAGENT

using namespace Core::Cloud;
using namespace Logger;

#define DEBUG "mainwindow"
#include "module/debug/debug.h"

// - Focus -

void
MainWindow::onFocusedWidgetChanged(QWidget *w_old, QWidget *w_new)
{
#ifdef Q_WS_WIN
  // When EmbeddedPlayer's lineEdit cleared its focus.
  if (!w_new && w_old == embeddedPlayer_->inputComboBox())
    QtWin::sendMouseClick(QPoint(0, 0), Qt::LeftButton);
#else
  Q_UNUSED(w_old);
  Q_UNUSED(w_new);
#endif // Q_WS_WIN
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

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f),
    tray_(0),
    dragPos_(BAD_POS), tokenType_(0),
    themeMenu_(0),
    setThemeToDefaultAct_(0), setThemeToRandomAct_(0),
    setThemeToBlack1Act_(0), setThemeToBlack2Act_(0),
    setThemeToBlue1Act_(0), setThemeToBlue2Act_(0),
    setThemeToBrown1Act_(0), setThemeToBrown2Act_(0),
    setThemeToGreen1Act_(0), setThemeToGreen2Act_(0),
    setThemeToLightBlue1Act_(0), setThemeToLightBlue2Act_(0),
    setThemeToOrange1Act_(0), setThemeToOrange2Act_(0),
    setThemeToPink1Act_(0), setThemeToPink2Act_(0),
    setThemeToPurple1Act_(0), setThemeToPurple2Act_(0),
    setThemeToRed1Act_(0), setThemeToRed2Act_(0),
    setThemeToYellow1Act_(0), setThemeToYellow2Act_(0)
{
#ifdef USE_WIN_PICKER
  setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
#endif // USE_WIN_PICKER

  setWindowIcon(QIcon(RC_IMAGE_APP)); // X11 require setting icon at runtime
  setWindowTitle(TR(T_TITLE_PROGRAM));
  setContentsMargins(0, 0, 0, 0);
  UiStyle::globalInstance()->setMainWindowStyle(this);

  menuBar()->setVisible(Settings::globalInstance()->isMenuBarVisible());

  createComponents();

  createConnections();

  createActions();

  createMenus();

  createDockWindows();

  // - Post behaviors

  // Accept drag/drop behavior (default enabled on Windows but disabled on Mac)
  setAcceptDrops(true);

  // Receive mouseMoveEvent even no mouse button is clicked
  // Set this to enable resetAutoHide embeddedPlayer when mouse is moved.
  setMouseTracking(true);

  // Adjust EmbeddedPlayer to fit in OsdWindow.
  embeddedPlayer_->invalidateGeometry();

  // Global hooks
#ifdef USE_WIN_HOOK
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
#endif // USE_WIN_HOOK

#ifdef USE_WIN_MOUSEHOOK
  MouseHook::globalInstance()->setEventListener(this);
  MouseHook::globalInstance()->start();
#endif // USE_WIN_MOUSEHOOK

  // Load settings
  Settings *settings = Settings::globalInstance();

  setSubtitleColor(settings->subtitleColor());

  recentFiles_ = settings->recentFiles();
  invalidateRecentMenu();

  recentPath_ = settings->recentPath();

  setTranslateEnabled(settings->isTranslateEnabled());
  setSubtitleStaysOnTop(settings->isSubtitleStaysOnTop());

  embeddedPlayer_->setOnTop(settings->isEmbeddedPlayerStaysOnTop());

  annotationFilter_->setEnabled(settings->isAnnotationFilterEnabled());

  invalidateAnnotationLanguages();

  // Initial focus
  mainPlayer_->inputComboBox()->setFocus();

  // System tray icon
  if (tray_)
    tray_->show(); // FIXME: not working on mac?

  // Parse arguments
  //parseArguments(args);

  //statusBar()->showMessage(tr("Ready"));
}

void
MainWindow::createComponents()
{
  // Systemt tray icon
  if (QSystemTrayIcon::isSystemTrayAvailable())
    tray_ = new Tray(this);

  // Utilities
  grabber_ = new Grabber(this);
  grabber_->setSavePath(Core::Os::desktopPath());

  // Caches
  cache_ = new Database(G_PATH_CACHEDB, this);
  if (!cache_->isValid())
    cache_->clear();
  queue_ = new Database(G_PATH_QUEUEDB, this);
  if (!queue_->isValid())
    queue_->clear();

  // Server agents
  server_ = new ServerAgent(this);
#ifdef USE_MODULE_CLIENTAGENT
  client_ = new ClientAgent(this);

  server_->setClientAgent(client_);
  client_->setServerAgent(server_);
#endif // USE_MODULE_CLIENTAGENT

  // Data server
  dataServer_ = new DataServer(server_, cache_, queue_, this);

  // Data manager
  dataManager_ = new DataManager(this);

  // Qth
#ifdef USE_WIN_QTH
  //Qth *qth = new Qth(winId(), this);
  //Qth::setGlobalInstance(qth);
  //QTH->setParentWinId(winId());
#endif // USE_WIN_QTH

  // Player
  player_ = new Player(this); {
    //player_->setKeyboardEnabled(true);  // default true
    //player_->setMouseEnabled(true);     // default true
    //player_->setEncoding("UTF-8");      // default do nothing
#ifdef Q_WS_WIN
    player_->setMouseEventEnabled();
#endif // Q_WS_WIN
  }
  Q_ASSERT(isValid());

  // Signal hub
  hub_ = new SignalHub(player_, this);

  // Logger
  logger_ = new EventLogger(player_, this);

  // Players and graphic views
  videoView_ = new VideoView(this);
  osdWindow_ = new OsdWindow(this);

  annotationView_ = new AnnotationGraphicsView(hub_, server_, player_, videoView_, osdWindow_);
  annotationView_->setFullScreenView(osdWindow_);
#ifdef USE_WIN_DWM
  {
    QWidget *w = annotationView_->editor();
    w->setParent(this, w->windowFlags()); // hot fix for dwm displaying issue
    UiStyle::globalInstance()->setWindowDwmEnabled(w->winId(), false);
    UiStyle::globalInstance()->setWindowDwmEnabled(w->winId(), true);
  }
#endif // USE_WIN_DWM
//#ifdef Q_WS_MAC // remove background in annotation view
//  annotationView_->setStyleSheet(
//    SS_BEGIN(QWidget)
//      SS_TRANSPARENT
//    SS_END
//  );
//#endif // Q_WS_MAC
  annotationFilter_ = new AnnotationFilter(this);
  annotationView_->setFilter(annotationFilter_);

  //QLayout *layout = new QStackedLayout(osdWindow_);
  //layout->addWidget(annotationView_);
  //osdWindow_->setLayout(layout);
  osdWindow_->setEventListener(annotationView_);

  //embeddedPlayer_ = new EmbeddedPlayerUi(hub_, player_, server_, osdWindow_);
  embeddedPlayer_ = new EmbeddedPlayerUi(hub_, player_, server_);
//#ifdef Q_WS_MAC // remove background in Osd player
//  embeddedPlayer_->setStyleSheet(
//    SS_BEGIN(QWidget)
//      SS_TRANSPARENT
//    SS_END
//  );
//#endif // Q_WS_MAC

  globalOsdConsole_ = new OsdConsole(annotationView_);
  globalOsdConsole_->setAutoClearInterval(G_CONSOLE_AUTOClEAR_TIMEOUT);
  OsdConsole::setGlobalInstance(globalOsdConsole_);

  // Show OSD window
  osdWindow_->showInOsdMode(); // this must go after all children of osdWindow_ are created
  embeddedPlayer_->setParent(osdWindow_); // Set parent after osdWindow_ become visible.
  embeddedPlayer_->setAutoHideEnabled();
  embeddedPlayer_->setContainerWidget(this);

  annotationView_->invalidateGeometry();

  mainPlayer_ = new MainPlayerUi(hub_, player_, server_, this);
  miniPlayer_ = new MiniPlayerUi(hub_, player_, server_, this);

#ifdef Q_WS_MAC
  videoView_->showView();
  player_->setEmbeddedWindow(videoView_->view());
  QTimer::singleShot(0, videoView_, SLOT(hideView()));
#else
  player_->embed(videoView_);
#endif // Q_WS_MAC
  player_->setFullScreen(false);

  // Translator
  translator_ = new Translator(this);

  // Dialogs

  //userPanel_ = new UserPanel(this);
  //userPanel_->hide(); // TODO!!!!!!

  blacklistView_ = new BlacklistView(annotationFilter_, this);

  deviceDialog_ = new DeviceDialog(this);

  loginDialog_ = new LoginDialog(this);
  seekDialog_ = new SeekDialog(this);
  syncDialog_ = new SyncDialog(this);

  windowPickDialog_ = new PickDialog(this);
  windowPickDialog_->setMessage(tr("Select annots window"));

  processPickDialog_ = new PickDialog(this);
  processPickDialog_->setMessage(tr("Select process window to open"));

  //liveDialog_ = new LiveDialog(this);
  //liveDialog_ = 0; // TODO

  aboutDialog_ = new AboutDialog(this);
  helpDialog_ = new HelpDialog(this);

  tokenView_ = new TokenView(server_, this);
  annotationBrowser_ = new AnnotationBrowser(this);
  annotationEditor_ = new AnnotationEditor(this);
  cloudView_ = new CloudView(this);

  if (ALPHA)
    commentView_ = new CommentView(this);
  else
    commentView_ = 0;

#ifdef USE_MODE_SIGNAL
  messageHandler_ = new MessageHandler(this);
  signalView_ = new SignalView(this);
  recentMessageView_ = new MessageView(this);
#endif // USE_MODE_SIGNAL

#ifdef USE_MODULE_DOLL
  doll_ = new Doll(this);
  connect(doll_, SIGNAL(said(QString)), SLOT(respond(QString)));
#endif // USE_MODULE_DOLL
}

void
MainWindow::createConnections()
{
#ifdef Q_WS_WIN
  connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), SLOT(onFocusedWidgetChanged(QWidget*, QWidget*)));
#endif // Q_WS_WIN

  // Player
  connect(player_, SIGNAL(mediaChanged()), annotationBrowser_, SLOT(clear()));
  connect(player_, SIGNAL(titleIdChanged(int)), SLOT(invalidateToken()));
  connect(player_, SIGNAL(errorEncountered()), SLOT(handlePlayerError()));
  connect(player_, SIGNAL(trackNumberChanged(int)), SLOT(invalidateMediaAndPlay()));
  connect(player_, SIGNAL(endReached()), SLOT(autoPlayNext()));

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
    connect(_playerui->toggleAnnotationButton(), SIGNAL(clicked()), annotationView_, SLOT(togglePlaybackEnabled())); \
    connect(_playerui->previousButton(), SIGNAL(clicked()), SLOT(previous())); \
    connect(_playerui->nextButton(), SIGNAL(clicked()), SLOT(next())); \
    connect(annotationView_, SIGNAL(playbackEnabledChanged(bool)), _playerui, SLOT(setAnnotationEnabled(bool)));

    CONNECT(mainPlayer_)
    CONNECT(miniPlayer_)
    CONNECT(embeddedPlayer_)
  #undef CONNECT

  connect(embeddedPlayer_, SIGNAL(invalidateMenuRequested()), SLOT(invalidateContextMenu()));

  // Queued connections
  connect(this, SIGNAL(responded(QString)), SLOT(respond(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(said(QString,QString)), SLOT(say(QString,QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(showTextRequested(QString)), SLOT(showText(QString)), Qt::QueuedConnection);

  // Annotation graphics view
  connect(this, SIGNAL(addAndShowAnnotationRequested(Annotation)),
          annotationView_, SLOT(addAndShowAnnotation(Annotation)),
          Qt::QueuedConnection);
  connect(this, SIGNAL(setAnnotationsRequested(AnnotationList)),
          annotationView_, SLOT(setAnnotations(AnnotationList)),
          Qt::QueuedConnection);

  // Tokens:
  connect(tokenView_, SIGNAL(aliasSubmitted(Alias)), SLOT(submitAlias(Alias)));
  connect(tokenView_, SIGNAL(tokenBlessedWithId(qint64)), SLOT(blessTokenWithId(qint64)));
  connect(tokenView_, SIGNAL(tokenCursedWithId(qint64)), SLOT(curseTokenWithId(qint64)));

  // Data manager
  connect(dataManager_, SIGNAL(tokenChanged(Token)), tokenView_, SLOT(setToken(Token)));
  connect(dataManager_, SIGNAL(aliasesChanged(AliasList)), tokenView_, SLOT(setAliases(AliasList)));
  connect(tokenView_, SIGNAL(aliasSubmitted(Alias)), dataManager_, SLOT(addAlias(Alias)));

  connect(annotationView_, SIGNAL(annotationAdded(Annotation)), dataManager_, SLOT(addAnnotation(Annotation)));
  connect(annotationView_, SIGNAL(annotationsRemoved()), dataManager_, SLOT(removeAnnotations()));

  connect(dataManager_, SIGNAL(annotationsChanged(AnnotationList)), annotationBrowser_, SLOT(setAnnotations(AnnotationList)));
  connect(dataManager_, SIGNAL(annotationAdded(Annotation)), annotationBrowser_, SLOT(addAnnotation(Annotation)));

  // Forward drag/drop event
  connect(tokenView_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  connect(tokenView_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  connect(tokenView_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  connect(tokenView_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));

#ifdef USE_MODE_SIGNAL
  //connect(signalView_->tokenView(), SIGNAL(aliasSubmitted(Alias)), SLOT(submitAlias(Alias)));

  connect(signalView_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  connect(signalView_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  connect(signalView_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  connect(signalView_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));
#endif // USE_MODE_SIGNAL


  connect(annotationBrowser_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  connect(annotationBrowser_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  connect(annotationBrowser_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  connect(annotationBrowser_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));

  connect(miniPlayer_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  connect(miniPlayer_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  connect(miniPlayer_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  connect(miniPlayer_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));

  // Annotations:
  connect(annotationEditor_, SIGNAL(textSaved(QString)), SLOT(eval(QString)));
  connect(annotationBrowser_, SIGNAL(annotationTextUpdated(QString,qint64)), SLOT(updateAnnotationTextWithId(QString,qint64)));
  connect(annotationView_, SIGNAL(annotationTextUpdated(QString,qint64)), SLOT(updateAnnotationTextWithId(QString,qint64)));

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
  connect(windowPickDialog_, SIGNAL(windowPicked(WId)), annotationView_, SLOT(setTrackedWindow(WId)));
  connect(processPickDialog_, SIGNAL(windowPicked(WId)), annotationView_, SLOT(setTrackedWindow(WId)));
#ifdef USE_MODE_SIGNAL
  connect(processPickDialog_, SIGNAL(windowPicked(WId)), SLOT(openProcessWindow(WId)));
#endif // USE_MODE_SIGNAL
  connect(annotationView_, SIGNAL(trackedWindowChanged(WId)), embeddedPlayer_, SLOT(setContainerWindow(WId)));

#ifdef Q_WS_WIN
  connect(annotationView_, SIGNAL(trackedWindowChanged(WId)), SLOT(setEmbeddedWindow(WId)));
#endif // Q_WS_WIN

  //connect(annotationView_, SIGNAL(trackedWindowDestroyed()), hub_, SLOT(setNormalPlayerMode()));
  //connect(embeddedPlayer_, SIGNAL(trackedWindowDestroyed()), hub_, SLOT(setNormalPlayerMode()));
  connect(annotationView_, SIGNAL(trackedWindowChanged(WId)), SLOT(invalidatePlayerMode()));

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

  connect(loginDialog_, SIGNAL(loginRequested(QString, QString)), SLOT(login(QString, QString)));
  connect(seekDialog_, SIGNAL(seekRequested(qint64)), SLOT(seek(qint64)));

  connect(deviceDialog_, SIGNAL(deviceSelected(QString,bool)), SLOT(openDevicePath(QString,bool)));

#ifdef Q_WS_MAC
  connect(player_, SIGNAL(playing()), SLOT(showVideoViewIfAvailable()));
  connect(player_, SIGNAL(stopped()), videoView_, SLOT(hideView()));
#endif // Q_WS_MAC

#ifdef Q_WS_WIN
  if (!QtWin::isWindowsVistaOrLater())
#endif // Q_WS_WIN
  {
    connect(player_, SIGNAL(playing()), SLOT(disableWindowTransparency()));
    connect(player_, SIGNAL(stopped()), SLOT(enableWindowTransparency()));
  }

  // Agents:
#ifdef USE_MODULE_CLIENTAGENT
  connect(client_, SIGNAL(chatReceived(QString)), SLOT(respond(QString)));
#endif // USE_MODULE_CLIENTAGENT

  // Language:
  connect(TranslatorManager::globalInstance(), SIGNAL(languageChanged()), SLOT(invalidateAppLanguage()));

  // Translator:
  connect(translator_, SIGNAL(translated(QString)), SLOT(showTextAsSubtitle(QString)));

  // Logger:

  connect(cache_, SIGNAL(cleared()), logger_, SLOT(logCacheCleared()));

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

  connect(annotationView_, SIGNAL(trackedWindowDestroyed()), logger_, SLOT(logTrackedWindowDestroyed()));

#ifdef USE_MODULE_CLIENTAGENT
  connect(client_, SIGNAL(authorized()), logger_, SLOT(logClientAgentAuthorized()));
  connect(client_, SIGNAL(deauthorized()), logger_, SLOT(logClientAgentDeauthorized()));
  connect(client_, SIGNAL(authorizationError()), logger_, SLOT(logClientAgentAuthorizationError()));
#endif // USE_MODULE_CLIENTAGENT

#ifdef USE_MODE_SIGNAL
  connect(signalView_, SIGNAL(hookSelected(int,ProcessInfo)), SLOT(openProcessHook(int,ProcessInfo)));
  connect(recentMessageView_, SIGNAL(hookSelected(int)), SLOT(openProcessHook(int)));
  connect(recentMessageView_, SIGNAL(hookSelected(int)), recentMessageView_, SLOT(hide()));
  connect(messageHandler_, SIGNAL(messageReceivedWithId(qint64)), annotationView_, SLOT(showAnnotationsAtPos(qint64)));
  connect(messageHandler_, SIGNAL(messageReceivedWithText(QString)), SLOT(translate(QString)));
#endif // USE_MODE_SIGNAL
}

void
MainWindow::parseArguments(const QStringList &args)
{
  if (args.size() <= 1)
    return;

  // TODO
  openPath(args.back());
}

bool
MainWindow::isValid() const
{ return player_ && player_->isValid();}

void
MainWindow::createActions()
{
#define MAKE_ACTION(_action, _styleid, _receiver, _slot) { \
    _action = new QAction(QIcon(RC_IMAGE_##_styleid), TR(T_MENUTEXT_##_styleid), this); \
    _action->setToolTip(TR(T_TOOLTIP_##_styleid)); \
  } connect(_action, SIGNAL(triggered()), _receiver, _slot);
#define MAKE_TOGGLE(_action, _styleid, _receiver, _slot) { \
    _action = new QAction(QIcon(RC_IMAGE_##_styleid), TR(T_MENUTEXT_##_styleid), this); \
    _action->setToolTip(TR(T_TOOLTIP_##_styleid)); \
    _action->setCheckable(true); \
  } connect(_action, SIGNAL(triggered(bool)), _receiver, _slot);

  MAKE_ACTION(openAct_,         OPEN,           hub_,           SLOT(open()))
  MAKE_ACTION(openFileAct_,     OPENFILE,       this,           SLOT(openFile()))
  MAKE_ACTION(openDeviceAct_,   OPENDEVICE,     this,           SLOT(openDevice()))
  MAKE_ACTION(openVideoDeviceAct_, OPENVIDEODEVICE,  this,      SLOT(openVideoDevice()))
  MAKE_ACTION(openAudioDeviceAct_, OPENAUDIODEVICE,  this,      SLOT(openAudioDevice()))
  MAKE_ACTION(openSubtitleAct_, OPENSUBTITLE,   this,           SLOT(openSubtitle()))
  MAKE_ACTION(playAct_,         PLAY,           hub_,           SLOT(play()))
  MAKE_ACTION(pauseAct_,        PAUSE,          hub_,           SLOT(pause()))
  MAKE_ACTION(previousAct_,     PREVIOUS,       this,           SLOT(previous()))
  MAKE_ACTION(nextAct_,         NEXT,           this,           SLOT(next()))
  MAKE_ACTION(nextFrameAct_,    NEXTFRAME,      player_,        SLOT(nextFrame()))
  MAKE_ACTION(stopAct_,         STOP,           hub_,           SLOT(stop()))
  MAKE_ACTION(replayAct_,       REPLAY,         this,           SLOT(replay()))
  MAKE_ACTION(snapshotAct_,     SNAPSHOT,       this,           SLOT(snapshot()))
  MAKE_TOGGLE(toggleFullScreenModeAct_, FULLSCREEN, hub_,       SLOT(setFullScreenWindowMode(bool)))
  MAKE_TOGGLE(toggleMenuBarVisibleAct_, SHOWMENUBAR, menuBar(), SLOT(setVisible(bool)))
  MAKE_TOGGLE(toggleEmbeddedModeAct_, EMBED,    hub_,           SLOT(setEmbeddedPlayerMode(bool)))
  MAKE_TOGGLE(toggleMiniModeAct_, MINI,         hub_,           SLOT(setMiniPlayerMode(bool)))
  MAKE_TOGGLE(toggleLiveModeAct_, LIVE,         hub_,           SLOT(setLivePlayMode(bool)))
  MAKE_TOGGLE(toggleSyncModeAct_, SYNC,         hub_,           SLOT(setSyncPlayMode(bool)))
  MAKE_TOGGLE(toggleAnnotationVisibleAct_, SHOWANNOT, annotationView_, SLOT(setPlaybackEnabled(bool)))
  MAKE_TOGGLE(toggleSubtitleVisibleAct_, SHOWSUBTITLE, player_, SLOT(setSubtitleVisible(bool)))
  //MAKE_TOGGLE(toggleWindowStaysOnTopAct_, WINDOWSTAYSONTOP, this, SLOT(setWindowStaysOnTop(bool)))
  MAKE_TOGGLE(toggleUserAnonymousAct_,  ANONYMOUS,       this,         SLOT(setUserAnonymous(bool)))
  //MAKE_TOGGLE(toggleUserPanelVisibleAct_, USER,          this,         SLOT(setUserPanelVisible(bool)))
  MAKE_TOGGLE(toggleBlacklistViewVisibleAct_, BLACKLIST,          blacklistView_,    SLOT(setVisible(bool)))
  MAKE_TOGGLE(toggleLoginDialogVisibleAct_, LOGINDIALOG, this,         SLOT(setLoginDialogVisible(bool)))
  MAKE_TOGGLE(toggleWindowPickDialogVisibleAct_,  WINDOWPICKDIALOG,  this,         SLOT(setWindowPickDialogVisible(bool)))
  MAKE_TOGGLE(toggleProcessPickDialogVisibleAct_, PROCESSPICKDIALOG, this,         SLOT(setProcessPickDialogVisible(bool)))
  MAKE_TOGGLE(toggleSeekDialogVisibleAct_,  SEEKDIALOG,  this,         SLOT(setSeekDialogVisible(bool)))
  MAKE_TOGGLE(toggleLiveDialogVisibleAct_,  LIVEDIALOG,  this,         SLOT(setLiveDialogVisible(bool)))
  MAKE_TOGGLE(toggleSyncDialogVisibleAct_,  SYNCDIALOG,  this,         SLOT(setSyncDialogVisible(bool)))
  MAKE_TOGGLE(toggleAnnotationBrowserVisibleAct_, ANNOTATIONBROWSER, annotationBrowser_, SLOT(setVisible(bool)))
  MAKE_TOGGLE(toggleAnnotationEditorVisibleAct_, ANNOTATIONEDITOR, annotationEditor_, SLOT(setVisible(bool)))
  MAKE_TOGGLE(toggleTokenViewVisibleAct_, TOKENVIEW, tokenView_, SLOT(setVisible(bool)))
  MAKE_TOGGLE(toggleCommentViewVisibleAct_, COMMENTVIEW, this, SLOT(setCommentViewVisible(bool)))
  MAKE_TOGGLE(toggleCloudViewVisibleAct_, CLOUDVIEW, cloudView_, SLOT(setVisible(bool)))
  MAKE_TOGGLE(toggleTranslateAct_, TRANSLATE,   this,           SLOT(setTranslateEnabled(bool)))
  MAKE_TOGGLE(toggleSubtitleStaysOnTopAct_, SUBTITLEONTOP,   this,  SLOT(setSubtitleStaysOnTop(bool)))
  MAKE_TOGGLE(toggleEmbeddedPlayerOnTopAct_, EMBEDONTOP,   embeddedPlayer_,  SLOT(setOnTop(bool)))
  MAKE_ACTION(loginAct_,        LOGIN,          this,           SLOT(showLoginDialog()))
  MAKE_ACTION(logoutAct_,       LOGOUT,         this,           SLOT(logout()))
  MAKE_ACTION(clearCacheAct_,   CLEARCACHE,     cache_,         SLOT(clear()))
  MAKE_ACTION(connectAct_,      CONNECT,        server_,        SLOT(updateConnected()))
  MAKE_ACTION(disconnectAct_,   DISCONNECT,     server_,        SLOT(disconnect()))
  MAKE_TOGGLE(setSubtitleColorToDefaultAct_, DEFAULTCOLOR, this, SLOT(setSubtitleColorToDefault()))
  MAKE_TOGGLE(setSubtitleColorToWhiteAct_, WHITECOLOR, this, SLOT(setSubtitleColorToWhite()))
  MAKE_TOGGLE(setSubtitleColorToBlueAct_, BLUECOLOR, this, SLOT(setSubtitleColorToBlue()))
  MAKE_TOGGLE(setSubtitleColorToOrangeAct_, ORANGECOLOR, this, SLOT(setSubtitleColorToOrange()))
  MAKE_TOGGLE(setSubtitleColorToPurpleAct_, PURPLECOLOR, this, SLOT(setSubtitleColorToPurple()))
  MAKE_TOGGLE(setSubtitleColorToBlackAct_, BLACKCOLOR, this, SLOT(setSubtitleColorToBlack()))
  MAKE_TOGGLE(setSubtitleColorToRedAct_, REDCOLOR, this, SLOT(setSubtitleColorToRed()))
#ifdef USE_MODE_SIGNAL
  MAKE_TOGGLE(toggleSignalViewVisibleAct_, SIGNALVIEW, this, SLOT(setSignalViewVisible(bool)))
  MAKE_TOGGLE(toggleRecentMessageViewVisibleAct_, RECENTMESSAGES, this, SLOT(setRecentMessageViewVisible(bool)))
#endif // USE_MODE_SIGNAL

  MAKE_ACTION(forward5sAct_,    FORWARD5S,      this,   SLOT(forward5s()))
  MAKE_ACTION(backward5sAct_,   BACKWARD5S,     this,   SLOT(backward5s()))
  MAKE_ACTION(forward10sAct_,   FORWARD10S,     this,   SLOT(forward10s()))
  MAKE_ACTION(backward10sAct_,  BACKWARD10S,    this,   SLOT(backward10s()))
  MAKE_ACTION(forward30sAct_,   FORWARD30S,     this,   SLOT(forward30s()))
  MAKE_ACTION(backward30sAct_,  BACKWARD30S,    this,   SLOT(backward30s()))
  MAKE_ACTION(forward90sAct_,   FORWARD60S,     this,   SLOT(forward60s()))
  MAKE_ACTION(backward90sAct_,  BACKWARD60S,    this,   SLOT(backward60s()))
  MAKE_ACTION(forward90sAct_,   FORWARD90S,     this,   SLOT(forward90s()))
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
  MAKE_TOGGLE(toggleAnnotationLanguageToUnknownAct_, UNKNOWNLANGUAGE, this,SLOT(invalidateAnnotationLanguages()))
  MAKE_TOGGLE(toggleAnnotationLanguageToAnyAct_, ANYLANGUAGE, this,SLOT(invalidateAnnotationLanguages()))

  MAKE_ACTION(showMaximizedAct_,        MAXIMIZE,       this,    SLOT(showMaximized()))
  MAKE_ACTION(showMinimizedAct_,        MINIMIZE,       this,    SLOT(showMinimized()))
  MAKE_ACTION(showNormalAct_,           RESTORE,        this,    SLOT(showNormal()))

  toggleAutoPlayNextAct_ = new QAction(TR(T_MENUTEXT_AUTOPLAYNEXT), this); {
    toggleAutoPlayNextAct_->setToolTip(TR(T_TOOLTIP_AUTOPLAYNEXT));
    toggleAutoPlayNextAct_->setCheckable(true);
    toggleAutoPlayNextAct_->setChecked(Settings::globalInstance()->isAutoPlayNext());
  }

#ifdef Q_WS_WIN
  if (!QtWin::isWindowsVistaOrLater())
#endif // Q_WS_WIN
  {
    MAKE_TOGGLE(setThemeToDefaultAct_,    DEFAULTTHEME,   this,   SLOT(setThemeToDefault()))
    MAKE_TOGGLE(setThemeToRandomAct_,     RANDOMTHEME,    this,   SLOT(setThemeToRandom()))
    MAKE_TOGGLE(setThemeToBlack1Act_,     BLACKTHEME1,    this,   SLOT(setThemeToBlack1()))
    MAKE_TOGGLE(setThemeToBlack2Act_,     BLACKTHEME2,    this,   SLOT(setThemeToBlack2()))
    MAKE_TOGGLE(setThemeToBlue1Act_,      BLUETHEME1,     this,   SLOT(setThemeToBlue1()))
    MAKE_TOGGLE(setThemeToBlue2Act_,      BLUETHEME2,     this,   SLOT(setThemeToBlue2()))
    MAKE_TOGGLE(setThemeToBrown1Act_,     BROWNTHEME1,    this,   SLOT(setThemeToBrown1()))
    MAKE_TOGGLE(setThemeToBrown2Act_,     BROWNTHEME2,    this,   SLOT(setThemeToBrown2()))
    MAKE_TOGGLE(setThemeToGreen1Act_,     GREENTHEME1,    this,   SLOT(setThemeToGreen1()))
    MAKE_TOGGLE(setThemeToGreen2Act_,     GREENTHEME2,    this,   SLOT(setThemeToGreen2()))
    MAKE_TOGGLE(setThemeToLightBlue1Act_, LIGHTBLUETHEME1,this,   SLOT(setThemeToLightBlue1()))
    MAKE_TOGGLE(setThemeToLightBlue2Act_, LIGHTBLUETHEME2,this,   SLOT(setThemeToLightBlue2()))
    MAKE_TOGGLE(setThemeToOrange1Act_,    ORANGETHEME1,   this,   SLOT(setThemeToOrange1()))
    MAKE_TOGGLE(setThemeToOrange2Act_,    ORANGETHEME2,   this,   SLOT(setThemeToOrange2()))
    MAKE_TOGGLE(setThemeToPink1Act_,      PINKTHEME1,     this,   SLOT(setThemeToPink1()))
    MAKE_TOGGLE(setThemeToPink2Act_,      PINKTHEME2,     this,   SLOT(setThemeToPink2()))
    MAKE_TOGGLE(setThemeToPurple1Act_,    PURPLETHEME1,   this,   SLOT(setThemeToPurple1()))
    MAKE_TOGGLE(setThemeToPurple2Act_,    PURPLETHEME2,   this,   SLOT(setThemeToPurple2()))
    MAKE_TOGGLE(setThemeToRed1Act_,       REDTHEME1,      this,   SLOT(setThemeToRed1()))
    MAKE_TOGGLE(setThemeToRed2Act_,       REDTHEME2,      this,   SLOT(setThemeToRed2()))
    MAKE_TOGGLE(setThemeToYellow1Act_,    YELLOWTHEME1,   this,   SLOT(setThemeToYellow1()))
    MAKE_TOGGLE(setThemeToYellow2Act_,    YELLOWTHEME2,   this,   SLOT(setThemeToYellow2()))
  }

  MAKE_ACTION(aboutAct_,        ABOUT,  this,   SLOT(about()))
  MAKE_ACTION(helpAct_,         HELP,   this,   SLOT(help()))
  MAKE_ACTION(quitAct_,         QUIT,   this,   SLOT(close()))

  openAct_->setShortcuts(QKeySequence::Open);
  helpAct_->setShortcuts(QKeySequence::HelpContents);
  quitAct_->setShortcuts(QKeySequence::Quit);

#undef MAKE_ACTION
}

void
MainWindow::createMenus()
{
  // Context menu
  contextMenu_ = new QMenu(TR(T_TITLE_PROGRAM), this); {
    UiStyle::globalInstance()->setContextMenuStyle(contextMenu_, true); // persistent = true
    embeddedPlayer_->setMenu(contextMenu_);
  }

  userMenu_ = new QMenu(TR(T_TITLE_PROGRAM), this); {
    UiStyle::globalInstance()->setContextMenuStyle(userMenu_, true); // persistent = true
    PlayerUi *players[] = { mainPlayer_, miniPlayer_, embeddedPlayer_ };
    BOOST_FOREACH (PlayerUi *p, players) {
      p->userButton()->setMenu(userMenu_);
      connect(p, SIGNAL(invalidateUserMenuRequested()), SLOT(invalidateUserMenu()));
    }
  }

  helpContextMenu_ = new QMenu(this); {
    UiStyle::globalInstance()->setContextMenuStyle(helpContextMenu_, true); // persistent = true
    helpContextMenu_->setTitle(TR(T_MENUTEXT_HELP));
    helpContextMenu_->setToolTip(TR(T_TOOLTIP_HELP));

    helpContextMenu_->addAction(helpAct_);
    helpContextMenu_->addAction(aboutAct_);
  }

  appLanguageMenu_ = new QMenu(this); {
    UiStyle::globalInstance()->setContextMenuStyle(appLanguageMenu_, true); // persistent = true

    appLanguageMenu_->setTitle("Language"); // no tr wrapping "language"
    appLanguageMenu_->setToolTip(TR(T_TOOLTIP_APPLANGUAGE));

    appLanguageMenu_->addAction(setAppLanguageToEnglishAct_);
    appLanguageMenu_->addAction(setAppLanguageToJapaneseAct_);
    //appLanguageMenu_->addAction(setAppLanguageToTraditionalChineseAct_); // Temporarily disabled since no traditional chinese at this time
    appLanguageMenu_->addAction(setAppLanguageToSimplifiedChineseAct_);
  }

  userLanguageMenu_ = new QMenu(this); {
    UiStyle::globalInstance()->setContextMenuStyle(userLanguageMenu_, true); // persistent = true

    userLanguageMenu_->setTitle(TR(T_MENUTEXT_USERLANGUAGE));
    userLanguageMenu_->setToolTip(TR(T_TOOLTIP_USERLANGUAGE));

    userLanguageMenu_->addAction(setUserLanguageToEnglishAct_);
    userLanguageMenu_->addAction(setUserLanguageToJapaneseAct_);
    userLanguageMenu_->addAction(setUserLanguageToChineseAct_);
    userLanguageMenu_->addAction(setUserLanguageToKoreanAct_);
    userLanguageMenu_->addAction(setUserLanguageToUnknownAct_);
  }

  annotationLanguageMenu_ = new QMenu(this); {
    UiStyle::globalInstance()->setContextMenuStyle(annotationLanguageMenu_, true); // persistent = true

    annotationLanguageMenu_->setTitle(TR(T_MENUTEXT_ANNOTATIONLANGUAGE));
    annotationLanguageMenu_->setToolTip(TR(T_TOOLTIP_ANNOTATIONLANGUAGE));

    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToAnyAct_);
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToUnknownAct_);
    annotationLanguageMenu_->addSeparator();
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToEnglishAct_);
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToJapaneseAct_);
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToChineseAct_);
    annotationLanguageMenu_->addAction(toggleAnnotationLanguageToKoreanAct_);
  }

#ifdef Q_WS_WIN
  if (!QtWin::isWindowsVistaOrLater())
#endif // Q_WS_WIN
  {
    themeMenu_ = new QMenu(this);

    UiStyle::globalInstance()->setContextMenuStyle(themeMenu_, true); // persistent = true
    themeMenu_->setTitle(TR(T_MENUTEXT_THEME));
    themeMenu_->setToolTip(TR(T_TOOLTIP_THEME));

    themeMenu_->addAction(setThemeToDefaultAct_);
    themeMenu_->addAction(setThemeToRandomAct_);
    themeMenu_->addSeparator();
    themeMenu_->addAction(setThemeToBlack1Act_);
    themeMenu_->addAction(setThemeToBlack2Act_);
    themeMenu_->addAction(setThemeToBlue1Act_);
    themeMenu_->addAction(setThemeToBlue2Act_);
    themeMenu_->addAction(setThemeToBrown1Act_);
    themeMenu_->addAction(setThemeToBrown2Act_);
    themeMenu_->addAction(setThemeToGreen1Act_);
    themeMenu_->addAction(setThemeToGreen2Act_);
    themeMenu_->addAction(setThemeToLightBlue1Act_);
    themeMenu_->addAction(setThemeToLightBlue2Act_);
    themeMenu_->addAction(setThemeToOrange1Act_);
    themeMenu_->addAction(setThemeToOrange2Act_);
    themeMenu_->addAction(setThemeToPink1Act_);
    themeMenu_->addAction(setThemeToPink2Act_);
    themeMenu_->addAction(setThemeToPurple1Act_);
    themeMenu_->addAction(setThemeToPurple2Act_);
    themeMenu_->addAction(setThemeToRed1Act_);
    themeMenu_->addAction(setThemeToRed2Act_);
    themeMenu_->addAction(setThemeToYellow1Act_);
    themeMenu_->addAction(setThemeToYellow2Act_);
  }

  subtitleStyleMenu_ = new QMenu(this); {
    UiStyle::globalInstance()->setContextMenuStyle(subtitleStyleMenu_, true); // persistent = true

    subtitleStyleMenu_->setTitle(TR(T_MENUTEXT_SUBTITLESTYLE));
    subtitleStyleMenu_->setToolTip(TR(T_TOOLTIP_SUBTITLESTYLE));

    subtitleStyleMenu_->addAction(setSubtitleColorToDefaultAct_);
    subtitleStyleMenu_->addSeparator();
    subtitleStyleMenu_->addAction(setSubtitleColorToWhiteAct_);
    subtitleStyleMenu_->addAction(setSubtitleColorToBlueAct_);
    subtitleStyleMenu_->addAction(setSubtitleColorToPurpleAct_);
    subtitleStyleMenu_->addAction(setSubtitleColorToRedAct_);
    subtitleStyleMenu_->addAction(setSubtitleColorToOrangeAct_);
    subtitleStyleMenu_->addAction(setSubtitleColorToBlackAct_);
  }

  browseMenu_ = new QMenu(this); {
    UiStyle::globalInstance()->setContextMenuStyle(browseMenu_, true); // persistent = true

    browseMenu_->setTitle(TR(T_MENUTEXT_BROWSE));
    browseMenu_->setToolTip(TR(T_TOOLTIP_BROWSE));
  }

  trackMenu_ = new QMenu(this); {
    UiStyle::globalInstance()->setContextMenuStyle(trackMenu_, true); // persistent = true

    trackMenu_->setTitle(TR(T_MENUTEXT_TRACK));
    trackMenu_->setToolTip(TR(T_TOOLTIP_TRACK));
  }

  recentMenu_ = new QMenu(this); {
    UiStyle::globalInstance()->setContextMenuStyle(recentMenu_, true); // persistent = true
    recentMenu_->setTitle(TR(T_MENUTEXT_RECENT));
    recentMenu_->setToolTip(TR(T_TOOLTIP_RECENT));
  }

  playlistMenu_ = new QMenu(this); {
    UiStyle::globalInstance()->setContextMenuStyle(playlistMenu_, true); // persistent = true
    playlistMenu_->setTitle(TR(T_MENUTEXT_PLAYLIST));
    playlistMenu_->setToolTip(TR(T_TOOLTIP_PLAYLIST));
  }

  backwardMenu_ = new QMenu(this); {
    backwardMenu_->setIcon(QIcon(RC_IMAGE_BACKWARD));
    backwardMenu_->setTitle(TR(T_MENUTEXT_BACKWARD));
    backwardMenu_->setToolTip(TR(T_TOOLTIP_BACKWARD));
    UiStyle::globalInstance()->setContextMenuStyle(backwardMenu_, true); // persistent = true

    backwardMenu_->addAction(backward5sAct_);
    backwardMenu_->addAction(backward30sAct_);
    backwardMenu_->addAction(backward90sAct_);
    backwardMenu_->addAction(backward5mAct_);
    backwardMenu_->addAction(backward10mAct_);
  }

  forwardMenu_ = new QMenu(this); {
    forwardMenu_->setIcon(QIcon(RC_IMAGE_FORWARD));
    forwardMenu_->setTitle(TR(T_MENUTEXT_FORWARD));
    forwardMenu_->setToolTip(TR(T_TOOLTIP_FORWARD));
    UiStyle::globalInstance()->setContextMenuStyle(forwardMenu_, true); // persistent = true

    forwardMenu_->addAction(forward5sAct_);
    forwardMenu_->addAction(forward30sAct_);
    forwardMenu_->addAction(forward90sAct_);
    forwardMenu_->addAction(forward5mAct_);
    forwardMenu_->addAction(forward10mAct_);
  }

  openMenu_ = new QMenu(this); {
    openMenu_->setTitle(TR(T_MENUTEXT_OPENCONTEXTMENU));
    openMenu_->setToolTip(TR(T_TOOLTIP_OPENCONTEXTMENU));
    openMenu_->setIcon(QIcon(RC_IMAGE_OPENCONTEXTMENU));
    UiStyle::globalInstance()->setContextMenuStyle(openMenu_, true); // persistent = true
  }

  subtitleMenu_ = new QMenu(this); {
    subtitleMenu_->setIcon(QIcon(RC_IMAGE_SUBTITLE));
    subtitleMenu_->setTitle(TR(T_MENUTEXT_SUBTITLE));
    subtitleMenu_->setToolTip(TR(T_TOOLTIP_SUBTITLE));
    UiStyle::globalInstance()->setContextMenuStyle(subtitleMenu_, true); // persistent = true
  }

  sectionMenu_ = new QMenu(this); {
    sectionMenu_->setIcon(QIcon(RC_IMAGE_SECTION));
    sectionMenu_->setTitle(TR(T_MENUTEXT_SECTION));
    sectionMenu_->setToolTip(TR(T_TOOLTIP_SECTION));
    UiStyle::globalInstance()->setContextMenuStyle(sectionMenu_, true); // persistent = true
  }


//#ifndef Q_WS_WIN
  // Use menuBar_ instead MainWindow::menuBar() to enable customization.
  // It is important to create the menubar WITHOUT PARENT, so that diff windows could share the same menubar.
  // See: http://doc.qt.nokia.com/latest/mac-differences.html#menu-bar
  // See: http://doc.qt.nokia.com/stable/qmainwindow.html#menuBar

  // jichi 11/11/11:  It's important not to use act like quitAct_ or aboutAct_ with translations.

  //menuBar_ = new QMenuBar;
  fileMenu_ = menuBar()->addMenu(tr("&File")); {
    fileMenu_->addAction(openAct_);
    fileMenu_->addSeparator();
    fileMenu_->addAction(openSubtitleAct_);
    fileMenu_->addAction(openDeviceAct_);
    fileMenu_->addAction(openVideoDeviceAct_);
#ifdef Q_WS_WIN // TODO add support for Mac/Linux
    fileMenu_->addAction(openAudioDeviceAct_);
#endif // Q_WS_WIN
    fileMenu_->addSeparator();
    fileMenu_->addAction(tr("&Quit"), this, SLOT(close())); // DO NOT TRANSLATE ME
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

  helpMenu_ = menuBar()->addMenu(tr("&Help"));
  helpMenu_->addAction(tr("&Help"), this, SLOT(help())); // DO NOT TRANSLATE ME
  helpMenu_->addAction(tr("&About"), this, SLOT(about())); // DO NOT TRANSLATE ME
//#endif // !Q_WS_WIN
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
  // Update player mode
  switch (hub_->tokenMode()) {
  case SignalHub::MediaTokenMode:
    break;
  case SignalHub::SignalTokenMode:
    if (annotationView_->trackedWindow())
      hub_->setEmbeddedPlayerMode();
    break;
  }

#ifdef USE_WIN_QTH
  messageHandler_->setActive(!hub_->isMediaTokenMode());
#endif // USE_WIN_QTH
}

void
MainWindow::updatePlayMode()
{
  // TODO
}

void
MainWindow::invalidatePlayerMode()
{
  switch (hub_->playerMode()) {
  case SignalHub::NormalPlayerMode:
  case SignalHub::MiniPlayerMode:
    if (annotationView_->trackedWindow())
      hub_->setEmbeddedPlayerMode();
    break;
  case SignalHub::EmbeddedPlayerMode:
    if (!annotationView_->trackedWindow())
      hub_->setNormalPlayerMode();
    break;
  }
}

void
MainWindow::updatePlayerMode()
{
  //bool m = hub_->isMediaTokenMode();
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
    if (annotationView_->trackedWindow())
      hide();
    mainPlayer_->hide();
    miniPlayer_->hide();
    embeddedPlayer_->show();
    osdWindow_->raise();
    break;
  }

  annotationView_->invalidateGeometry();
}

void
MainWindow::updateWindowMode()
{
  switch (hub_->windowMode()) {
  case SignalHub::FullScreenWindowMode:
#ifdef USE_WIN_DWM
    if (UiStyle::isAeroAvailable())
      UiStyle::globalInstance()->setWindowDwmEnabled(this, false);
#endif // USE_WIN_DWM
#ifdef Q_WS_X11
    if (hub_->isMediaTokenMode() && player_->hasMedia())
      UiStyle::globalInstance()->setBlackBackground(this);
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
    case SignalHub::SignalTokenMode:
      hub_->setEmbeddedPlayerMode(); // always embed in full screen signal mode
      annotationView_->setFullScreenMode();
      break;
    }
    break;

  case SignalHub::NormalWindowMode:
    if (embeddedPlayer_->isVisible())
      embeddedPlayer_->hide();

    annotationView_->setFullScreenMode(false);

    if (!annotationView_->trackedWindow())
      hub_->setNormalPlayerMode();
    else {
      hide();
      updatePlayerMode();
    }
    break;
  }
}

/*
void
MainWindow::setVisible(bool visible)
{
  if (visible == isVisible())
    return;

  Base::setVisible(visible);

  if (!hub_->isFullScreenWindowMode()) {
#ifdef USE_WIN_DWM
    if (UiStyle::isAeroAvailable())
      UiStyle::globalInstance()->setWindowDwmEnabled(this, true);
    else
#endif // USE_WIN_DWM
      UiStyle::globalInstance()->setWindowBackground(this, false); // persistent is false
  }
}
*/

// - Open file -

void
MainWindow::open()
{
  switch (hub_->tokenMode()) {
  case SignalHub::MediaTokenMode: openFile(); break;
  case SignalHub::SignalTokenMode: openProcess(); break;
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
    + TR(T_FORMAT_PROGRAM)   + ("(" G_FORMAT_PROGRAM ")" ";;")
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
#endif // Q_WS_MAC
#ifdef Q_OS_UNIX
  if (QtUnix::isDeviceFile(path))
    return true;
#endif // Q_OS_UNIX
  return false;
}

void
MainWindow::openDevice()
{ deviceDialog_->show(); }

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

  openPath(mrl);
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
    openPath(path);
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
    foreach (QString fileName, l)
      openSubtitlePath(fileName);
  }
}

void
MainWindow::openUrl(const QUrl &url)
{
  if (!url.isValid())
    return;

  QString path = url.toLocalFile();
  if (!path.isEmpty())
    openSource(path);
  else
    openPath(url.toEncoded(), false); // check path = false
}

void
MainWindow::openUrls(const QList<QUrl> &urls)
{
  // FIXME: add to playlist rather then this approach!!!!!
  if (!urls.empty()) {
    playlist_.clear();
    foreach (const QUrl &url, urls) {
      QString path = url.toLocalFile();
      if (!path.isEmpty())
        playlist_.append(path);
    }
    openUrl(urls.front());
  }
}

void
MainWindow::openMimeData(const QMimeData *mime)
{
  if (mime && mime->hasUrls())
    openUrls(mime->urls());
}


void
MainWindow::setRecentOpenedFile(const QString &path)
{
  recentOpenedFile_ = path;
  if (playlist_.size() > 1)
    invalidatePlaylistMenu();
}

void
MainWindow::openPath(const QString &path, bool checkPath)
{
  if (hub_->isSignalTokenMode())
    hub_->stop();
  hub_->setMediaTokenMode();

  recentDigest_.clear();

  setRecentOpenedFile(path);

  setBrowsedFile(path);

  if (checkPath) {
    QFileInfo fi(removeUrlPrefix(path));
    if (!fi.exists()) {
      warn(TR(T_ERROR_BAD_FILEPATH) + ": " + path);
      return;
    }
  }

  tokenType_ = fileType(path);

  player_->openMedia(path);

  addRecent(path);
  if (player_->hasPlaylist())
    return;

  if (player_->hasMedia()) {
    recentDigest_.clear();
    invalidateMediaAndPlay();
  }
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
  if (player_->hasMedia()) {
    if (async &&
        recentDigest_.isEmpty() &&
        isDigestReady(player_->mediaPath())) {
      log(tr("analyzing media ..."));
      QThreadPool::globalInstance()->start(new task_::invalidateMediaAndPlay(this));
      DOUT("exit: returned from async branch");
      return;
    }

    DOUT("playerMutex locking");
    playerMutex_.lock();
    DOUT("playerMutex locked");

    dataManager_->token().setId(0);
    dataManager_->removeAliases();
    annotationView_->removeAnnotations();

    QString path = player_->mediaPath();
    if (isDigestReady(path)) {
      if (recentDigest_.isEmpty())
        recentDigest_ = digestFromFile(removeUrlPrefix(path));
      if (!recentDigest_.isEmpty())
        setToken(path);
    }

    addRecent(path);
    //if (mode_ == NormalPlayMode)
    //  videoView_->resize(INIT_WINDOW_SIZE);

    player_->play();
    QTimer::singleShot(0, player_, SLOT(loadExternalSubtitles())); // load external subtitles later

    invalidateWindowTitle();

#ifdef Q_WS_MAC
    // ensure videoView_ stretch in full screen mode
    if (hub_->isEmbeddedPlayerMode() && !videoView_->isViewVisible()) {
      videoView_->showView();
      hub_->setEmbeddedPlayerMode(false);
      hub_->setEmbeddedPlayerMode(true);
    }
#endif // Q_WS_MAC

#ifdef Q_WS_X11
    if (hub_->isFullScreenWindowMode())
      UiStyle::globalInstance()->setBlackBackground(this);
#endif // Q_WS_X11

    // SingleShot only because of multi-threading problem.
    QTimer::singleShot(0, this, SLOT(invalidateTrackMenu()));

    DOUT("playerMutex unlocking");
    playerMutex_.unlock();
    DOUT("playerMutex unlocked");
  }
  DOUT("exit");
}

void
MainWindow::openSubtitlePath(const QString &path)
{
  if (!player_->hasMedia()) {
    log(TR(T_ERROR_NO_MEDIA))   ;
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

#define MAINWINDOW_ACTION_TRIGGER(_action) \
  void \
  MainWindow::_action() \
  { \
    Q_ASSERT(_action##Act_); \
    _action##Act_->trigger(); \
  }

  //MAINWINDOW_ACTION_TRIGGER(pause);
  //MAINWINDOW_ACTION_TRIGGER(stop);
  MAINWINDOW_ACTION_TRIGGER(nextFrame);

#undef MAINWINDOW_ACTION_TRIGGER

void
MainWindow::pause()
{
  switch (hub_->tokenMode()) {
  case SignalHub::SignalTokenMode:
    break;

  case SignalHub::MediaTokenMode:
    if (player_->isPlaying())
      player_->pause();
    break;
  }
}

void
MainWindow::stop()
{
  switch (hub_->tokenMode()) {
  case SignalHub::SignalTokenMode:
#ifdef USE_MODE_SIGNAL
    messageHandler_->setActive(false);
#endif // USE_MODE_SIGNAL
    break;

  case SignalHub::MediaTokenMode:
    if (!player_->isStopped())
      player_->stop();
    break;
  }
}

bool
MainWindow::isAutoPlayNext() const
{ return toggleAutoPlayNextAct_->isChecked(); }

void
MainWindow::play()
{
  switch (hub_->tokenMode()) {
  case SignalHub::SignalTokenMode:
#ifdef USE_MODE_SIGNAL
    messageHandler_->setActive(true);
#endif // USE_MODE_SIGNAL
    break;

  case SignalHub::MediaTokenMode:
    if (player_->hasMedia())
      player_->play();
    else
      open();
    break;
  }
}

void
MainWindow::playPause()
{
  switch (hub_->tokenMode()) {
  case SignalHub::SignalTokenMode:
    if (hub_->isPlaying())
      hub_->pause();
    else
      hub_->play();
    break;

  case SignalHub::MediaTokenMode:
    if (player_->hasMedia())
      player_->playPause();
    else
      open();
    break;
  }
}

void
MainWindow::replay()
{
  if (hub_->isSignalTokenMode()) {
    play();
    return;
  }

  if (player_->hasMedia()) {
    player_->setTime(0);
    emit seeked();

    // A quick fix to solve delay on position update
    // TO BE REMOVED!
    if (hasVisiblePlayer())
      visiblePlayer()->invalidatePositionSlider();

    if (!player_->isPlaying())
      player_->play();
  }
}

void
MainWindow::snapshot()
{
  switch (hub_->tokenMode()) {
  case SignalHub::SignalTokenMode:
    if (annotationView_->trackedWindow())
      grabber_->grabWindow(annotationView_->trackedWindow());
    else
      grabber_->grabDesktop();
    log(tr("snapshot saved on the destop"));
    break;

  case SignalHub::MediaTokenMode:
    if (player_->hasMedia() && !player_->isStopped()) {

      QString mediaName = QFileInfo(player_->mediaPath()).fileName();
      QTime time = Core::msecs2time(player_->time());

      QString saveName = mediaName + "__" + time.toString("hh_mm_ss_zzz") + ".png";
      QString savePath = Core::Os::desktopPath() + "/" + saveName;

      bool succeed = player_->snapshot(savePath);
      if (succeed)
        say(": " + TR(T_SUCCEED_SNAPSHOT_SAVED), "purple");
      else
        say(": " + TR(T_ERROR_SNAPSHOT_FAILED) + ": " + savePath, "red");
    }
    break;
  }
}

void
MainWindow::seek(qint64 time)
{
  DOUT("enter: time =" << time);

  if (hub_->isSignalTokenMode())
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
  switch (hub_->tokenMode()) {
  case SignalHub::SignalTokenMode:
    // TODO: volume
    break;

  case SignalHub::MediaTokenMode:
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
    break;
  }
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
  switch (hub_->tokenMode()) {
  case SignalHub::SignalTokenMode:
    return hub_->isPlaying();

  case SignalHub::MediaTokenMode:
    return player_->isPlaying();

  default:
    Q_ASSERT(0);
    return false;
  }
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
#ifdef USE_WIN_DWM
    setAeroStyleEnabled(false);
#endif // USE_WIN_DWM
    mainPlayer_->hide();
    showFullScreen();
    embeddedPlayer_->setVisible(!miniPlayer_->isVisible());
    break;

  case NormalPlayMode:
    embeddedPlayer_->hide();
    showNormal();
    mainPlayer_->setVisible(!miniPlayer_->isVisible());
    annotationView_->invalidateSize(); // Otherwise the height doesn't look right
#ifdef USE_WIN_DWM
    setAeroStyleEnabled(true);
#endif // USE_WIN_DWM
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
{ aboutDialog_->show(); }

// TODO
void
MainWindow::help()
{ helpDialog_->show(); }

// - Update -

void
MainWindow::invalidateWindowTitle()
{
  switch (hub_->tokenMode()) {
  case SignalHub::SignalTokenMode:
#ifdef USE_MODE_SIGNAL
    setWindowTitle(messageHandler_->processInfo().processName);
#endif // USE_MODE_SIGNAL
    break;

  case SignalHub::MediaTokenMode:
    if (player_->hasMedia()) {
      QString title = player_->mediaTitle();
      if (title.isEmpty()) {
        title = player_->mediaPath();
        if (!title.isEmpty())
          title = QFileInfo(title).fileName();
      }
      if (title.isEmpty())
        title = TR(T_TITLE_PROGRAM);

      setWindowTitle(title);
    } else
      setWindowTitle(TR(T_TITLE_PROGRAM));
    break;
  }
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
MainWindow::showLoginDialog()
{ setLoginDialogVisible(true); }

void
MainWindow::hideLoginDialog()
{ setLoginDialogVisible(false); }

void
MainWindow::setLoginDialogVisible(bool visible)
{
  if (visible) {
    loginDialog_->setUserName(server_->user().name());
    loginDialog_->setPassword(server_->user().password());
    loginDialog_->show();
    loginDialog_->raise();
  } else
    loginDialog_->hide();
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
  if (visible) {
    seekDialog_->setTime(player_->time());
    seekDialog_->show();
    seekDialog_->raise();
  } else
    seekDialog_->hide();
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
  syncDialog_->setVisible(visible);
  if (visible)
    syncDialog_->raise();
}

void
MainWindow::showCommentView()
{ setCommentViewVisible(true); }

void
MainWindow::hideCommentView()
{ setCommentViewVisible(false); }

void
MainWindow::setCommentViewVisible(bool visible)
{
  if (visible && dataManager_->hasToken())
    commentView_->setTokenId(dataManager_->token().id());
  commentView_->setVisible(visible);
  if (visible)
    commentView_->raise();
}

void
MainWindow::showWindowPickDialog()
{ setWindowPickDialogVisible(true); }

void
MainWindow::setWindowPickDialogVisible(bool visible)
{
  windowPickDialog_->setVisible(visible);
  if (visible)
    windowPickDialog_->raise();
}

void
MainWindow::setProcessPickDialogVisible(bool visible)
{
  processPickDialog_->setVisible(visible);
  if (visible)
    processPickDialog_->raise();
}

// - Annotations -

void
MainWindow::submitAlias(const Alias &input, bool async)
{
  DOUT("enter: async =" << async << ", text =" << input.text());
  if (async) {
    log(tr("connecting server to submit alias ..."));
    QThreadPool::globalInstance()->start(new task_::submitAlias(input, this));
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  Alias a = input;
  //bool id = server_->submitAlias(a);
  dataServer_->submitAlias(a);
  //if (id)
    log(tr("alias saved") + ": " + a.text());
  //else
  //  warn(tr("failed to update alias text") + ": " + a.text());

  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");
  DOUT("exit");
}

void
MainWindow::updateAnnotationTextWithId(const QString &text, qint64 id)
{
  bool ok = server_->updateAnnotationTextWithId(text, id);
  if (ok)
    log(tr("annotation saved") + ": " + text);
  else
    warn(tr("failed to update annotation text") + ": " + text);
}

QString
MainWindow::removeUrlPrefix(const QString &url)
{
  if (url.startsWith(PLAYER_URL_CD, Qt::CaseInsensitive))
    return url.mid(::strlen(PLAYER_URL_CD));

  if (url.startsWith(PLAYER_URL_VCD, Qt::CaseInsensitive))
    return url.mid(::strlen(PLAYER_URL_VCD));

  if (url.startsWith(PLAYER_URL_DVD, Qt::CaseInsensitive))
    return url.mid(::strlen(PLAYER_URL_DVD));

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
  foreach (QString suffix, supportedProgramSuffices)
    if (fileName.endsWith("." + suffix, Qt::CaseInsensitive))
      return Token::TT_Program;
#endif // USE_MODE_SIGNAL

  if (Player::isSupportedVideo(fileName))
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
MainWindow::invalidateToken()
{ setToken(QString()); }

void
MainWindow::showVideoViewIfAvailable()
{
#ifdef Q_WS_MAC
  if (tokenType_ != Token::TT_Audio)
    videoView_->showView();
#endif // Q_WS_MAC
}

void
MainWindow::setToken(const QString &input, bool async)
{
  DOUT("enter: async =" << async << ", path =" << input);
  if (async) {
    log(tr("connecting server to query media/game token ..."));
    QThreadPool::globalInstance()->start(new task_::setToken(input, this));
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  Token token = dataManager_->token();
  dataManager_->removeToken();

  Alias alias;

  if (!input.isEmpty()) {
    token.setType(tokenType_ = fileType(input));

    token.setDigestType(0);

    QString path = removeUrlPrefix(input);
    token.setDigest(recentDigest_);

    // If isAliasReady, make fileName as alias and guess tt.
    // Otherwise, keep token's original tt from tokenView_
    if (isAliasReady(path)) {
      QString fileName = QFileInfo(path).fileName();
      if (!fileName.isEmpty()) {
//#ifdef Q_WS_MAC
//        if (tt == Token::TT_Audio)
//          videoView_->hideView();
//#endif // Q_WS_MAC

        if (fileName.size() > Traits::MAX_ALIAS_LENGTH) {
          fileName = fileName.mid(0, Traits::MAX_ALIAS_LENGTH);
          warn(TR(T_WARNING_LONG_STRING_TRUNCATED) + ": " + fileName);
        }

        alias.setType(Alias::AT_Source);
        alias.setLanguage(server_->user().language());
        alias.setText(fileName);
        alias.setUpdateTime(QDateTime::currentMSecsSinceEpoch() / 1000);
      }
    }
  }

  if (!token.hasDigest()) {
    DOUT("inetMutex unlocking");
    inetMutex_.unlock();
    DOUT("inetMutex unlocked");
    DOUT("exit from empty digest branch");
    return;
  }

  token.setCreateTime(QDateTime::currentMSecsSinceEpoch() / 1000);

  if (hub_->isMediaTokenMode() && player_->hasMedia()) {
    int tid = 0;
    if (player_->hasPlaylist())
      tid = player_->trackNumber();
    else if (player_->hasTitles())
      tid = player_->titleId();

    if (tid < 0)
      tid = 0;
    token.setDigestType(tid);
  }

  AliasList aliases;

  qint64 tid = dataServer_->submitTokenAndAlias(token, alias);
  if (tid) {
    Token t = dataServer_->selectTokenWithId(tid);
    if (t.isValid()) {
      aliases = dataServer_->selectAliasesWithTokenId(tid);
      token = t;
    } //else
      //warn(TR(T_ERROR_SUBMIT_TOKEN) + ": " + input);
  }

  if (!token.isValid() && cache_->isValid() && token.hasDigest()) {
    log(tr("searching for token in cache ..."));
    Token t = cache_->selectTokenWithDigest(token.digest(), token.digestType());
    if (t.isValid())
      token = t;
  }

  aliases = dataServer_->selectAliasesWithToken(token);

  dataManager_->setToken(token);
  dataManager_->setAliases(aliases);

  if (ALPHA)
    commentView_->setTokenId(token.id());
#ifdef USE_MODE_SIGNAL
  //signalView_->tokenView()->setAliases(aliases);
#endif // USE_MODE_SIGNAL

  AnnotationList annots = dataServer_->selectAnnotationsWithToken(token);
  //annotationBrowser_->setAnnotations(annots);
  //annotationView_->setAnnotations(annots);
  emit setAnnotationsRequested(annots);

  /*
  if (tokens.size() == 1) {
    Token token = tokens.front();
    if (!tokenView_->hasToken()) {
      tokenView_->setToken(token);
      server_->queryClusteredTokensyTokenId(token.id());
      return;
    }
  }

  tokenView_->setClusteredTokens(tokens);
  foreach (const Token &t, tokens)
    server_->queryAnnotationsByTokenId(t.id());
  */

  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");
  DOUT("exit");
}

void
MainWindow::showText(const QString &text)
{
  DOUT("enter: text =" << text);
  Annotation annot;
  if (server_->isAuthorized()) {
    annot.setUserId(server_->user().id());
    annot.setUserAlias(server_->user().name());
    annot.setUserAnonymous(server_->user().isAnonymous());
  }
  annot.setLanguage(Traits::AnyLanguage);
  annot.setText(text);
  annotationView_->showAnnotation(annot);
  DOUT("exit");
}

void
MainWindow::showTextAsSubtitle(const QString &input)
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
  if (server_->isAuthorized()) {
    annot.setUserId(server_->user().id());
    annot.setUserAlias(server_->user().name());
    annot.setUserAnonymous(server_->user().isAnonymous());
  }
  annot.setLanguage(Traits::AnyLanguage);
  QString sub = CORE_CMD_SUB " " + text;
  annot.setText(sub)    ;
  annotationView_->showAnnotation(annot);

  if (!rest.isEmpty())
    showTextAsSubtitle(rest);
}

void
MainWindow::submitText(const QString &text, bool async)
{
  DOUT("enter: async =" << async << ", text =" << text);
  if (!server_->isAuthorized() ||
      !dataManager_->token().hasDigest() ||
      hub_->isMediaTokenMode() && !player_->hasMedia()
#ifdef USE_MODE_SIGNAL
      || hub_->isSignalTokenMode() && !messageHandler_->lastMessageHash().isValid()
#endif // USE_MODE_SIGNAL
     ) {

    showText(text);
    DOUT("exit: returned from showText branch");
    return;
  }

  if (async) {
    log(tr("connecting server to submit annot ..."));
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
    annot.setTokenDigestType(dataManager_->token().digestType());
    annot.setUserId(server_->user().id());
    annot.setUserAlias(server_->user().name());
    annot.setUserAnonymous(server_->user().isAnonymous());
    annot.setLanguage(server_->user().language());
    annot.setCreateTime(QDateTime::currentMSecsSinceEpoch() / 1000);
    switch (hub_->tokenMode()) {
    case SignalHub::MediaTokenMode:
      annot.setPos(
        tokenType_ == Token::TT_Picture ?
          0 : player_->time()
      );
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
  qDebug()<<11111111;
  QString text = input.trimmed();
  if (text.isEmpty())
    return;

  QString dummy;
  QStringList tags;
  boost::tie(dummy, tags) = ANNOT_PARSE_CODE(text);

  foreach (const QString &tag, tags)
    switch (qHash(tag)) {
    case Core::H_Chat: chat(text); return;
    case Core::H_Play: play(); return;

    case Core::H_Quit:
    case Core::H_Exit:
    case Core::H_Close:
      close();
      return;
    }

  if (hub_->isSignalTokenMode() ||
      player_->hasMedia() && !player_->isStopped())
    submitText(text);
  else
    chat(text);
}

void
MainWindow::chat(const QString &input, bool async)
{
  DOUT("enter: async =" << async << ", text =" << input);
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

#ifdef USE_MODULE_DOLL
  Q_ASSERT(doll_);
  doll_->chat(text);
#else
  if (server_->isConnected())
    emit responded(server_->chat(text));
#endif // USE_MODULE_DOLL

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

  gConsole() << core_html_style(text, "color:" + color) + core_html_br();
  DOUT("exit");
}

// - Events -

bool
MainWindow::event(QEvent *e)
{
  bool accept = true;
  switch (e->type()) {
  case QEvent::FileOpen: { // See: http://www.qtcentre.org/wiki/index.php?title=Opening_documents_in_the_Mac_OS_X_Finder
      QFileOpenEvent *fe = dynamic_cast<QFileOpenEvent*>(e);
      Q_ASSERT(fe);
      if (fe) {
        QString url = fe->file();
        QTimer::singleShot(0, new MainWindow_slot_::OpenSource(url, this), SLOT(openSource()));
      }
    } break;

  default: accept = Base::event(e);
  }

  return accept;
}

/*
void
MainWindow::setWindowDwmEnabled(bool t)
{
  if (UiStyle::isAeroAvailable())
    UiStyle::globalInstance()->setWindowDwmEnabled(this, t);
}
*/

void
MainWindow::setVisible(bool visible)
{
  //if (visible == isVisible())
  //  return;
  if (!isMinimized() && !isFullScreen()) { // invoked by showNormal or showMaximized
#ifdef USE_WIN_DWM
    if (UiStyle::isAeroAvailable())
      UiStyle::globalInstance()->setWindowDwmEnabled(this, true);
      //QTimer::singleShot(0,
      //  new MainWindow_slot_::SetWindowDwmEnabled(true, this), SLOT(setWindowDwmEnabled())
      //);
    else
#endif // USE_WIN_DWM
    UiStyle::globalInstance()->setWindowBackground(this, false); // persistent is false
  }

  Base::setVisible(visible);
}

void
MainWindow::mousePressEvent(QMouseEvent *event)
{
  //DOUT("enter");
  //if (contextMenu_->isVisible())
  //  contextMenu_->hide();
#ifdef Q_WS_MAC
  if (hub_->isEmbeddedPlayerMode()) {
    // Prevent auto hide osd player.
    // Since NSView::mouseMoved is not avaible, use mouseDown to prevent hiding instead.
    if (!hasVisiblePlayer()) {
      embeddedPlayer_->show();
      //osdDock_->raise();
      osdWindow_->raise();
    } else if (embeddedPlayer_->isVisible())
      embeddedPlayer_->resetAutoHideTimeout();
      //QTimer::singleShot(0, embeddedPlayer_, SLOT(resetAutoHideTimeout()));

    // Alternative to windows hook:
    //if (isPlaying())
    //  pause();
  }
#endif // Q_WS_MAC

  if (event)
    switch (event->button()) {
    case Qt::LeftButton:
#ifdef Q_WS_MAC
      if (videoView_->isViewVisible())
        videoView_->setViewMousePressPos(event->globalPos());
#endif // Q_WS_MAC
      if (!isFullScreen() && dragPos_ == BAD_POS) {
        dragPos_ = event->globalPos() - frameGeometry().topLeft();
        event->accept();
      } break;

    case Qt::MiddleButton:
      hub_->toggleMiniPlayerMode();
      event->accept();
      break;

    case Qt::RightButton:
#ifndef Q_WS_MAC
      if (!contextMenu_->isVisible()) {
        QContextMenuEvent cm(QContextMenuEvent::Mouse, event->pos(), event->globalPos(), event->modifiers());
        QCoreApplication::sendEvent(this, &cm);
        event->accept();
      }
#endif // !Q_WS_MAC
      break;

    default: break;
    }

  Base::mousePressEvent(event);
  //DOUT("exit");
}

void
MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
  //DOUT("enter");
  dragPos_ = BAD_POS;

#ifdef Q_WS_MAC
  if (event && event->button() == Qt::LeftButton && // event->buttons() is always zero, qt4 bug?
      videoView_->isViewVisible())
    videoView_->setViewMouseReleasePos(event->globalPos());
#endif // Q_WS_MAC

  if (event)
    event->accept();

  Base::mouseReleaseEvent(event);
  //DOUT("exit");
}

bool
MainWindow::isGlobalPosNearEmbeddedPlayer(const QPoint &pos) const
{
  QRect r = annotationView_->globalRect();
  return pos.x() < 6 || pos.x() > osdWindow_->width() - 6 ||  // Near the borders
         pos.y() < 6 || pos.y() > osdWindow_->height() - 6 ||
  (embeddedPlayer_->isOnTop() ? ( // Near the top
         pos.x() < r.right() + 6 && pos.x() > r.left() - 6 &&
         pos.y() < r.bottom() - r.height() / 4 * 3 + 6 && pos.y() > r.top() - 6
  ) : ( // Near the bottom
         pos.x() < r.right() + 6 && pos.x() > r.left() - 6 &&
         pos.y() < r.bottom() + 6 && pos.y() > r.top() + r.height() / 4 * 3 - 6
  ));
}

void
MainWindow::mouseMoveEvent(QMouseEvent *event)
{
  //DOUT("enter");

  // Prevent auto hide osd player.
  if (hub_->isEmbeddedPlayerMode())
    if (!event || isGlobalPosNearEmbeddedPlayer(event->globalPos())) {
      if (!hasVisiblePlayer())
        embeddedPlayer_->show();
      else if (embeddedPlayer_->isVisible())
        embeddedPlayer_->resetAutoHideTimeout();
        //QTimer::singleShot(0, embeddedPlayer_, SLOT(resetAutoHideTimeout()));
    }

#ifdef Q_WS_MAC
   if (event && videoView_->isViewVisible())
     videoView_->setViewMouseMovePos(event->globalPos());
#endif // Q_WS_MAC

  // Move the main window
  if (event && event->buttons() & Qt::LeftButton &&
      dragPos_ != BAD_POS && !isFullScreen()) {
    QPoint globalPos = event->globalPos();
    // FIXME: not working
    //QRect globalRect(globalPos, size());
    //if (!isRectOutOfScreen(globalRect))
      move(globalPos - dragPos_);

    annotationView_->invalidatePos();
//#ifdef Q_WS_MAC // FIXME: solve the postion tracking problem....
//  QTimer::singleShot(200, annotationView_, SLOT(invalidatePos()));
//#endif // Q_WS_MAC

    event->accept();
  }

  Base::mouseMoveEvent(event);
  //DOUT("exit");
}

void
MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
  //DOUT("enter");
  if (event)
    switch (event->button()) {
    case Qt::LeftButton: hub_->toggleFullScreenWindowMode(); event->accept(); break;
    default: break;
    }

  Base::mouseDoubleClickEvent(event);
  //DOUT("exit");
}

void
MainWindow::wheelEvent(QWheelEvent *event)
{
  //DOUT("enter");
  if (event && event->orientation() == Qt::Vertical) {
    qreal vol = hub_->volume();
    vol += event->delta() / (8 * 360.0);
    vol = (vol > 1)? 1 : (vol < 0)? 0 : vol;
    hub_->setVolume(vol);
    event->accept();
  }

  Base::wheelEvent(event);
  //DOUT("exit");
}

void
MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
  //DOUT("enter");
  if (event) {
    invalidateContextMenu();
    contextMenu_->popup(event->globalPos());
    event->accept(); }

  //Base::contextMenuEvent(event);
  //DOUT("exit");
}

void
MainWindow::invalidateContextMenu()
{
  //DOUT("enter");
  contextMenu_->clear();
  if (!contextMenuActions_.isEmpty()) {
    foreach (QAction *a, contextMenuActions_)
      if (a)
        a->deleteLater();
    contextMenuActions_.clear();
  }

  // Open
  {
    //contextMenu_->addSeparator();

    //contextMenu_->addAction(openAct_);
    contextMenu_->addAction(openFileAct_);
    contextMenu_->addAction(openDeviceAct_);

#ifdef USE_WIN_PICKER
    toggleWindowPickDialogVisibleAct_->setChecked(windowPickDialog_->isVisible());
    contextMenu_->addAction(toggleWindowPickDialogVisibleAct_);
#endif // USE_WIN_PICKER

    openMenu_->clear(); {
      if (hub_->isMediaTokenMode() && player_->hasMedia()) {
        openMenu_->addAction(openSubtitleAct_);
        openMenu_->addSeparator();
      }
      openMenu_->addAction(openVideoDeviceAct_);
#ifdef Q_WS_WIN // TODO add support for Mac/Linux
      openMenu_->addAction(openAudioDeviceAct_);
#endif // Q_WS_WIN
      //openMenu_->addAction(openFileAct_);
    }
    contextMenu_->addMenu(openMenu_);

    if (browsedFiles_.size() > 1)
      contextMenu_->addMenu(browseMenu_);

    // Recent
    invalidateRecent();
    if (!recentFiles_.isEmpty())
      contextMenu_->addMenu(recentMenu_);

    // - Playlist
    //invalidatePlaylist();
    if (playlist_.size() > 1 && hub_->isMediaTokenMode())
      contextMenu_->addMenu(playlistMenu_);

    // Tracks
    if (hub_->isMediaTokenMode() && player_->hasMedia() &&
        player_->hasPlaylist())
      //invalidateTrackMenu();
      contextMenu_->addMenu(trackMenu_);

    // DVD sections
    if (hub_->isMediaTokenMode() && player_->hasMedia() &&
        player_->hasTitles()) {

      sectionMenu_->clear();

      if (player_->titleId() > 0)
        sectionMenu_->addAction(previousSectionAct_);
      if (player_->titleId() < player_->titleCount() - 1)
        sectionMenu_->addAction(nextSectionAct_);
      sectionMenu_->addSeparator();

      for (int tid = 0; tid < player_->titleCount(); tid++) {
        QString text = TR(T_SECTION) + " " + QString::number(tid);
        BOOST_AUTO(a, new Core::Gui::ActionWithId(tid, text, sectionMenu_));
        contextMenuActions_.append(a);
        if (tid == player_->titleId()) {
#ifdef Q_WS_X11
          a->setCheckable(true);
          a->setChecked(true);
#else
          a->setIcon(QIcon(RC_IMAGE_CURRENTSUBTITLE));
#endif // Q_WS_X11
        }
        connect(a, SIGNAL(triggeredWithId(int)), player_, SLOT(setTitleId(int)));
        sectionMenu_->addAction(a);
      }

      contextMenu_->addMenu(sectionMenu_);
    }

#ifdef USE_MODE_SIGNAL
    if (!(player_->hasMedia() && !player_->isStopped())) {
      contextMenu_->addSeparator();
#ifdef USE_WIN_PICKER
      toggleProcessPickDialogVisibleAct_->setChecked(processPickDialog_->isVisible());
      contextMenu_->addAction(toggleProcessPickDialogVisibleAct_);
#endif // USE_WIN_PICKER

       toggleSignalViewVisibleAct_->setChecked(signalView_->isVisible());
       contextMenu_->addAction(toggleSignalViewVisibleAct_ );

       if (hub_->isSignalTokenMode()) {
         toggleRecentMessageViewVisibleAct_->setChecked(recentMessageView_->isVisible());
         contextMenu_->addAction(toggleRecentMessageViewVisibleAct_ );
       }
     }
#endif // USE_MODE_SIGNAL

    // Subtitle menu
    if (hub_->isMediaTokenMode() && player_->hasMedia()) {
      contextMenu_->addSeparator();

      subtitleMenu_->clear();
      if (player_->hasSubtitles()) {
        if (player_->isSubtitleVisible()) {
          toggleSubtitleVisibleAct_->setChecked(true);
          toggleSubtitleVisibleAct_->setText(TR(T_MENUTEXT_SHOWSUBTITLE));
        } else {
          toggleSubtitleVisibleAct_->setChecked(false);
          toggleSubtitleVisibleAct_->setText(TR(T_MENUTEXT_HIDESUBTITLE));
        }
        contextMenu_->addAction(toggleSubtitleVisibleAct_);

        subtitleMenu_->addSeparator();

        QStringList l = player_->subtitleDescriptions();
        int id = 0;
        foreach (const QString &subtitle, l) {
          BOOST_AUTO(a, new Core::Gui::ActionWithId(id, subtitle, subtitleMenu_));
          contextMenuActions_.append(a);
          if (id == player_->subtitleId()) {
#ifdef Q_WS_X11
            a->setCheckable(true);
            a->setChecked(true);
#else
            a->setIcon(QIcon(RC_IMAGE_CURRENTSUBTITLE));
#endif // Q_WS_X11
          }
          connect(a, SIGNAL(triggeredWithId(int)), player_, SLOT(setSubtitleId(int)));
          subtitleMenu_->addAction(a);
          id++;
        }

        subtitleMenu_->addSeparator();
      }
      subtitleMenu_->addAction(openSubtitleAct_);
      contextMenu_->addMenu(subtitleMenu_);
    }
  }

  if (hub_->isSignalTokenMode()
      || hub_->isMediaTokenMode() && player_->hasMedia()) {
    // Basic control
    contextMenu_->addSeparator();

    if (hub_->isSignalTokenMode() && !hub_->isPlaying() ||
        hub_->isMediaTokenMode() && !player_->isPlaying())
      contextMenu_->addAction(playAct_);

    if (hub_->isSignalTokenMode() && hub_->isPlaying() ||
        hub_->isMediaTokenMode() && player_->isPlaying())
      contextMenu_->addAction(pauseAct_);

    // jichi 11/18/2011: Removed from context menu only in order to save space ....
    //if (hub_->isMediaTokenMode())
    //  contextMenu_->addAction(nextFrameAct_);

    if (hub_->isSignalTokenMode() && !hub_->isStopped() ||
        hub_->isMediaTokenMode() && !player_->isStopped())
    contextMenu_->addAction(stopAct_);

    if (hub_->isMediaTokenMode()) {
      contextMenu_->addAction(replayAct_);

      contextMenu_->addAction(previousAct_);

      contextMenu_->addAction(nextAct_);

      contextMenu_->addAction(toggleAutoPlayNextAct_);
    }

    if (hub_->isMediaTokenMode() && !player_->isStopped() ||
        hub_->isSignalTokenMode())
      contextMenu_->addAction(snapshotAct_);
  }

  // Forward/backward
  if (hub_->isMediaTokenMode() && player_->hasMedia()) {
    contextMenu_->addSeparator();

    contextMenu_->addMenu(backwardMenu_);
    contextMenu_->addMenu(forwardMenu_);

    toggleSeekDialogVisibleAct_->setChecked(seekDialog_->isVisible());
    contextMenu_->addAction(toggleSeekDialogVisibleAct_);
  }

  // Annotation
  {
    contextMenu_->addSeparator();
    if (annotationView_->isPlaybackEnabled()) {
      toggleAnnotationVisibleAct_->setChecked(true);
      toggleAnnotationVisibleAct_->setText(TR(T_MENUTEXT_SHOWANNOT));
    } else {
      toggleAnnotationVisibleAct_->setChecked(false);
      toggleAnnotationVisibleAct_->setText(TR(T_MENUTEXT_HIDEANNOT));
    }
    contextMenu_->addAction(toggleAnnotationVisibleAct_ );

    toggleBlacklistViewVisibleAct_->setChecked(blacklistView_->isVisible());
    contextMenu_->addAction(toggleBlacklistViewVisibleAct_);

    if (annotationFilter_->isEnabled())
      contextMenu_->addMenu(annotationLanguageMenu_);

    // Subtitle menu
    if (hub_->isSignalTokenMode() &&
        server_->isConnected() && server_->isAuthorized()) {
      contextMenu_->addSeparator();
      contextMenu_->addAction(toggleTranslateAct_);
    }
    toggleSubtitleStaysOnTopAct_->setChecked(isSubtitleStaysOnTop());
    contextMenu_->addAction(toggleSubtitleStaysOnTopAct_);

    contextMenu_->addMenu(subtitleStyleMenu_);

    toggleSubtitleStaysOnTopAct_->setChecked(isSubtitleStaysOnTop());
    contextMenu_->addAction(toggleSubtitleStaysOnTopAct_);
  }

  if (player_->hasMedia()
#ifdef USE_MODE_SIGNAL
      && hub_->isMediaTokenMode()
#endif // USE_MODE_SIGNAL
      ) {
    // Sync mode
    if (ALPHA) {
      contextMenu_->addSeparator();

      toggleSyncModeAct_->setChecked(hub_->isSyncPlayMode());
      contextMenu_->addAction(toggleSyncModeAct_);

      toggleSyncDialogVisibleAct_->setChecked(syncDialog_->isVisible());
      contextMenu_->addAction(toggleSyncDialogVisibleAct_);
    }
    /*
    { // Live mode
      contextMenu_->addSeparator();

      toggleLiveModeAct_->setChecked(isLiveMode());
      contextMenu_->addAction(toggleLiveModeAct_);

      toggleLiveDialogVisibleAct_->setChecked(liveDialog_->isVisible());
      contextMenu_->addAction(toggleLiveDialogVisibleAct_);
    }
    */
  }

  // Toggles
  {
    contextMenu_->addSeparator();

    toggleFullScreenModeAct_->setChecked(hub_->isFullScreenWindowMode());
    contextMenu_->addAction(toggleFullScreenModeAct_);

    toggleMiniModeAct_->setChecked(hub_->isMiniPlayerMode());
    contextMenu_->addAction(toggleMiniModeAct_);

    toggleEmbeddedModeAct_->setChecked(hub_->isEmbeddedPlayerMode());
    contextMenu_->addAction(toggleEmbeddedModeAct_);
    if (hub_->isEmbeddedPlayerMode()) {
      toggleEmbeddedPlayerOnTopAct_->setChecked(embeddedPlayer_->isOnTop());
      contextMenu_->addAction(toggleEmbeddedPlayerOnTopAct_);
    }

    toggleAnnotationEditorVisibleAct_->setChecked(annotationEditor_->isVisible());
    contextMenu_->addAction(toggleAnnotationEditorVisibleAct_ );

    if (hub_->isSignalTokenMode() || player_->hasMedia()) {
      toggleAnnotationBrowserVisibleAct_->setChecked(annotationBrowser_->isVisible());
      contextMenu_->addAction(toggleAnnotationBrowserVisibleAct_ );

      toggleTokenViewVisibleAct_->setChecked(tokenView_->isVisible());
      contextMenu_->addAction(toggleTokenViewVisibleAct_ );
    }

    if (ALPHA) if (commentView_->tokenId()) {
      toggleCommentViewVisibleAct_->setChecked(commentView_->isVisible());
      contextMenu_->addAction(toggleCommentViewVisibleAct_ );
    }

    toggleCloudViewVisibleAct_->setChecked(cloudView_->isVisible());
    contextMenu_->addAction(toggleCloudViewVisibleAct_ );
  }

  // User
  if (!server_->isAuthorized()) {
    contextMenu_->addSeparator();
    contextMenu_->addAction(loginAct_);
  }

  // Toggles
  {
    //if (!isFullScreenMode()) {
    //  contextMenu_->addSeparator();
    //  contextMenu_->addAction(toggleWindowStaysOnTopAct_);
    //}
  }

  // App
  {
    contextMenu_->addSeparator();

    // Theme
    if (themeMenu_) {
      contextMenu_->addMenu(themeMenu_);

      UiStyle::Theme t = UiStyle::globalInstance()->theme();
      setThemeToDefaultAct_->setChecked(t == UiStyle::DefaultTheme);
      setThemeToRandomAct_->setChecked(t == UiStyle::RandomTheme);
      setThemeToBlack1Act_->setChecked(t == UiStyle::Black1Theme);
      setThemeToBlack2Act_->setChecked(t == UiStyle::Black2Theme);
      setThemeToBlue1Act_->setChecked(t == UiStyle::Blue1Theme);
      setThemeToBlue2Act_->setChecked(t == UiStyle::Blue2Theme);
      setThemeToBrown1Act_->setChecked(t == UiStyle::Brown1Theme);
      setThemeToBrown2Act_->setChecked(t == UiStyle::Brown2Theme);
      setThemeToGreen1Act_->setChecked(t == UiStyle::Green1Theme);
      setThemeToGreen2Act_->setChecked(t == UiStyle::Green2Theme);
      setThemeToLightBlue1Act_->setChecked(t == UiStyle::LightBlue1Theme);
      setThemeToLightBlue2Act_->setChecked(t == UiStyle::LightBlue2Theme);
      setThemeToOrange1Act_->setChecked(t == UiStyle::Orange1Theme);
      setThemeToOrange2Act_->setChecked(t == UiStyle::Orange2Theme);
      setThemeToPink1Act_->setChecked(t == UiStyle::Pink1Theme);
      setThemeToPink2Act_->setChecked(t == UiStyle::Pink2Theme);
      setThemeToPurple1Act_->setChecked(t == UiStyle::Purple1Theme);
      setThemeToPurple2Act_->setChecked(t == UiStyle::Purple2Theme);
      setThemeToRed1Act_->setChecked(t == UiStyle::Red1Theme);
      setThemeToRed2Act_->setChecked(t == UiStyle::Red2Theme);
      setThemeToYellow1Act_->setChecked(t == UiStyle::Yellow1Theme);
      setThemeToYellow2Act_->setChecked(t == UiStyle::Yellow2Theme);
    }

    // Language
    {
      contextMenu_->addMenu(appLanguageMenu_);

      //int l = TranslatorManager::globalInstance()->language();
      int l = Settings::globalInstance()->language();
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

    contextMenu_->addAction(showMinimizedAct_);

    toggleMenuBarVisibleAct_->setChecked(menuBar()->isVisible());
    contextMenu_->addAction(toggleMenuBarVisibleAct_);

    contextMenu_->addMenu(helpContextMenu_);
    contextMenu_->addAction(quitAct_);
  }
  //DOUT("exit");
}

void
MainWindow::invalidateUserMenu()
{
  //userMenu_->clear();
  if (!server_->isAuthorized())
    return;

  // Menu

  if (userMenu_->isEmpty()) {
    userMenu_->addAction(toggleUserAnonymousAct_);
    userMenu_->addMenu(userLanguageMenu_);

    userMenu_->addSeparator();
    userMenu_->addAction(logoutAct_);
  }

  //if (ALPHA) {
  //  //toggleUserPanelVisibleAct_->setChecked(loginDialog_->isVisible());
  //  //userMenu_->addAction(toggleUserPanelVisibleAct_);
//
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
MainWindow::invalidateTrackMenu()
{
  trackMenu_->clear();
  if (!player_->hasPlaylist())
    return;

  foreach (Player::MediaInfo mi, player_->playlist()) {
    QString text = QString("%1. %2").arg(QString::number(mi.track + 1)).arg(mi.title);
    BOOST_AUTO(a, new Core::Gui::ActionWithId(mi.track, text, trackMenu_));
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
  Q_UNUSED(event);
  if (annotationView_->isVisible()) {
    annotationView_->invalidatePos();
//#ifdef Q_WS_MAC // FIXME: solve the postion tracking problem....
//    QTimer::singleShot(200, annotationView_, SLOT(invalidatePos()));
//#endif // Q_WS_MAC
  }

  Base::moveEvent(event);
}

void
MainWindow::resizeEvent(QResizeEvent *event)
{
  Q_UNUSED(event);
  //if (isMaximized()) {
  //  setFullScreenPlayMode();
  //  return;
  //}
  if (annotationView_->isVisible()) {
    annotationView_->invalidateSize();
    globalOsdConsole_->resize(annotationView_->size());
  }

  Base::resizeEvent(event);
}

void
MainWindow::changeEvent(QEvent *event)
{
  Q_ASSERT(event);
  Base::changeEvent(event);

  switch (event->type()) {
  case QEvent::WindowStateChange:
#ifdef Q_WS_X11
    if (windowState() == Qt::WindowFullScreen)
      QtX::setWindowInputShape(osdWindow_->winId(), embeddedPlayer_->pos(), embeddedPlayer_->rect());
    else
      QtX::zeroWindowInputShape(osdWindow_->winId());
#endif // Q_WS_X11
    break;

  default: break;
  }
}

// - Keyboard events -

void
MainWindow::keyPressEvent(QKeyEvent *event)
{
  switch (event->key()) {

    //if (!hasVisiblePlayer() || !visiblePlayer()->lineEdit()->hasFocus())
  case Qt::Key_Escape: hub_->toggleMiniPlayerMode(); break;
  case Qt::Key_Return: hub_->toggleFullScreenWindowMode(); break;

  case Qt::Key_Up:      volumeUp(); break;
  case Qt::Key_Down:    volumeDown(); break;

  case Qt::Key_Left:
    if (event->modifiers() == Qt::NoModifier) backward();
    else backward(G_BACKWARD_INTERVAL * 3);
    break;

  case Qt::Key_Right:
    if (event->modifiers() == Qt::NoModifier) forward();
    else forward(G_FORWARD_INTERVAL * 3);
    break;

  case Qt::Key_Space: playPause(); break;

  default:
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
    Base::keyPressEvent(event);
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
MainWindow::closeEvent(QCloseEvent *event)
{
  DOUT("enter");
  //respond(tr("yin: ...")); // crash on mac
  log("closing application ...");

  //hide();
  //if (parentWidget())
  //  parentWidget()->hide();

#ifdef USE_MODE_SIGNAL
  QTH->clear();
#endif // USE_MODE_SIGNAL

  if (tray_)
    tray_->hide();

  // Save settings
  Settings *settings = Settings::globalInstance();

  //invalidateRecent();
  settings->setRecentFiles(recentFiles_);

  settings->setRecentPath(recentPath_);

  settings->setThemeId(UiStyle::globalInstance()->theme());

  settings->setThemeId(UiStyle::globalInstance()->theme());

  settings->setMenuBarVisible(menuBar()->isVisible());

  settings->setAnnotationFilterEnabled(annotationFilter_->isEnabled());

  settings->setSubtitleColor(subtitleColor());

  // Disabled for saving closing time orz
  //if (server_->isConnected() && server_->isAuthorized())
  //  server_->logout();

  settings->setTranslateEnabled(isTranslateEnabled());

  settings->setEmbeddedPlayerStaysOnTop(embeddedPlayer_->isOnTop());

  settings->setAutoPlayNext(isAutoPlayNext());

  // Wait for thread pool
  if (QThreadPool::globalInstance()->activeThreadCount()) {
#ifdef USE_WIN_PICKER
    if (PICKER->isActive())
      PICKER->stop();
#endif // USE_WIN_PICKER
#ifdef USE_WIN_HOOK
    if (HOOK->isActive())
      HOOK->stop();
#endif // USE_WIN_HOOK
    if (!hub_->isStopped()) {
      if (hub_->isMediaTokenMode())
        player_->setVolume(0);
      hub_->stop();
    }
    hide();
    osdWindow_->hide();
    //osdDock_->hide();
    annotationView_->hide();
    globalOsdConsole_->hide();
    //if (parentWidget())
    //  parentWidget()->hide();

    // FIXME: use waitForDone with timeout in Qt 4.8
    QThreadPool::globalInstance()->waitForDone();
    //QThreadPool::globalInstance()->waitForDone(5000);
    //DOUT("WARNING: killing active threads; will be fixed in Qt 4.8");
  }

  //if (parentWidget())
  //  QTimer::singleShot(0, parentWidget(), SLOT(close()));

  QTimer::singleShot(0, qApp, SLOT(quit())); // ensure quit app and clean up zombie threads
  Base::closeEvent(event);

  emit windowClosed();
  DOUT("exit");
}

// - Stays on top -

//bool
//MainWindow::isWindowStaysOnTop() const
//{ return windowFlags() & Qt::WindowStaysOnTopHint; }

/*
void
MainWindow::setWindowStaysOnTop(bool enabled)
{
  say(": " + TR(T_MESSAGE_ENABLE_WINDOWSTAYSONTOP));
  if (isWindowStaysOnTop() != enabled) {
    bool visible = isVisible();
    if (visible)
      hide();
#ifdef USE_WIN_DWM
    bool aeroEnabled = isAeroStyleEnabled();
    if (aeroEnabled)
      setAeroStyleEnabled(false);

    foreach (QWidget *w, dialogs_)
      DWM_DISABLE_AERO_WIDGET(w);

#endif // USE_WIN_DWM
    if (enabled)
      setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    else
      setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);

#ifdef USE_WIN_DWM
    if (aeroEnabled)
      setAeroStyleEnabled(true);

    foreach (QWidget *w, dialogs_)
      DWM_ENABLE_AERO_WIDGET(w);
#endif // USE_WIN_DWM

    // FIXME: correctly raise OsdWindow!!!

    if (visible)
      show();
  }
}
*/

// - Synchronize -

void
MainWindow::blessTokenWithId(qint64 id, bool async)
{
  DOUT("enter: async =" << async);
  if (!id) {
    warn("invalid cast id");
    DOUT("exit from invalid parameter branch");
    return;
  }

  if (!server_->isConnected() || !server_->isAuthorized()) {
    warn("cannot perform cast when offline");
    DOUT("exit from offline branch");
    return;
  }

  if (async) {
    log(tr("submit bless cast to token ..."));
    QThreadPool::globalInstance()->start(new task_::blessTokenWithId(id, this));
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  bool ok = server_->blessTokenWithId(id);
  if (ok) {
    log("token blessed");
    if (dataManager_->token().id() == id) {
      dataManager_->token().blessedCount()++;
      //dataManager_->invalidateToken();
    }
  } else
    warn("failed to bless token");

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
    warn("invalid cast id");
    DOUT("exit from invalid parameter branch");
    return;
  }

  if (!server_->isConnected() || !server_->isAuthorized()) {
    warn("cannot perform cast when offline");
    DOUT("exit from offline branch");
    return;
  }

  if (async) {
    log(tr("submit curse cast to token ..."));
    QThreadPool::globalInstance()->start(new task_::curseTokenWithId(id, this));
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  bool ok = server_->curseTokenWithId(id);
  if (ok) {
    log("token curseed");
    if (dataManager_->token().id() == id) {
      dataManager_->token().cursedCount()++;
      //dataManager_->invalidateToken();
    }
  } else
    warn("failed to curse token");

  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");
  DOUT("exit: async =" << async);
}

void
MainWindow::blessAnnotationWithId(qint64 tid, bool async)
{
  Q_UNUSED(tid);
  Q_UNUSED(async);
  // CHECKPOINT
}
void
MainWindow::curseAnnotationWithId(qint64 tid, bool async)
{
  Q_UNUSED(tid);
  Q_UNUSED(async);
  // CHECKPOINT
}
void
MainWindow::blockAnnotationWithId(qint64 tid, bool async)
{
  Q_UNUSED(tid);
  Q_UNUSED(async);
  // CHECKPOINT
}

void
MainWindow::blessAliasWithId(qint64 tid, bool async)
{
  Q_UNUSED(tid);
  Q_UNUSED(async);
  // CHECKPOINT
}
void
MainWindow::curseAliasWithId(qint64 tid, bool async)
{
  Q_UNUSED(tid);
  Q_UNUSED(async);
  // CHECKPOINT
}
void
MainWindow::blockAliasWithId(qint64 tid, bool async)
{
  Q_UNUSED(tid);
  Q_UNUSED(async);
  // CHECKPOINT
}

void
MainWindow::logout(bool async)
{
  DOUT("enter: async =" << async);
  if (async) {
    log(tr("connecting server to logout ..."));
    QThreadPool::globalInstance()->start(new task_::logout(this));
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");

  annotationBrowser_->setUserId(0);
  server_->logout();

  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");

  Settings *s = Settings::globalInstance();
  //s->setUserName(QString());
  s->setPassword(QString());

  DOUT("exit: async =" << async);
}

void
MainWindow::login(const QString &userName, const QString &encryptedPassword, bool async)
{
  DOUT("enter: async =" << async << ", userName =" << userName);
  if (async) {
    QThreadPool::globalInstance()->start(new task_::login(userName, encryptedPassword, this));
    DOUT("exit: returned from async branch");
    return;
  }

  DOUT("inetMutex locking");
  inetMutex_.lock();
  DOUT("inetMutex locked");
  // CHECKPOINT: Multithreading here, break this function into two parts
  server_->updateConnected();
  if (!server_->isConnected() && cache_->isValid()) {
    // login from cache
    log(TR(T_MESSAGE_TRY_LOGINFROMCACHE));
    User user = cache_->selectUserWithNameAndPassword(userName, encryptedPassword);
    if (user.isValid()) {
      server_->setAuthorized(true);
      server_->setUser(user);
      annotationBrowser_->setUserId(user.id());
      log(TR(T_SUCCEED_LOGINFROMCACHE));
    } else
      warn(TR(T_ERROR_LOGINFROMCACHE_FAILURE));

    DOUT("inetMutex unlocking");
    inetMutex_.unlock();
    DOUT("inetMutex unlocked");
    DOUT("exit from offline branch");
    return;
  }

  Settings *settings = Settings::globalInstance();
  settings->setUserName(userName);
  settings->setPassword(encryptedPassword);

  server_->login(userName, encryptedPassword);

  if (server_->isConnected() && server_->isAuthorized()) {

    //if (!server_->user().hasLanguage())
    //  server_->setUserLanguage(Traits::UnknownLanguage);

    qint64 uid = server_->user().id();
    settings->setUserId(uid);
    //tokenView_->setUserId(uid);
    annotationView_->setUserId(uid);
    annotationBrowser_->setUserId(uid);

    if (cache_->isValid()) {
      cache_->deleteUserWithId(uid);
      cache_->insertUser(server_->user());
    }

    // Languages
    switch (server_->user().language()) {
    case Traits::Japanese:
      toggleAnnotationLanguageToJapaneseAct_->setChecked(true);
      break;
    case Traits::Chinese:
      toggleAnnotationLanguageToChineseAct_->setChecked(true);
      break;
    case Traits::Korean:
      toggleAnnotationLanguageToKoreanAct_->setChecked(true);
      break;
    case Traits::UnknownLanguage:
      toggleAnnotationLanguageToUnknownAct_->setChecked(true);
      break;
    case Traits::English:
    case Traits::AnyLanguage:
    default:
      toggleAnnotationLanguageToEnglishAct_->setChecked(true);
    }
    invalidateAnnotationLanguages();



    QDate today = QDate::currentDate();
    if (Settings::globalInstance()->updateDate() != today) {
      bool updated = server_->isSoftwareUpdated();
      if (!updated)
        warn(tr("new version released, check here: ") + "\n" + G_UPDATEPAGE_URL);

      Settings::globalInstance()->setUpdateDate(today);
    }

    dataServer_->commitQueue();
  }

  DOUT("inetMutex unlocking");
  inetMutex_.unlock();
  DOUT("inetMutex unlocked");
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
      log(tr("Sync mode started"));
    else
      log(tr("Sync mode stopped"));
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
      log(tr("Live mode started"));
    else
      log(tr("Live mode stopped"));
  }
}
*/

#ifdef USE_WIN_QTH

// - Signal mode -

void
MainWindow::openProcessWindow(WId hwnd)
{
  ulong pid = QtWin::getWindowProcessId(hwnd);
  if (pid) {
    log(tr("found process id for window") + QString(" (pid = %1)").arg(QString::number(pid)));
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

  if (QTH->isProcessAttached(pid))
    log(tr("process was attached") + QString(" (pid = %1)").arg(pid));
  else {
    bool ok = QTH->attachProcess(pid);
    if (ok) {
      log(tr("process attached") + QString(" (pid = %1)").arg(QString::number(pid)));
      signalView_->processView()->refresh();
    } else {
      error(tr("failed to attach process ") + QString(" (pid = %1)").arg(QString::number(pid)));
      if (!QtWin::isProcessActiveWithId(pid))
        notify(tr("Is the process running now?"));
      else if (!QTH->isElevated())
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
  ulong pid = QTH->getProcessIdByName(processName);
  if (pid) {
    log(tr("process was started") + QString(" (pid = %1)").arg(pid));
  } else {
    QtWin::createProcessWithExecutablePath(path);
    log(tr("told process to start") + QString(" (name = %1)").arg(processName));
    pid = QTH->getProcessIdByName(processName);
  }

  addRecent(path);

  signalView_->processView()->refresh();
  if (!pid) {
    warn(tr("failed to start process") + QString(" (name = %1)").arg(processName));
    openProcess();
  } else {
    enum { seconds = G_OPENPROCESS_TIMEOUT / 1000 }; // msecs / 1000
    log(tr("wait %1 seconds for process to start ...").arg(QString::number(seconds)));
    auto *slot = new MainWindow_slot_::OpenProcessId(pid, this);
    QTimer::singleShot(G_OPENPROCESS_TIMEOUT, slot, SLOT(openProcessId()));
  }
}

void
MainWindow::openProcessHook(int hookId, const ProcessInfo &pi)
{
  if (player_->hasMedia() && !player_->isStopped())
    stop();

  if (pi.isValid()) {
    log(tr("openning process") + ": " + pi.processName);

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
  }

  hub_->setSignalTokenMode();

  dataManager_->token().setId(0);
  dataManager_->removeAliases();
  annotationView_->removeAnnotations();

  if (pi.isValid()) {
    recentDigest_ = digestFromFile(pi.executablePath);
    if (!recentDigest_.isEmpty())
      setToken(pi.executablePath);
  }

  hub_->play();
}

#endif // USE_WIN_QTH

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
  if (visible)
    signalView_->raise();
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
    recentMessageView_->addMessages(::revertList(messageHandler_->recentMessages()), messageHandler_->hookId());
    recentMessageView_->setCurrentIndex(1);
  }
  recentMessageView_->setVisible(visible);
  if (visible)
    recentMessageView_->raise();
}

#endif // USE_MODE_SIGNAL

// - User -

void
MainWindow::setUserAnonymous(bool t, bool async)
{
  DOUT("enter: async =" << async);

  if (server_->isConnected() && server_->isAuthorized()) {

    if (async) {
      log(tr("connecting server to change anonymous status ..."));
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
    log(tr("you are anonymous now"));
  else
    log(tr("you are not anonymous now"));

  DOUT("exit");
}

void
MainWindow::setUserLanguage(int lang, bool async)
{
  DOUT("enter: async =" << async);

  if (server_->isConnected() && server_->isAuthorized()) {

    if (async) {
      log(tr("connecting server to change language ..."));
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

  log(tr("your language is ") + languageToString(server_->user().language()));

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
  Settings::globalInstance()->setAnnotationLanguages(languages);
}

// - Language -

void
MainWindow::setAppLanguageToEnglish()
{ Settings::globalInstance()->setLanguage(QLocale::English); invalidateAppLanguage(); }

void
MainWindow::setAppLanguageToJapanese()
{ Settings::globalInstance()->setLanguage(QLocale::Japanese); invalidateAppLanguage(); }

// TODO: Simplified Chinese and Traditional Chinese are not differed until qt 4.8
void
MainWindow::setAppLanguageToTraditionalChinese()
{ Settings::globalInstance()->setLanguage(TranslatorManager::TraditionalChinese); invalidateAppLanguage(); }

void
MainWindow::setAppLanguageToSimplifiedChinese()
{ Settings::globalInstance()->setLanguage(QLocale::Chinese); invalidateAppLanguage(); }

void
MainWindow::invalidateAppLanguage()
{ warn(tr("restart the app to use the new language")); }

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
  invalidatePlaylistMenu();
}

void
MainWindow::invalidatePlaylist()
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
    invalidatePlaylistMenu();
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
MainWindow::invalidatePlaylistMenu()
{
  QList<QAction*> l = playlistMenu_->actions();
  foreach (QAction *a, l)
    if (a && a != clearPlaylistAct_)
      a->deleteLater();
  playlistMenu_->clear();

  if (!playlist_.isEmpty()) {
    int i = playlist_.indexOf(recentOpenedFile_);
    playlistMenu_->addAction(clearPlaylistAct_);
    if (i > 0)
      playlistMenu_->addAction(previousPlaylistItemAct_);
    if (i < playlist_.size() - 1)
      playlistMenu_->addAction(nextPlaylistItemAct_);
    playlistMenu_->addSeparator();

    foreach (QString f, playlist_) {
      QString path = removeUrlPrefix(f);
      QString text = QFileInfo(path).fileName();
      if (text.isEmpty()) {
        text = QDir(path).dirName();
        if (text.isEmpty())
          text = f;
      }
      BOOST_AUTO(a, new Core::Gui::ActionWithId(i++, text, playlistMenu_));
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
MainWindow::invalidateRecent()
{
  if (recentFiles_.isEmpty())
    return;
  bool update = false;

  BOOST_AUTO(p, recentFiles_.begin());
  while (p != recentFiles_.end()) {
    QFileInfo fi(removeUrlPrefix(*p));
    if (fi.exists())
      ++p;
    else {
      p = recentFiles_.erase(p);
      update = true;
    }
  }

  // FIXME
  if (!recentFiles_.isEmpty()) {
     QStringList unique = ::uniqueList(recentFiles_);
     if (unique.size() != recentFiles_.size()) {
       recentFiles_ = unique;
       update = true;
     }
  }

  if (update)
    invalidateRecentMenu();
}

void
MainWindow::invalidateRecentMenu()
{
  QList<QAction*> l = recentMenu_->actions();
  foreach (QAction *a, l)
    if (a && a != clearRecentAct_)
      a->deleteLater();
  recentMenu_->clear();

  if (!recentFiles_.isEmpty()) {
    int i = 0;
    foreach (QString f, recentFiles_) {
      if (i >= G_RECENT_COUNT)
        break;

      QString path = removeUrlPrefix(f);
      QString text = QFileInfo(path).fileName();
      if (text.isEmpty()) {
        text = QDir(path).dirName();
        if (text.isEmpty())
          text = f;
      }
      BOOST_AUTO(a, new Core::Gui::ActionWithId(i++, text, recentMenu_));
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
  invalidateRecentMenu();
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
  if (path.isEmpty())
    return;

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
  openPath(path, true);
}

void
MainWindow::addRecent(const QString &path)
{
  //if (recentFiles_.indexOf(path) >= 0)
  if (!recentFiles_.isEmpty())
    recentFiles_.removeAll(path);
  recentFiles_.prepend(path);

  // SingleShot to avoid multi-threading issue.
  QTimer::singleShot(0, this, SLOT(invalidateRecentMenu()));
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
  if (!isTranslateEnabled() || text.isEmpty())
    return;

  if (!server_->isConnected() || !server_->isAuthorized())
    return;

  int l = server_->user().language();
  QString lcode;
  switch (l) {
  case Traits::English:  lcode = Translator::English; break;
  case Traits::Japanese: lcode = Translator::Japanese; break;
  case Traits::Chinese:  lcode = Translator::SimplifiedChinese; break;
  case Traits::Korean:   lcode = Translator::Korean; break;
  default: return;
  }

  translator_->translate(text, lcode);
}

// - Subtitle -

void
MainWindow::setSubtitleStaysOnTop(bool t)
{
  annotationView_->setSubtitlePosition(
        t ? AnnotationGraphicsView::AP_Top
          : AnnotationGraphicsView::AP_Bottom);

  Settings::globalInstance()->setSubtitleStaysOnTop(t);
}

bool
MainWindow::isSubtitleStaysOnTop() const
{ return annotationView_->subtitlePosition() == AnnotationGraphicsView::AP_Top; }

void
MainWindow::uncheckSubtitleColorActions()
{
  setSubtitleColorToDefaultAct_->setChecked(false);
  setSubtitleColorToWhiteAct_->setChecked(false);
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
    UiStyle::globalInstance()->setTheme(UiStyle::_theme##Theme); \
\
    if (hub_->isMediaTokenMode() && hub_->isEmbeddedPlayerMode() && \
        player_->hasMedia() && \
        !UiStyle::isAeroAvailable()) \
      UiStyle::globalInstance()->setBlackBackground(this); \
  }

  SET_THEME_TO(Default) SET_THEME_TO(Random)
  SET_THEME_TO(Black1)  SET_THEME_TO(Black2)
  SET_THEME_TO(Blue1)   SET_THEME_TO(Blue2)
  SET_THEME_TO(Brown1)  SET_THEME_TO(Brown2)
  SET_THEME_TO(Green1)  SET_THEME_TO(Green2)
  SET_THEME_TO(LightBlue1) SET_THEME_TO(LightBlue2)
  SET_THEME_TO(Orange1) SET_THEME_TO(Orange2)
  SET_THEME_TO(Pink1)   SET_THEME_TO(Pink2)
  SET_THEME_TO(Purple1) SET_THEME_TO(Purple2)
  SET_THEME_TO(Red1)    SET_THEME_TO(Red2)
  SET_THEME_TO(Yellow1) SET_THEME_TO(Yellow2)
#undef SET_THEME_TO

void
MainWindow::enableWindowTransparency()
{ setWindowOpacity(G_WINDOW_OPACITY); }

void
MainWindow::disableWindowTransparency()
{ setWindowOpacity(1.0); }

// - Browse -

void
MainWindow::setBrowsedFile(const QString &filePath)
{
  browsedFiles_.clear();
  browseMenu_->clear();

  QFileInfo fi(filePath);
  QDir dir = fi.dir();

  if (!dir.exists())
    return;

  int tt = fileType(filePath);
  QStringList nf;
  switch (tt) {
  case Token::TT_Video:         nf = Player::supportedVideoFilters(); break;
  case Token::TT_Audio:         nf = Player::supportedAudioFilters(); break;
  case Token::TT_Picture:       nf = Player::supportedPictureFilters(); break;
  }
  if (nf.isEmpty())
    return;

  browsedFiles_ = dir.entryInfoList(nf, QDir::Files);
  if (browsedFiles_.size() <= 1)
    return;

  if (browsedFiles_.size() > 10) {
    browseMenu_->addAction(previousFileAct_);
    browseMenu_->addAction(nextFileAct_);
    browseMenu_->addSeparator();
  }

  int id = 0;
  foreach (QFileInfo f, browsedFiles_) {
    BOOST_AUTO(a, new Core::Gui::ActionWithId(id++, f.fileName(), browseMenu_));
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
MainWindow::openBrowsedFile(int id)
{
  if (id < 0)
    id = browsedFiles_.size() - 1;
  else if (id >= browsedFiles_.size())
    id = 0;

  if (id >= 0 && id < browsedFiles_.size())
    openPath(browsedFiles_[id].absoluteFilePath());
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
  foreach (QFileInfo f, browsedFiles_) {
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
MainWindow::previous()
{
  if (hub_->isMediaTokenMode() && player_->hasMedia()) {
    if (player_->hasPlaylist())
      player_->previousTrack();
    //else if (player_->hasChapters())
    //  player_->setPreviousChapter();
    else if (player_->hasTitles())
      player_->setPreviousTitle();
    else if (playlist_.size() > 1)
      openPreviousPlaylistItem();
    else if (browsedFiles_.size() > 1)
      openPreviousFile();
  }
}

void
MainWindow::next()
{
  if (hub_->isMediaTokenMode() && player_->hasMedia()) {
    if (player_->hasPlaylist())
      player_->nextTrack();
    //else if (player_->hasChapters())
    //  player_->setNextChapter();
    else if (player_->hasTitles())
      player_->setNextTitle();
    else if (playlist_.size() > 1)
      openNextPlaylistItem();
    else if (browsedFiles_.size() > 1)
      openNextFile();
  }
}

void
MainWindow::autoPlayNext()
{
  if (isAutoPlayNext())
    next();
}

// - Tracking window -

void
MainWindow::setEmbeddedWindow(WId winId)
{
  if (!winId)
    player_->embed(videoView_);
  else {
#ifndef Q_WS_MAC
    player_->embed(0);
    player_->setEmbeddedWindow(winId);
#endif // !Q_WS_MAC
  }
}

// - Error handling -

void
MainWindow::handlePlayerError()
{ }

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

