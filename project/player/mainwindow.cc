// mainwindow.cc
// 6/30/2011

#include "mainwindow.h"

#include "db.h"
#include "datamanager.h"
#include "logger.h"
#include "eventlogger.h"
#include "grabber.h"
#include "osdconsole.h"
#include "osdwindow.h"
#include "playerui.h"
#include "videoview.h"
#include "osdplayer.h"
#include "signalhub.h"
#include "mainplayer.h"
#include "miniplayer.h"
#include "annotationgraphicsview.h"
#include "annotationeditor.h"
#include "tokenview.h"
#include "commentview.h"
#include "tray.h"
#include "cloudview.h"
#include "annotationbrowser.h"
#include "stylesheet.h"
#include "uistyle.h"
#include "rc.h"
#include "tr.h"
#include "translator.h"
#include "logindialog.h"
#include "pickdialog.h"
#include "seekdialog.h"
#include "livedialog.h"
#include "syncdialog.h"
#include "settings.h"
#include "global.h"
#ifdef USE_MODE_SIGNAL
  #include "signalview.h"
  #include "messageview.h"
  #include "processview.h"
  #include "messagehandler.h"
#endif // USE_MODE_SIGNAL
#include "module/player/player.h"
#ifdef USE_MODULE_SERVERAGENT
  #include "module/serveragent/serveragent.h"
#endif // USE_MODULE_SERVERAGENT
#ifdef USE_MODULE_CLIENTAGENT
  #include "module/clientagent/clientagent.h"
#endif // USE_MODULE_CLIENTAGENT
#ifdef USE_MODULE_DOLL
  #include "module/doll/doll.h"
#endif // USE_MODULE_DOLL
//#ifdef USE_WIN_DWM
//  #include "win/dwm/dwm.h"
//#endif // USE_WIN_DWM
#ifdef USE_WIN_QTH
  #include "win/qth/qth.h"
#endif // USE_WIN_QTH
#ifdef USE_WIN_QTWIN
  #include "win/qtwin/qtwin.h"
#endif // USE_WIN_QTWIN
#ifdef USE_WIN_PICKER
  #include "win/picker/picker.h"
#endif // USE_WIN_PICKER
#include "core/os.h"
#include "core/cmd.h"
#include "core/annotationparser.h"
#include "core/util/datetime.h"
#include "core/htmltag.h"
#include "core/gui/actionwithid.h"
#include "core/cloud/traits.h"
#include "core/cloud/alias.h"
#include "core/cloud/token.h"
#include "core/cloud/annotation.h"
#include <QtGui>
#include <boost/tuple/tuple.hpp>
#include <boost/typeof/typeof.hpp>

#ifndef USE_MODULE_SERVERAGENT
  #error "Module server agent is indispensible."
#endif // USE_MODULE_SERVERAGENT

using namespace Core::Cloud;
using namespace Logger;

#define DEBUG "MainWindow"
#include "module/debug/debug.h"

// - Helpers -

namespace { // anonymous

  template <typename L>
  inline L
  revertList(const L &l)
  {
    L ret;
    if (!l.isEmpty())
      foreach (typename L::value_type v, l)
        ret.push_front(v);
    return ret;
  }

} // anonymous namespace

// - Tasks -

namespace { namespace task_ { // anonymous

  class login : public QRunnable
  {
    MainWindow *w_;
    QString name_, pass_;
    virtual void run() { w_->login(name_, pass_, false); } // \override, async = false
  public:
    login(const QString userName, const QString &password, MainWindow *w)
      : w_(w), name_(userName), pass_(password) { Q_ASSERT(w_); }
  };

  class logout : public QRunnable
  {
    MainWindow *w_;
    virtual void run() { w_->logout(false); } // \override, async = false
  public:
    explicit logout(MainWindow *w) : w_(w) { Q_ASSERT(w_); }
  };

  class setUserAnonymous : public QRunnable
  {
    MainWindow *w_;
    bool t_;
    virtual void run() { w_->setUserAnonymous(t_, false); } // \override, async = false
  public:
    setUserAnonymous(bool t, MainWindow *w) : w_(w), t_(t) { Q_ASSERT(w_); }
  };

  class setUserLanguage : public QRunnable
  {
    MainWindow *w_;
    int l_;
    virtual void run() { w_->setUserLanguage(l_, false); } // \override, async = false
  public:
    setUserLanguage(int l, MainWindow *w) : w_(w), l_(l) { Q_ASSERT(w_); }
  };

  class chat : public QRunnable
  {
    MainWindow *w_;
    QString text_;
    virtual void run() { w_->chat(text_, false); } // \override, async = false
  public:
    chat(const QString &text, MainWindow *w) : w_(w), text_(text) { Q_ASSERT(w_); }
  };

  class submitText : public QRunnable
  {
    MainWindow *w_;
    QString text_;
    virtual void run() { w_->submitText(text_, false); } // \override, async = false
  public:
    submitText(const QString &text, MainWindow *w) : w_(w), text_(text) { Q_ASSERT(w_); }
  };

  class submitAlias : public QRunnable
  {
    MainWindow *w_;
    Alias a_;
    virtual void run() { w_->submitAlias(a_, false); } // \override, async = false
  public:
    submitAlias(const Alias &a, MainWindow *w) : w_(w), a_(a) { Q_ASSERT(w_); }
  };

  class setToken : public QRunnable
  {
    MainWindow *w_;
    QString t_;
    virtual void run() { w_->setToken(t_, false); } // \override, async = false
  public:
    setToken(const QString &t, MainWindow *w) : w_(w), t_(t) { Q_ASSERT(w_); }
  };

} } // anonymous namespace task_

// - Hook -
#ifdef USE_WIN_HOOK

HookEventForwarder::HookEventForwarder(MainWindow *w, QObject *parent)
  : Base(parent), w_(w)
{ Q_ASSERT(w_); }

bool
HookEventForwarder::eventFilter(QObject *hook, QEvent *event)
{
  // jichi 8/2/2011: Ugly fix for cancelling context menu.
  static bool contextMenuPoppedUp = false;

  if (event)
    switch (event->type()) {
    case QEvent::ContextMenu:
      contextMenuPoppedUp = true;
      QApplication::sendEvent(w_, event);
      return true;

      // Forward
    case QEvent::MouseButtonDblClick:
      contextMenuPoppedUp = false;
    case QEvent::MouseMove:
    case QEvent::Wheel:
      QApplication::sendEvent(w_, event);
      return true;

    case QEvent::MouseButtonPress: {
        QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
        Q_ASSERT(mouseEvent);
        if (mouseEvent->button() == Qt::LeftButton
            && !contextMenuPoppedUp)
          if (w_->isPlaying() || w_->isEditing())
            w_->pause();
      }
      contextMenuPoppedUp = false;
    case QEvent::MouseButtonRelease:
      event->accept();
      return true;

    //case QEvent::KeyPress:
    //case QEvent::KeyRelease:
    //  //QApplication::sendEvent(w_, event);
    //  if (w_->visiblePlayer())
    //    w_->visiblePlayer()->lineEdit()->clearFocus();
    //  w_->setFocus();
    //  return true;
    }

  return Base::eventFilter(hook, event);
}

#endif // USE_WIN_HOOK

// - Focus -

void
MainWindow::onFocusedWidgetChanged(QWidget *w_old, QWidget *w_new)
{
#ifdef USE_WIN_QTWIN
  // When OSDPlayer's lineEdit cleared its focus.
  if (!w_new && w_old == osdPlayer_->lineEdit())
    QtWin::sendMouseClick(QPoint(0, 0), Qt::LeftButton);
#else
  Q_UNUSED(w_old);
  Q_UNUSED(w_new);
#endif // USE_WIN_QTWIN
}

bool
MainWindow::isEditing() const
{
  return commandLineEditHasFocus()
      && prefixLineEditHasFocus();
}

bool
MainWindow::commandLineEditHasFocus() const
{
  return mainPlayer_->lineEdit()->hasFocus()
      || miniPlayer_->lineEdit()->hasFocus()
      || osdPlayer_->lineEdit()->hasFocus();
}

bool
MainWindow::prefixLineEditHasFocus() const
{
  return mainPlayer_->prefixLineEdit()->hasFocus()
      || miniPlayer_->prefixLineEdit()->hasFocus()
      || osdPlayer_->prefixLineEdit()->hasFocus();
}

// - Constructions -

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f),
    dragPos_(BAD_POS),
    lastOpenedPath_(INIT_OPEN_PATH)
{
  UiStyle::globalInstance()->setMainWindowStyle(this);
  if (menuBar())
    menuBar()->hide();

  setWindowTitle(TR(T_TITLE_PROGRAM));
  setContentsMargins(0, 0, 0, 0);

  createComponents();

  createConnections();

  createActions();

  createMenus();

  createDockWindows();


  // Post behaviors

  // Receive mouseMoveEvent even no mouse button is clicked
  // Set this to enable resetAutoHide osdPlayer when mouse is moved.
  setMouseTracking(true);

  // Adjust OSDPlayer to fit in OSDWindow.
  osdPlayer_->invalidateGeometry();

  // Global hook
#ifdef USE_WIN_HOOK
  HOOK->setMouseHookEnabled(true);
  //HOOK->setKeyboardHookEnabled(false);
  HOOK->setThreadsFilterEnabled(false);         // Use global hook.
  HOOK->setWindowsFilterEnabled(true);          // Only enable for specified window
  HOOK->setContextMenuEventEnabled(true);       // Listen to context menu event.
  HOOK->setDoubleClickEventEnabled(true);       // Listen to double click event.
  HOOK->setMouseMoveEventEscaped(true);         // Always receive mouse move event
  HOOK->installEventFilter(new HookEventForwarder(this, this));

  //connect(player_, SIGNAL(mediaOpen()), videoView_, SLOT(addToWindowsHook()));
  connect(player_, SIGNAL(playing()), videoView_, SLOT(addToWindowsHook()));
  connect(player_, SIGNAL(mediaClosed()), videoView_, SLOT(removeFromWindowsHook()));

  HOOK->start();
#endif // USE_WIN_HOOK

  recent_ = Settings::globalInstance()->recent();

  // Initial focus
  mainPlayer_->lineEdit()->setFocus();

  // System tray icon
  tray_->show();

  // Parse arguments
  //parseArguments(args);

  //statusBar()->showMessage(tr("Ready"));
}

void
MainWindow::createComponents()
{
  // Signal hub
  hub_ = new SignalHub(this);

  // Systemt tray icon
  tray_ = new Tray(this);

  // Utilities
  grabber_ = new Grabber(this);

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

  // Data manager
  dataManager_ = new DataManager(server_, cache_, queue_, this);

  // Qth
#ifdef USE_WIN_QTH
  //Qth *qth = new Qth(winId(), this);
  //Qth::setGlobalInstance(qth);
  //QTH->setParentWinId(winId());
#endif // USE_WIN_QTH

  // Player
  player_ = new Player(this); {
    //player_->setKeyboardEnabled(false);
#ifndef Q_WS_WIN
    player_->setMouseEnabled(false);
#endif // Q_WS_WIN
    //player_->setEncoding("UTF-8");
  }
  //assert(isValid()); // TODO: fix this using a pop up dialog and then exit

  // Logger
  logger_ = new EventLogger(player_, this);

  // Players and graphic views
  videoView_ = new VideoView(this);
  osdWindow_ = new OSDWindow(this);

  annotationView_ = new AnnotationGraphicsView(hub_,  player_, videoView_, osdWindow_);
#ifdef USE_WIN_DWM
  {
    QWidget *w = annotationView_->editor();
    w->setParent(this, w->windowFlags()); // hot fix for dwm displaying issue
    UiStyle::globalInstance()->setWindowDwmEnabled(w->winId(), false);
    UiStyle::globalInstance()->setWindowDwmEnabled(w->winId(), true);
  }
#endif // USE_WIN_DWM
#ifdef Q_WS_MAC // remove background in annotation view
  annotationView_->setStyleSheet(
    SS_BEGIN(QWidget)
      SS_TRANSPARENT
    SS_END
  );
#endif // Q_WS_MAC

  //QLayout *layout = new QStackedLayout(osdWindow_);
  //layout->addWidget(annotationView_);
  //osdWindow_->setLayout(layout);
  osdWindow_->setEventListener(annotationView_);

  osdPlayer_ = new OSDPlayerUi(hub_, player_, server_, osdWindow_);
#ifdef Q_WS_MAC // remove background in OSD player
  osdPlayer_->setStyleSheet(
    SS_BEGIN(QWidget)
      SS_TRANSPARENT
    SS_END
  );
#endif // Q_WS_MAC

  globalOsdConsole_ = new OSDConsole(annotationView_);
  globalOsdConsole_->setAutoClearInterval(G_CONSOLE_AUTOClEAR_TIMEOUT);
  OSDConsole::setGlobalInstance(globalOsdConsole_);

  mainPlayer_ = new MainPlayerUi(hub_, player_, server_, this);
  miniPlayer_ = new MiniPlayerUi(hub_, player_, server_, this);

#ifdef Q_WS_MAC
  player_->setEmbeddedWindowHandle(videoView_->view());
#else
  player_->embed(videoView_);
#endif // Q_WS_MAC

  //annotationView_->hide();
  miniPlayer_->hide();
  osdPlayer_->hide();
  osdPlayer_->setAutoHideEnabled();

  // Dialogs

  //userPanel_ = new UserPanel(this);
  //userPanel_->hide(); // TODO!!!!!!

  loginDialog_ = new LoginDialog(this);
  seekDialog_ = new SeekDialog(this);
  syncDialog_ = new SyncDialog(this);

  windowPickDialog_ = new PickDialog(this);
  windowPickDialog_->setMessage(tr("Select annots window"));

  processPickDialog_ = new PickDialog(this);
  processPickDialog_->setMessage(tr("Select process window to open"));

  //liveDialog_ = new LiveDialog(this);
  //liveDialog_ = 0; // TODO

  tokenView_ = new TokenView(this);
  annotationBrowser_ = new AnnotationBrowser(this);
  annotationEditor_ = new AnnotationEditor(this);
  cloudView_ = new CloudView(this);
  commentView_ = new CommentView(this);

#ifdef USE_MODE_SIGNAL
  messageHandler_ = new MessageHandler(this);
  signalView_ = new SignalView(this);
  recentMessageView_ = new MessageView(this);
#endif USE_MODE_SIGNAL

#ifdef USE_MODULE_DOLL
  doll_ = new Doll(this);
  connect(doll_, SIGNAL(said(QString)), this, SLOT(respond(QString)));
#endif // USE_MODULE_DOLL

}

void
MainWindow::createConnections()
{
  connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)),
          this, SLOT(onFocusedWidgetChanged(QWidget*, QWidget*)));

  connect(player_, SIGNAL(mediaChanged()), annotationBrowser_, SLOT(clear()));

  // hub
  connect(hub_, SIGNAL(playModeChanged(SignalHub::PlayMode)), SLOT(updatePlayMode()));
  connect(hub_, SIGNAL(playerModeChanged(SignalHub::PlayerMode)), SLOT(updatePlayerMode()));
  connect(hub_, SIGNAL(tokenModeChanged(SignalHub::TokenMode)), SLOT(updateTokenMode()));
  connect(hub_, SIGNAL(videoModeChanged(SignalHub::VideoMode)), SLOT(updateVideoMode()));
  connect(hub_, SIGNAL(opened()),SLOT(open()));
  connect(hub_, SIGNAL(played()),SLOT(play()));
  connect(hub_, SIGNAL(paused()),SLOT(pause()));
  connect(hub_, SIGNAL(stopped()),SLOT(stop()));

  // Player UI

  #define CONNECT(_playerui) \
    connect(_playerui, SIGNAL(commandEntered(QString)), SLOT(eval(QString))); \
    connect(_playerui, SIGNAL(showUserPanelRequested()), SLOT(showUserPanel())); \
    connect(_playerui, SIGNAL(showPositionPanelRequested()), SLOT(showSeekDialog())); \
    connect(_playerui->lineEdit(), SIGNAL(textChanged(QString)), SLOT(syncLineEditText(QString))); \
    connect(_playerui->prefixLineEdit(), SIGNAL(textChanged(QString)), SLOT(syncPrefixLineEditText(QString))); \
    connect(_playerui->toggleAnnotationButton(), SIGNAL(clicked()), annotationView_, SLOT(togglePlaybackEnabled())); \
    connect(annotationView_, SIGNAL(playbackEnabledChanged(bool)), _playerui, SLOT(setAnnotationEnabled(bool)));

    CONNECT(mainPlayer_)
    CONNECT(miniPlayer_)
    CONNECT(osdPlayer_)
  #undef CONNECT

  connect(osdPlayer_, SIGNAL(invalidateMenuRequested()), SLOT(invalidateContextMenu()));

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
#ifdef USE_MODE_SIGNAL
  //connect(signalView_->tokenView(), SIGNAL(aliasSubmitted(Alias)), SLOT(submitAlias(Alias)));

  connect(signalView_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  connect(signalView_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  connect(signalView_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  connect(signalView_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));
#endif // USE_MODE_SIGNAL

  connect(tokenView_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  connect(tokenView_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  connect(tokenView_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  connect(tokenView_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));

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

  //connect(miniPlayer_, SIGNAL(togglePlayModeRequested()), hub_, SLOT(toggleFullScreenVideoMode()));
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

  // - Picker -
  connect(windowPickDialog_, SIGNAL(windowPicked(WId)), annotationView_, SLOT(setTrackingWindow(WId)));
  connect(processPickDialog_, SIGNAL(windowPicked(WId)), annotationView_, SLOT(setTrackingWindow(WId)));
#ifdef USE_MODE_SIGNAL
  connect(processPickDialog_, SIGNAL(windowPicked(WId)), SLOT(openProcessWindow(WId)));
#endif // USE_MODE_SIGNAL
  connect(annotationView_, SIGNAL(trackingWindowChanged(WId)), osdPlayer_, SLOT(setTrackingWindow(WId)));

  // Synchronize text edit
  //#define SYNC(_playerui1, _playerui2)
  //  connect(_playerui1->lineEdit(), SIGNAL(textChanged(QString)), _playerui2->lineEdit(), SLOT(setText(QString)));
  //  connect(_playerui2->lineEdit(), SIGNAL(textChanged(QString)), _playerui1->lineEdit(), SLOT(setText(QString)));
  //SYNC(mainPlayer_, miniPlayer_)
  //SYNC(mainPlayer_, osdPlayer_)
  //SYNC(miniPlayer_, osdPlayer_)
  //#undef SYNC

  //connect(server_, SIGNAL(annotationsReceived(AnnotationList)),
  //        annotationView_, SLOT(addAnnotations(AnnotationList)));
  //connect(server_, SIGNAL(tokensReceived(TokenList)),
  //        this, SLOT(processTokens(TokenList)));

  connect(loginDialog_, SIGNAL(loginRequested(QString, QString)), SLOT(login(QString, QString)));
  connect(seekDialog_, SIGNAL(seekRequested(qint64)), SLOT(seek(qint64)));

#ifdef Q_WS_MAC
  connect(player_, SIGNAL(playing()), videoView_, SLOT(showView()));
  connect(player_, SIGNAL(stopped()), videoView_, SLOT(hideView()));
#endif // Q_WS_MAC

  // Agents:
#ifdef USE_MODULE_CLIENTAGENT
  connect(client_, SIGNAL(chatReceived(QString)), this, SLOT(respond(QString)));
#endif // USE_MODULE_CLIENTAGENT

  // Language:
  connect(Translator::globalInstance(), SIGNAL(languageChanged()), SLOT(invalidateAppLanguage()));

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

#ifdef USE_MODULE_CLIENTAGENT
  connect(client_, SIGNAL(authorized()), logger_, SLOT(logClientAgentAuthorized()));
  connect(client_, SIGNAL(deauthorized()), logger_, SLOT(logClientAgentDeauthorized()));
  connect(client_, SIGNAL(authorizationError()), logger_, SLOT(logClientAgentAuthorizationError()));
#endif // USE_MODULE_CLIENTAGENT

#ifdef USE_MODE_SIGNAL
  connect(signalView_, SIGNAL(hookSelected(int,ProcessInfo)), SLOT(openProcessHook(int,ProcessInfo)));
  connect(recentMessageView_, SIGNAL(hookSelected(int)), SLOT(openProcessHook(int)));
  connect(messageHandler_, SIGNAL(messageReceived(qint64)), annotationView_, SLOT(showAnnotationsAtPos(qint64)));
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
    _action->setStatusTip(TR(T_STATUSTIP_##_styleid)); \
  } connect(_action, SIGNAL(triggered()), _receiver, _slot);
#define MAKE_TOGGLE(_action, _styleid, _receiver, _slot) { \
    _action = new QAction(QIcon(RC_IMAGE_##_styleid), TR(T_MENUTEXT_##_styleid), this); \
    _action->setStatusTip(TR(T_STATUSTIP_##_styleid)); \
    _action->setCheckable(true); \
  } connect(_action, SIGNAL(triggered(bool)), _receiver, _slot);

  MAKE_ACTION(openAct_,         OPEN,           hub_,           SLOT(open()))
  MAKE_ACTION(openFileAct_,     OPENFILE,       this,           SLOT(openFile()))
  MAKE_ACTION(openDeviceAct_,   OPENDEVICE,     this,           SLOT(openDevice()))
  MAKE_ACTION(openSubtitleAct_, OPENSUBTITLE,   this,           SLOT(openSubtitle()))
  MAKE_ACTION(playAct_,         PLAY,           hub_,           SLOT(play()))
  MAKE_ACTION(pauseAct_,        PAUSE,          hub_,           SLOT(pause()))
  MAKE_ACTION(nextFrameAct_,    NEXTFRAME,      player_,        SLOT(nextFrame()))
  MAKE_ACTION(stopAct_,         STOP,           hub_,           SLOT(stop()))
  MAKE_ACTION(replayAct_,       REPLAY,         this,           SLOT(replay()))
  MAKE_ACTION(snapshotAct_,     SNAPSHOT,       this,           SLOT(snapshot()))
  MAKE_TOGGLE(toggleFullScreenModeAct_, FULLSCREEN, hub_,       SLOT(setFullScreenVideoMode(bool)))
  MAKE_TOGGLE(toggleMiniModeAct_, MINI,         hub_,           SLOT(setMiniPlayerMode(bool)))
  MAKE_TOGGLE(toggleLiveModeAct_, LIVE,         hub_,           SLOT(setLivePlayMode(bool)))
  MAKE_TOGGLE(toggleSyncModeAct_, SYNC,         hub_,           SLOT(setSyncPlayMode(bool)))
  MAKE_TOGGLE(toggleAnnotationVisibleAct_, SHOWANNOT, annotationView_, SLOT(setPlaybackEnabled(bool)))
  MAKE_TOGGLE(toggleSubtitleVisibleAct_, SHOWSUBTITLE, player_, SLOT(setSubtitleVisible(bool)))
  //MAKE_TOGGLE(toggleWindowStaysOnTopAct_, WINDOWSTAYSONTOP, this, SLOT(setWindowStaysOnTop(bool)))
  MAKE_TOGGLE(toggleUserAnonymousAct_,  ANONYMOUS,       this,         SLOT(setUserAnonymous(bool)))
  MAKE_TOGGLE(toggleUserPanelVisibleAct_, USER,          this,         SLOT(setUserPanelVisible(bool)))
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
  MAKE_ACTION(loginAct_,        LOGIN,          this,           SLOT(showLoginDialog()))
  MAKE_ACTION(logoutAct_,       LOGOUT,         this,           SLOT(logout()))
  MAKE_ACTION(clearCacheAct_,   CLEARCACHE,     cache_,         SLOT(clear()))
  MAKE_ACTION(connectAct_,      CONNECT,        server_,        SLOT(updateConnected()))
  MAKE_ACTION(disconnectAct_,   DISCONNECT,     server_,        SLOT(disconnect()))
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

  MAKE_ACTION(recent0Act_,      RECENTFILE,     this,   SLOT(openRecent0()))
  MAKE_ACTION(recent1Act_,      RECENTFILE,     this,   SLOT(openRecent1()))
  MAKE_ACTION(recent2Act_,      RECENTFILE,     this,   SLOT(openRecent2()))
  MAKE_ACTION(recent3Act_,      RECENTFILE,     this,   SLOT(openRecent3()))
  MAKE_ACTION(recent4Act_,      RECENTFILE,     this,   SLOT(openRecent4()))
  MAKE_ACTION(recent5Act_,      RECENTFILE,     this,   SLOT(openRecent5()))
  MAKE_ACTION(recent6Act_,      RECENTFILE,     this,   SLOT(openRecent6()))
  MAKE_ACTION(recent7Act_,      RECENTFILE,     this,   SLOT(openRecent7()))
  MAKE_ACTION(recent8Act_,      RECENTFILE,     this,   SLOT(openRecent8()))
  MAKE_ACTION(recent9Act_,      RECENTFILE,     this,   SLOT(openRecent9()))
  MAKE_ACTION(clearRecentAct_,  CLEARRECENT,    this,   SLOT(clearRecent()))

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

  MAKE_ACTION(aboutAct_,        ABOUT,          this,   SLOT(about()))
  MAKE_ACTION(helpAct_,         HELP,           this,   SLOT(help()))
  MAKE_ACTION(quitAct_,         QUIT,           this,   SLOT(close()))

  openAct_->setShortcuts(QKeySequence::Open);
  helpAct_->setShortcuts(QKeySequence::HelpContents);
  quitAct_->setShortcuts(QKeySequence::Quit);

#undef MAKE_ACTION
}

void
MainWindow::createMenus()
{
  // Context menu
  contextMenu_ = new QMenu(TR(T_TITLE_PROGRAM), this);
  UiStyle::globalInstance()->setContextMenuStyle(contextMenu_);
  osdPlayer_->setMenu(contextMenu_);

  appLanguageContextMenu_ = new QMenu(this); {
    UiStyle::globalInstance()->setContextMenuStyle(appLanguageContextMenu_);

    appLanguageContextMenu_->setTitle("Language"); // no tr wrapping "language"
    appLanguageContextMenu_->setToolTip(TR(T_TOOLTIP_APPLANGUAGE));

    appLanguageContextMenu_->addAction(setAppLanguageToEnglishAct_);
    appLanguageContextMenu_->addAction(setAppLanguageToJapaneseAct_);
    appLanguageContextMenu_->addAction(setAppLanguageToTraditionalChineseAct_);
    appLanguageContextMenu_->addAction(setAppLanguageToSimplifiedChineseAct_);
  }

  userLanguageContextMenu_ = new QMenu(this); {
    UiStyle::globalInstance()->setContextMenuStyle(userLanguageContextMenu_);

    userLanguageContextMenu_->setTitle(TR(T_MENUTEXT_USERLANGUAGE));
    userLanguageContextMenu_->setToolTip(TR(T_TOOLTIP_USERLANGUAGE));

    userLanguageContextMenu_->addAction(setUserLanguageToEnglishAct_);
    userLanguageContextMenu_->addAction(setUserLanguageToJapaneseAct_);
    userLanguageContextMenu_->addAction(setUserLanguageToChineseAct_);
    userLanguageContextMenu_->addAction(setUserLanguageToKoreanAct_);
    userLanguageContextMenu_->addAction(setUserLanguageToUnknownAct_);
  }

  annotationLanguageContextMenu_ = new QMenu(this); {
    UiStyle::globalInstance()->setContextMenuStyle(annotationLanguageContextMenu_);

    annotationLanguageContextMenu_->setTitle(TR(T_MENUTEXT_ANNOTATIONLANGUAGE));
    annotationLanguageContextMenu_->setToolTip(TR(T_TOOLTIP_ANNOTATIONLANGUAGE));

    annotationLanguageContextMenu_->addAction(toggleAnnotationLanguageToAnyAct_);
    annotationLanguageContextMenu_->addAction(toggleAnnotationLanguageToUnknownAct_);
    annotationLanguageContextMenu_->addSeparator();
    annotationLanguageContextMenu_->addAction(toggleAnnotationLanguageToEnglishAct_);
    annotationLanguageContextMenu_->addAction(toggleAnnotationLanguageToJapaneseAct_);
    annotationLanguageContextMenu_->addAction(toggleAnnotationLanguageToChineseAct_);
    annotationLanguageContextMenu_->addAction(toggleAnnotationLanguageToKoreanAct_);
  }

  recentMenu_ = new QMenu(this); {
    UiStyle::globalInstance()->setContextMenuStyle(recentMenu_);
    recentMenu_->setTitle(TR(T_MENUTEXT_RECENT));
    recentMenu_->setToolTip(TR(T_TOOLTIP_RECENT));
  }

  backwardContextMenu_ = new QMenu(this); {
    backwardContextMenu_->setIcon(QIcon(RC_IMAGE_BACKWARD));
    backwardContextMenu_->setTitle(TR(T_MENUTEXT_BACKWARD));
    backwardContextMenu_->setToolTip(TR(T_TOOLTIP_BACKWARD));
    UiStyle::globalInstance()->setContextMenuStyle(backwardContextMenu_);

    backwardContextMenu_->addAction(backward5sAct_);
    backwardContextMenu_->addAction(backward30sAct_);
    backwardContextMenu_->addAction(backward90sAct_);
    backwardContextMenu_->addAction(backward5mAct_);
    backwardContextMenu_->addAction(backward10mAct_);
  }

  forwardContextMenu_ = new QMenu(this); {
    forwardContextMenu_->setIcon(QIcon(RC_IMAGE_FORWARD));
    forwardContextMenu_->setTitle(TR(T_MENUTEXT_FORWARD));
    forwardContextMenu_->setToolTip(TR(T_TOOLTIP_FORWARD));
    UiStyle::globalInstance()->setContextMenuStyle(forwardContextMenu_);

    forwardContextMenu_->addAction(forward5sAct_);
    forwardContextMenu_->addAction(forward30sAct_);
    forwardContextMenu_->addAction(forward90sAct_);
    forwardContextMenu_->addAction(forward5mAct_);
    forwardContextMenu_->addAction(forward10mAct_);
  }

  openContextMenu_ = new QMenu(this); {
    openContextMenu_->setTitle(TR(T_MENUTEXT_OPENCONTEXTMENU));
    openContextMenu_->setToolTip(TR(T_TOOLTIP_OPENCONTEXTMENU));
    openContextMenu_->setIcon(QIcon(RC_IMAGE_OPENCONTEXTMENU));
    UiStyle::globalInstance()->setContextMenuStyle(openContextMenu_);
  }

  subtitleContextMenu_ = new QMenu(this); {
    subtitleContextMenu_->setIcon(QIcon(RC_IMAGE_SUBTITLE));
    subtitleContextMenu_->setTitle(TR(T_MENUTEXT_SUBTITLE));
    subtitleContextMenu_->setToolTip(TR(T_TOOLTIP_SUBTITLE));
    UiStyle::globalInstance()->setContextMenuStyle(subtitleContextMenu_);
  }

#ifdef Q_WS_MAC
  fileMenu_ = menuBar()->addMenu(tr("&File")); {
    //fileMenu_->addAction(newAct);
    fileMenu_->addAction(openAct_);
    //fileMenu_->addAction(saveAct);
    //fileMenu_->addAction(printAct);
    //fileMenu_->addSeparator();
    fileMenu_->addAction(quitAct_);
  }

  //editMenu = menuBar()->addMenu(tr("&Edit"));
  //editMenu->addAction(undoAct);
  //editMenu->addAction(insertAct);
  //editMenu->addAction(analyzeAct);
  //editMenu->addAction(transformAct);
  //editMenu->addAction(applyAct);
  //editMenu->addAction(generateAct);
  //editMenu->addAction(clearAct);

  viewMenu_ = menuBar()->addMenu(tr("&View"));

  menuBar()->addSeparator();

  helpMenu_ = menuBar()->addMenu(tr("&Help"));
  helpMenu_->addAction(aboutAct_);
#endif // Q_WS_MAC
}

// jichi:10/17/2011: TODO: Totally remove dock widgets; use layout instead
void
MainWindow::createDockWindows()
{
  // Main view
  setCentralWidget(videoView_);

  // OSD view
  osdDock_ = new OSDDock(this);
  osdDock_->setWidget(osdWindow_);
  annotationView_->setFullScreenView(osdDock_);
  osdDock_->showFullScreen();

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
  bool m = hub_->isMediaTokenMode();
  switch (hub_->playerMode()) {
  case SignalHub::NormalPlayerMode:
    if (!m && annotationView_->trackingWindow())
      hub_->setFullScreenPlayerMode(!m);
    else
      hub_->setMiniPlayerMode(!m);
    break;

  case SignalHub::FullScreenPlayerMode:
    mainPlayer_->hide();
    miniPlayer_->hide();
    if (m)
      hub_->setNormalPlayerMode();

    //setVisible(m);
    //osdPlayer_->show();
    //osdDock_->raise();
    break;

  case SignalHub::MiniPlayerMode:
    if (m)
      hub_->setNormalPlayerMode();
    else {
      hide();
      mainPlayer_->hide();
      osdPlayer_->hide();
      miniPlayer_->show();
    }
    break;
  }

  if (m)
    annotationView_->setTrackingWindow(0);

  annotationView_->invalidateGeometry();
  osdPlayer_->invalidateGeometry();

#ifdef USE_WIN_QTH
  messageHandler_->setActive(!m);
#endif // USE_WIN_QTH
}

void
MainWindow::updatePlayMode()
{
  // TODO
}

void
MainWindow::updatePlayerMode()
{
  bool v = hub_->isMediaTokenMode();
  switch (hub_->playerMode()) {
  case SignalHub::NormalPlayerMode:
    if (osdPlayer_->isVisible()) osdPlayer_->hide();
    if (miniPlayer_->isVisible()) miniPlayer_->hide();
    if (!mainPlayer_->isVisible()) mainPlayer_->show();
    show();
    break;

  case SignalHub::FullScreenPlayerMode:
    setVisible(v);
    if (mainPlayer_->isVisible()) mainPlayer_->hide();
    if (miniPlayer_->isVisible()) miniPlayer_->hide();
    if (!osdPlayer_->isVisible()) { osdPlayer_->show(); osdDock_->raise(); }
    break;

  case SignalHub::MiniPlayerMode:
    setVisible(v);
    if (mainPlayer_->isVisible()) mainPlayer_->hide();
    if (osdPlayer_->isVisible()) osdPlayer_->hide();
    if (!miniPlayer_->isVisible()) miniPlayer_->show();
    break;
  }

  annotationView_->invalidateGeometry();
}

void
MainWindow::updateVideoMode()
{
  switch (hub_->videoMode()) {
  case SignalHub::FullScreenVideoMode:
#ifdef USE_WIN_DWM
    UiStyle::globalInstance()->setWindowDwmEnabled(this, false);
#endif // USE_WIN_DWM
    if (mainPlayer_->isVisible())
      mainPlayer_->hide();

    if (hub_->isMediaTokenMode())
      showFullScreen();

    if (hub_->isNormalPlayerMode()
        || hub_->isMiniPlayerMode() && hub_->isSignalTokenMode())
      hub_->setFullScreenPlayerMode();
    break;

  case SignalHub::NormalVideoMode:
    if (osdPlayer_->isVisible())
      osdPlayer_->hide();

    if (hub_->isMediaTokenMode())
      showNormal();

    if (hub_->isMediaTokenMode() && hub_->isFullScreenPlayerMode()) {
      hub_->setNormalPlayerMode();
    } else if (hub_->isSignalTokenMode())
      hub_->setMiniPlayerMode();

    annotationView_->invalidateSize(); // Otherwise the height doesn't look right

#ifdef USE_WIN_DWM
    UiStyle::globalInstance()->setWindowDwmEnabled(this, true);
#endif // USE_WIN_DWM
    break;
  }
}

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
  if (!QDir(lastOpenedPath_).exists())
    lastOpenedPath_.clear();

  static const QString filter =
      TR(T_FORMAT_SUPPORTED) + ("(" G_FORMAT_SUPPORTED ")" ";;")
    + TR(T_FORMAT_VIDEO)     + ("(" G_FORMAT_VIDEO ")" ";;")
    + TR(T_FORMAT_AUDIO)     + ("(" G_FORMAT_AUDIO ")" ";;")
    + TR(T_FORMAT_ALL)       + ("(" G_FORMAT_ALL ")");

  QString fileName = QFileDialog::getOpenFileName(
        this, TR(T_TITLE_OPENFILE), lastOpenedPath_, filter);

  if (!fileName.isEmpty()) {
    lastOpenedPath_ = QFileInfo(fileName).absolutePath();
    openPath(fileName);
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
  return path.contains(QRegExp("^[a-zA-Z]:$"))
      || path.contains(QRegExp("^[a-zA-Z]:\\\\$"));
#else
  Q_UNUSED(path)
  return true;
#endif // Q_WS_WIN
}

void
MainWindow::openDevice()
{
  if (!QDir(lastOpenedPath_).exists())
    lastOpenedPath_.clear();

  QString path = QFileDialog::getExistingDirectory(
        this, TR(T_TITLE_OPENDEVICE), lastOpenedPath_);

  if (!path.isEmpty()) {
    lastOpenedPath_ = path;

    if (isDevicePath(path))
      openPath(path);
    else
      warn(TR(T_ERROR_BAD_DEVICEPATH) + ": " + path);
  }
}

void
MainWindow::openDirectory()
{
  if (!QDir(lastOpenedPath_).exists())
    lastOpenedPath_.clear();

  QString path = QFileDialog::getExistingDirectory(
        this, TR(T_TITLE_OPENDEVICE), lastOpenedPath_);

  if (!path.isEmpty()) {
    lastOpenedPath_ = path;
    openPath(path);
  }
}

void
MainWindow::openSubtitle()
{
  if (!QDir(lastOpenedPath_).exists())
    lastOpenedPath_.clear();

  static const QString filter =
      TR(T_FORMAT_SUBTITLE)  + ("(" G_FORMAT_SUBTITLE ")" ";;")
    + TR(T_FORMAT_ALL)       + ("(" G_FORMAT_ALL ")");

  QString fileName = QFileDialog::getOpenFileName(
        this, TR(T_TITLE_OPENFILE), lastOpenedPath_, filter);

  if (!fileName.isEmpty()) {
    lastOpenedPath_ = QFileInfo(fileName).absolutePath();
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
  else {
    bool checkPath = false;
    openPath(url.toEncoded(), checkPath);
  }
}

void
MainWindow::openUrls(const QList<QUrl> &urls)
{
  // FIXME: add to playlist rather then this approach!!!!!
  if (!urls.empty()) {
    //foreach (const QUrl &url, urls)
    //  openUrl(url);
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
MainWindow::openPath(const QString &path, bool checkPath)
{
  if (hub_->isSignalTokenMode())
    hub_->stop();
  hub_->setMediaTokenMode();

  if (checkPath) {
    if (!QFileInfo(path).exists()) {
      warn(TR(T_ERROR_BAD_FILEPATH) + ": " + path);
      return;
    }
  }

  player_->openMedia(path);
  if (player_->hasMedia()) {
    //if (mode_ == NormalPlayMode)
    //  videoView_->resize(INIT_WINDOW_SIZE);

    setToken(path);

#ifdef Q_WS_MAC
    if (!videoView_->isViewVisible())
       videoView_->showView();
#endif // Q_WS_MAC

    addRecent(path);

    QTimer::singleShot(0, player_, SLOT(loadExternalSubtitles()));
    player_->play();

    invalidateWindowTitle();
  }
}

void
MainWindow::openSubtitlePath(const QString &path)
{
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
  DOUT("seek: enter: time =" << time);

  if (hub_->isSignalTokenMode())
    return;

  if (!player_->hasMedia() || !player_->seekable())
    return;

  if (time <= 0) {
    //pause();
    //player_->setTime(0);
    replay();
    DOUT("seek: exit: time <=0, time =" << time);
    return;
  }

  qint64 length = player_->mediaLength();
  if (time >= length) {
    //pause();
    //player_->setTime(length - 1);
    stop();
    DOUT("seek: exit: time >= length, length =" << length << ", time =" << time);
    return;
  }

  player_->setTime(time);
  emit seeked();

  logger_->logSeeked(time);
  DOUT("seek: exit: time =" << time);
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

    qreal v_old = player_->volume();
    qreal v_new = delta + v_old;
    if (v_new <= 0) {
      if (v_old > 0)
        player_->setVolume(0);
    } else if (v_new >= 1) {
      if (v_old < 1)
        player_->setVolume(1);
    } else
      player_->setVolume(v_new);
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
    osdPlayer_->setVisible(!miniPlayer_->isVisible());
    break;

  case NormalPlayMode:
    osdPlayer_->hide();
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
      || osdPlayer_->isVisible()
      || miniPlayer_->isVisible();
}

PlayerUi*
MainWindow::visiblePlayer()
{
  return mainPlayer_->isVisible()? static_cast<PlayerUi*>(mainPlayer_)
       : osdPlayer_->isVisible() ? static_cast<PlayerUi*>(osdPlayer_)
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
    osdPlayer_->setVisible(!visible);
    miniPlayer_->setVisible(visible);
    break;
  }
}
*/

// - Help -

void
MainWindow::about()
{
  // TODO
  static QString msg = QString("%1 %2 %3\n%4\n%5\n%6")
      .arg(G_ORGANIZATION)
      .arg(G_APPLICATION)
      .arg(G_VERSION)
      .arg(G_HOMEPAGE)
      .arg(G_UPDATEPAGE)
      .arg(G_EMAIL);
  QMessageBox::about(this, TR(T_TITLE_ABOUT), msg);
}

// TODO
void
MainWindow::help()
{ about(); }

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
MainWindow::syncLineEditText(const QString &text)
{
  if (!mainPlayer_->lineEdit()->hasFocus())
    mainPlayer_->lineEdit()->setText(text);
  if (!miniPlayer_->lineEdit()->hasFocus())
    miniPlayer_->lineEdit()->setText(text);
  if (!osdPlayer_->lineEdit()->hasFocus())
    osdPlayer_->lineEdit()->setText(text);
}

void
MainWindow::syncPrefixLineEditText(const QString &text)
{
  if (!mainPlayer_->prefixLineEdit()->hasFocus())
    mainPlayer_->prefixLineEdit()->setText(text);
  if (!miniPlayer_->prefixLineEdit()->hasFocus())
    miniPlayer_->prefixLineEdit()->setText(text);
  if (!osdPlayer_->prefixLineEdit()->hasFocus())
    osdPlayer_->prefixLineEdit()->setText(text);
}

// - Dialogs -

void
MainWindow::showUserPanel()
{ setUserPanelVisible(true); }

void
MainWindow::hideUserPanel()
{ setUserPanelVisible(false); }

void
MainWindow::setUserPanelVisible(bool visible)
{ setLoginDialogVisible(visible); }

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
{ liveDialog_->setVisible(visible); }

void
MainWindow::showSyncDialog()
{ setSyncDialogVisible(true); }

void
MainWindow::hideSyncDialog()
{ setSyncDialogVisible(false); }

void
MainWindow::setSyncDialogVisible(bool visible)
{ syncDialog_->setVisible(visible); }

void
MainWindow::showCommentView()
{ setCommentViewVisible(true); }

void
MainWindow::hideCommentView()
{ setCommentViewVisible(false); }

void
MainWindow::setCommentViewVisible(bool visible)
{
  if (visible && tokenView_->hasToken())
    commentView_->setTokenId(tokenView_->token().id());
  commentView_->setVisible(visible);
}

void
MainWindow::setWindowPickDialogVisible(bool visible)
{ windowPickDialog_->setVisible(visible); }

void
MainWindow::setProcessPickDialogVisible(bool visible)
{ processPickDialog_->setVisible(visible); }

// - Annotations -

void
MainWindow::submitAlias(const Alias &input, bool async)
{
  DOUT("submitAlias:enter: async =" << async << ", text =" << input.text());
  if (async) {
    log(tr("connecting server to submit alias ..."));
    QThreadPool::globalInstance()->start(new task_::submitAlias(input, this));
    DOUT("submitText:exit: returned from async branch");
    return;
  }

  DOUT("submitAlias: mutex locking");
  mutex_.lock();
  DOUT("submitAlias: mutex locked");

  Alias a = input;
  a.setType(hub_->tokenType());

  bool id = server_->submitAlias(a);
  if (id)
    log(tr("alias saved") + ": " + a.text());
  else
    warn(tr("failed to update alias text") + ": " + a.text());

  DOUT("submitAlias: mutex unlocking");
  mutex_.unlock();
  DOUT("submitAlias: mutex unlocked");
  DOUT("submitAlias:exit");
}

void
MainWindow::updateAnnotationTextWithId(const QString &text, qint64 id)
{
  bool ok = server_->updateAnnotationTextWithId(text, id, hub_->tokenType());
  if (ok)
    log(tr("annotation saved") + ": " + text);
  else
    warn(tr("failed to update annotation text") + ": " + text);
}

void
MainWindow::setToken(const QString &input, bool async)
{
  DOUT("setToken:enter: async =" << async << ", path =" << input);
  if (async) {
    log(tr("connecting server to query media/game token ..."));
    QThreadPool::globalInstance()->start(new task_::setToken(input, this));
    DOUT("setToken:exit: returned from async branch");
    return;
  }

  DOUT("setToken: mutex locking");
  mutex_.lock();
  DOUT("setToken: mutex locked");

  tokenView_->clearToken();

  // Always assume path pointed a local file.
  // TODO: Add support for token with remote URL by streaming
  // Following logic is messed up.
  QString path = input;
  QString digest = Token::digestFromFile(path);
  if (digest.isEmpty()) {
    QString localPath = QUrl(path).toLocalFile();
    QString decodedLocalPath = QUrl::fromPercentEncoding(localPath.toAscii());
    path = decodedLocalPath;

    digest = Token::digestFromFile(path);
    if (digest.isEmpty()) {
      warn(TR(T_ERROR_HASH_TOKEN) + ": " + path);
      DOUT("setToken: mutex unlocking");
      mutex_.unlock();
      DOUT("setToken: mutex unlocked");
      DOUT("setToken:exit: empty digest skipped");
      return;
    }
  }

  int tt = hub_->tokenType();
  Token token;
  token.setType(tt);
  token.setDigest(digest);
  token.setCreateTime(QDateTime::currentMSecsSinceEpoch() / 1000);

  QString fileName = QFileInfo(path).fileName();

  if (fileName.size() > Traits::MAX_ANNOT_LENGTH) {
    fileName = fileName.mid(0, Traits::MAX_ALIAS_LENGTH);
    warn(TR(T_WARNING_LONG_STRING_TRUNCATED) + ": " + fileName);
  }

  Alias alias;
  alias.setType(tt);
  alias.setAliasType(Alias::AT_Source);
  alias.setLanguage(server_->user().language());
  alias.setText(fileName);
  alias.setUpdateTime(QDateTime::currentMSecsSinceEpoch() / 1000);

  AliasList aliases;

  qint64 tid = dataManager_->submitTokenAndAlias(token, alias);
  if (tid) {
    token = dataManager_->selectTokenWithId(tid, tt);
    if (token.isValid())
      aliases = dataManager_->selectAliasesWithTokenId(tid, tt);
    else
      warn(TR(T_ERROR_SUBMIT_TOKEN) + ": " + input);
  }

  if (!token.isValid() && cache_->isValid()) {
    log(tr("searching for token in cache ..."));
    token = cache_->selectTokenWithDigest(digest);
  }

  aliases = dataManager_->selectAliasesWithToken(token);

  tokenView_->setToken(token);
  commentView_->setTokenId(token.id());
  tokenView_->setAliases(aliases);
#ifdef USE_MODE_SIGNAL
  //signalView_->tokenView()->setAliases(aliases);
#endif // USE_MODE_SIGNAL

  AnnotationList annots = dataManager_->selectAnnotationsWithToken(token);
  annotationBrowser_->setAnnotations(annots);
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

  DOUT("setToken: mutex unlocking");
  mutex_.unlock();
  DOUT("setToken: mutex unlocked");
  DOUT("setToken:exit");
}

void
MainWindow::showText(const QString &text)
{
  Annotation annot;
  if (server_->isAuthorized()) {
    annot.setUserId(server_->user().id());
    annot.setUserAlias(server_->user().name());
    annot.setUserAnonymous(server_->user().isAnonymous());
  }
  annot.setLanguage(Traits::AnyLanguage);
  annot.setText(text);
  annotationView_->showAnnotation(annot);
}

void
MainWindow::submitText(const QString &text, bool async)
{
  DOUT("submitText:enter: async =" << async << ", text =" << text);
  if (!server_->isAuthorized() ||
      !tokenView_->hasToken() ||
      hub_->isMediaTokenMode() && !player_->hasMedia()
#ifdef USE_MODE_SIGNAL
      || hub_->isSignalTokenMode() && !messageHandler_->lastMessageHash().isValid()
#endif // USE_MODE_SIGNAL
     ) {
    showText(text);
    DOUT("submitText:exit: returned from showText branch");
    return;
  }

  if (async) {
    log(tr("connecting server to submit annot ..."));
    QThreadPool::globalInstance()->start(new task_::submitText(text, this));
    DOUT("submitText:exit: returned from async branch");
    return;
  }

  DOUT("submitText: mutex locking");
  mutex_.lock();
  DOUT("submitText: mutex locked");

  Annotation annot;
  annot.setTokenId(tokenView_->token().id());
  annot.setUserId(server_->user().id());
  annot.setUserAlias(server_->user().name());
  annot.setUserAnonymous(server_->user().isAnonymous());
  annot.setLanguage(server_->user().language());
  annot.setCreateTime(QDateTime::currentMSecsSinceEpoch() / 1000);
  switch (hub_->tokenMode()) {
  case SignalHub::MediaTokenMode:
    annot.setPos(player_->time());
    break;
  case SignalHub::SignalTokenMode:
#ifdef USE_MODE_SIGNAL
    annot.setPos(messageHandler_->lastMessageHash().hash);
    annot.setPosType(messageHandler_->lastMessageHash().count);
#endif // USE_MODE_SIGNAL
    break;
  }

  annot.setType(hub_->tokenType());

  if (text.size() <= Traits::MAX_ANNOT_LENGTH)
    annot.setText(text);
  else {
    annot.setText(text.mid(0, Traits::MAX_ANNOT_LENGTH));
    warn(TR(T_WARNING_LONG_STRING_TRUNCATED) + ": " + annot.text());
  }

  qint64 id = dataManager_->submitAnnotation(annot);
  if (id)
    annot.setId(id);
  else
    warn(TR(T_ERROR_SUBMIT_ANNOTATION) + ": " + text);

  annotationBrowser_->addAnnotation(annot);
  //annotationView_->addAndShowAnnotation(annot);
  emit addAndShowAnnotationRequested(annot);

  DOUT("submitText: mutex unlocking");
  mutex_.unlock();
  DOUT("submitText: mutex unlocked");
  DOUT("submitText: exit");
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
  DOUT("chat:enter: async =" << async << ", text =" << input);
  QString text = input.trimmed();
  if (text.isEmpty()) {
    DOUT("chat:exit: empty text skipped");
    return;
  }

  if (async) {
    //log(tr("connecting server to submit chat text ..."));
    QThreadPool::globalInstance()->start(new task_::chat(input, this));
    DOUT("chat:exit: returned from async branch");
    return;
  }

  DOUT("chat: mutex locking");
  mutex_.lock();
  DOUT("chat: mutex locked");

  emit showTextRequested(CORE_CMD_COLOR_BLUE " " + text);
  emit said(text, "blue");

#ifdef USE_MODULE_DOLL
  Q_ASSERT(doll_);
  doll_->chat(text);
#else
  if (server_->isConnected())
    emit responded(server_->chat(text));
#endif // USE_MODULE_DOLL

  DOUT("chat: mutex unlocking");
  mutex_.unlock();
  DOUT("chat: mutex unlocked");
  DOUT("chat:exit");
}

void
MainWindow::respond(const QString &text)
{
  showText(CORE_CMD_COLOR_PURPLE " " + text);
  say(text, "purple");
}

void
MainWindow::say(const QString &text, const QString &color)
{
  if (text.isEmpty())
    return;

  gConsole() << core_html_style(text, "color:" + color) << core_html_br();
}

// - Mouse events -

void
MainWindow::mousePressEvent(QMouseEvent *event)
{
#ifdef Q_WS_MAC
  if (hub_->isFullScreenVideoMode()) {
    // Prevent auto hide osd player.
    // Since NSView::mouseMoved is not avaible, use mouseDown to prevent hiding instead.
    if (!hasVisiblePlayer()) {
      osdPlayer_->show();
      osdDock_->raise();
    } else if (osdPlayer_->isVisible())
      osdPlayer_->resetAutoHideTimeout();

    // Alternative to windows hook:
    //if (isPlaying())
    //  pause();
  }

#endif // Q_WS_MAC

  if (!event)
    return;

  switch (event->button()) {
  case Qt::LeftButton:
    if (!isFullScreen() && dragPos_ == BAD_POS) {
      dragPos_ = event->globalPos() - frameGeometry().topLeft();
      event->accept();
    } break;

  case Qt::MiddleButton:
    hub_->toggleMiniPlayerMode();
    event->accept();
    break;

  //case Qt::RightButton:
  //  open();
  //  event->accept();
  //  break;

  default: break;
  }
}

bool
MainWindow::isGlobalPosNearOSDPlayer(const QPoint &pos) const
{
  QRect r = annotationView_->globalRect();
  return pos.x() < 6 || pos.x() > osdWindow_->width() - 6 ||  // Near the border
         pos.y() < 6 || pos.y() > osdWindow_->height() - 6 || // Near the border

         pos.x() < r.right() + 6 && pos.x() > r.left() - 6 &&
         pos.y() < r.bottom() + 6 && pos.y() > r.top() + r.height() / 4 * 3 - 6;
}

void
MainWindow::mouseMoveEvent(QMouseEvent *event)
{
  // Prevent auto hide osd player.
  if (hub_->isFullScreenPlayerMode())
    if (!event || isGlobalPosNearOSDPlayer(event->globalPos())) {
      if (!hasVisiblePlayer())
        osdPlayer_->show();
      else if (osdPlayer_->isVisible())
        osdPlayer_->resetAutoHideTimeout();
    }

  // Move the main window
  if (event && event->buttons() & Qt::LeftButton
      && dragPos_ != BAD_POS && !isFullScreen()) {
    QPoint newPos = event->globalPos() - dragPos_;
    // use QApplication::postEvent is more elegant but less efficient
    move(newPos);

    annotationView_->invalidatePos();
//#ifdef Q_WS_MAC // FIXME: solve the postion tracking problem....
//  QTimer::singleShot(200, annotationView_, SLOT(invalidatePos()));
//#endif // Q_WS_MAC

    event->accept();
  }
}

void
MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
  dragPos_ = BAD_POS;

#ifdef Q_WS_MAC
  //annotationView_->invalidatePos();

  //if (event && event->button() == Qt::RightButton) {
  //  QContextMenuEvent cm(QContextMenuEvent::Mouse, event->pos(), event->globalPos(), event->modifiers());
  //  //QApplication::sendEvent(this, &cm);
  //  contextMenuEvent(&cm);
  //  return;
  //}
#endif // Q_WS_MAC

  if (event)
    event->accept();
}

void
MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
  if (!event)
    return;

  switch (event->button()) {
  case Qt::LeftButton: hub_->toggleFullScreenVideoMode(); event->accept(); break;
  default: break;
  }
}

void
MainWindow::wheelEvent(QWheelEvent *event)
{
  if (event && event->orientation() == Qt::Vertical) {
    qreal vol = player_->volume();
    vol += event->delta() / (8 * 360.0);
    vol = (vol > 1)? 1 : (vol < 0)? 0 : vol;
    player_->setVolume(vol);
    event->accept();
  }
}

void
MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
  if (event) {
    invalidateContextMenu();
    contextMenu_->popup(event->globalPos());
    event->accept();
  }
}

void
MainWindow::invalidateContextMenu()
{
  contextMenu_->clear();

  // Open
  {
    //contextMenu_->addSeparator();

    //contextMenu_->addAction(openAct_);
    contextMenu_->addAction(openFileAct_);

#ifdef USE_MODE_SIGNAL
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
#endif // USE_MODE_SIGNAL

    if (openContextMenu_->isEmpty()) {
      // only add once
      openContextMenu_->addAction(openSubtitleAct_);
      openContextMenu_->addAction(openDeviceAct_);
      //openContextMenu_->addAction(openFileAct_);
    }
    contextMenu_->addMenu(openContextMenu_);

    // Recent
    if (!recent_.isEmpty()) {
      recentMenu_->clear(); {

      QList<QAction*> l;
#define ADD_RECENT(_i) \
        if (recent_.size() > _i) { \
          QString path = recent_[_i]; \
          QFileInfo fi(path); \
          if (fi.exists()) { \
            recent##_i##Act_->setText(fi.fileName()); \
            recent##_i##Act_->setToolTip(path); \
            l.prepend(recent##_i##Act_); \
          } else \
            recent_.removeAt(_i); \
        }

        ADD_RECENT(9) ADD_RECENT(8) ADD_RECENT(7)
        ADD_RECENT(6) ADD_RECENT(5) ADD_RECENT(4)
        ADD_RECENT(3) ADD_RECENT(2) ADD_RECENT(1)
        ADD_RECENT(0)
#undef ADD_RECENT
        recentMenu_->addActions(l);
        recentMenu_->addSeparator();
        recentMenu_->addAction(clearRecentAct_);
      }

      contextMenu_->addMenu(recentMenu_);
    }
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

#ifdef USE_WIN_PICKER
     toggleWindowPickDialogVisibleAct_->setChecked(windowPickDialog_->isVisible());
     contextMenu_->addAction(toggleWindowPickDialogVisibleAct_);
#endif // USE_WIN_PICKER

     contextMenu_->addMenu(annotationLanguageContextMenu_);

    // Subtitle menu
    if (hub_->isMediaTokenMode() && player_->hasMedia()) {
      contextMenu_->addSeparator();

      subtitleContextMenu_->clear();
      if (player_->hasSubtitles()) {
        if (player_->isSubtitleVisible()) {
          toggleSubtitleVisibleAct_->setChecked(true);
          toggleSubtitleVisibleAct_->setText(TR(T_MENUTEXT_SHOWSUBTITLE));
        } else {
          toggleSubtitleVisibleAct_->setChecked(false);
          toggleSubtitleVisibleAct_->setText(TR(T_MENUTEXT_HIDESUBTITLE));
        }
        contextMenu_->addAction(toggleSubtitleVisibleAct_);

        subtitleContextMenu_->addSeparator();

        QStringList l = player_->subtitleDescriptions();
        int id = 0;
        foreach (const QString &subtitle, l) {
          BOOST_AUTO(action, new Core::Gui::ActionWithId(id, subtitle, subtitleContextMenu_));
          if (id == player_->subtitleId())
            action->setIcon(QIcon(RC_IMAGE_CURRENTSUBTITLE));
          connect(action, SIGNAL(triggeredWithId(int)), player_, SLOT(setSubtitleId(int)));
          subtitleContextMenu_->addAction(action);
          id++;
        }

        subtitleContextMenu_->addSeparator();
      }
      subtitleContextMenu_->addAction(openSubtitleAct_);
      contextMenu_->addMenu(subtitleContextMenu_);
    }
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

    if (hub_->isMediaTokenMode())
      contextMenu_->addAction(nextFrameAct_);

    if (hub_->isSignalTokenMode() && !hub_->isStopped() ||
        hub_->isMediaTokenMode() && !player_->isStopped())
    contextMenu_->addAction(stopAct_);

    if (hub_->isMediaTokenMode())
      contextMenu_->addAction(replayAct_);

    if (hub_->isMediaTokenMode() && !player_->isStopped() ||
        hub_->isSignalTokenMode())
      contextMenu_->addAction(snapshotAct_);
  }

  // Forward/backward
  if (hub_->isMediaTokenMode() && player_->hasMedia()) {
    contextMenu_->addSeparator();

    contextMenu_->addMenu(backwardContextMenu_);
    contextMenu_->addMenu(forwardContextMenu_);

    toggleSeekDialogVisibleAct_->setChecked(seekDialog_->isVisible());
    contextMenu_->addAction(toggleSeekDialogVisibleAct_);
  }

  // Toggles
  {
    contextMenu_->addSeparator();

    toggleFullScreenModeAct_->setChecked(hub_->isFullScreenVideoMode());
    contextMenu_->addAction(toggleFullScreenModeAct_);

    toggleMiniModeAct_->setChecked(hub_->isMiniPlayerMode());
    contextMenu_->addAction(toggleMiniModeAct_);

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
  {
    contextMenu_->addSeparator();

    if (server_->isAuthorized()) {
      contextMenu_->addAction(logoutAct_);

      if (ALPHA) {
        toggleUserPanelVisibleAct_->setChecked(loginDialog_->isVisible());
        contextMenu_->addAction(toggleUserPanelVisibleAct_);

        if (server_->isConnected())
          contextMenu_->addAction(disconnectAct_);
        else
          contextMenu_->addAction(connectAct_);
      }

      toggleUserAnonymousAct_->setChecked(server_->user().isAnonymous());
      contextMenu_->addAction(toggleUserAnonymousAct_);

      contextMenu_->addMenu(userLanguageContextMenu_); {
        setUserLanguageToEnglishAct_->setChecked(server_->user().language() == Traits::English);
        setUserLanguageToJapaneseAct_->setChecked(server_->user().language() == Traits::Japanese);
        setUserLanguageToChineseAct_->setChecked(server_->user().language() == Traits::Chinese);
        setUserLanguageToKoreanAct_->setChecked(server_->user().language() == Traits::Korean);
        setUserLanguageToUnknownAct_->setChecked(server_->user().language() == Traits::UnknownLanguage);
      }

    } else
      contextMenu_->addAction(loginAct_);
  }

  // Toggles
  {
    //if (!isFullScreenMode()) {
    //  contextMenu_->addSeparator();
    //  contextMenu_->addAction(toggleWindowStaysOnTopAct_);
    //}
  }

  // Language
  if (ALPHA) {
    contextMenu_->addSeparator();
    contextMenu_->addMenu(appLanguageContextMenu_);

    int l = Translator::globalInstance()->language();
    setAppLanguageToJapaneseAct_->setChecked(l == QLocale::Japanese);
    setAppLanguageToTraditionalChineseAct_->setChecked(l == Translator::TraditionalChinese);
    setAppLanguageToSimplifiedChineseAct_->setChecked(l == QLocale::Chinese);

    switch (l) {
    case QLocale::Japanese: case QLocale::Taiwan: case QLocale::Chinese:
      setAppLanguageToEnglishAct_->setChecked(false);
      break;
    case QLocale::English: default:
      setAppLanguageToEnglishAct_->setChecked(true);
    }
  }

  // Help
  {
    contextMenu_->addSeparator();

    if (ALPHA)
      contextMenu_->addAction(helpAct_);
    contextMenu_->addAction(aboutAct_);
    contextMenu_->addAction(quitAct_);
  }
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
  // Enable full screen since it is buggy.
  //if (event &&
  //    event->type() == QEvent::WindowStateChange &&
  //    windowState() == Qt::WindowMaximized)
  //  hub_->setFullScreenVideoMode();
  //else
    Base::changeEvent(event);
}

// - Keyboard events -

void
MainWindow::keyPressEvent(QKeyEvent *event)
{
  switch (event->key()) {

    //if (!hasVisiblePlayer() || !visiblePlayer()->lineEdit()->hasFocus())
  case Qt::Key_Escape: hub_->toggleMiniPlayerMode(); break;
  case Qt::Key_Return: hub_->toggleFullScreenVideoMode(); break;

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
    if (hub_->isFullScreenVideoMode()) { // OSD mode
      if (!hasVisiblePlayer())
        osdPlayer_->show();
      else if (osdPlayer_->isVisible())
        osdPlayer_->resetAutoHideTimeout();
    }
    if (hasVisiblePlayer()
        && !visiblePlayer()->lineEdit()->hasFocus()) {
#ifdef USE_WIN_QTWIN
      QtWin::setFocus(visiblePlayer()->lineEdit());
#else
      visiblePlayer()->lineEdit()->setFocus();
#endif // USE_WIN_QTWIN
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
  Q_ASSERT(event);
  if (isMimeDataSupported(event->mimeData()))
    event->acceptProposedAction();
}

void
MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
  Q_ASSERT(event);
  if (isMimeDataSupported(event->mimeData()))
    event->acceptProposedAction();
}

void
MainWindow::dropEvent(QDropEvent *event)
{
  Q_ASSERT(event);
  if (isMimeDataSupported(event->mimeData())) {
    setFocus();
    event->acceptProposedAction();
    openMimeData(event->mimeData());
  }
}

void
MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{ event->accept(); }

// - Close -
void
MainWindow::closeEvent(QCloseEvent *event)
{
  DOUT("closeEvent:enter");
  respond(tr("yin: ..."));
  log("closing application ...");

  //hide();
  //if (parentWidget())
  //  parentWidget()->hide();

#ifdef USE_MODE_SIGNAL
  QTH->clear();
#endif // USE_MODE_SIGNAL

  tray_->hide();

  Settings::globalInstance()->setRecent(recent_);

  // Disabled for saving closing time orz
  //if (server_->isConnected() && server_->isAuthorized())
  //  server_->logout();

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
    osdDock_->hide();
    annotationView_->hide();
    globalOsdConsole_->hide();
    //if (parentWidget())
    //  parentWidget()->hide();
    QThreadPool::globalInstance()->waitForDone();
  }

  if (parentWidget()) {
    //parentWidget()->hide();
    parentWidget()->close();
  }

  Base::closeEvent(event);
  DOUT("closeEvent:exit");
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

    // FIXME: correctly raise OSDWindow!!!

    if (visible)
      show();
  }
}
*/

// - Synchronize -

void
MainWindow::logout(bool async)
{
  DOUT("logout:enter: async =" << async);
  if (async) {
    log(tr("connecting server to logout ..."));
    QThreadPool::globalInstance()->start(new task_::logout(this));
    DOUT("logout:exit: returned from async branch");
    return;
  }

  DOUT("logout: mutex locking");
  mutex_.lock();
  DOUT("logout: mutex locked");

  annotationBrowser_->setUserId(0);
  server_->logout();

  DOUT("logout: mutex unlocking");
  mutex_.unlock();
  DOUT("logout: mutex unlocked");
  DOUT("logout:exit: async =" << async);
}

void
MainWindow::login(const QString &userName, const QString &encryptedPassword, bool async)
{
  DOUT("login:enter: async =" << async << ", userName =" << userName);
  if (async) {
    QThreadPool::globalInstance()->start(new task_::login(userName, encryptedPassword, this));
    DOUT("login:exit: returned from async branch");
    return;
  }

  DOUT("login: mutex locking");
  mutex_.lock();
  DOUT("login: mutex locked");
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
    tokenView_->setUserId(uid);
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
  }

  if (server_->isConnected() && server_->isAuthorized()) {
    bool updated = server_->isSoftwareUpdated();
    if (!updated)
      warn(tr("new version released, check here: ") + G_UPDATEPAGE_URL);
  }

  dataManager_->commitQueue();

  DOUT("login: mutex unlocking");
  mutex_.unlock();
  DOUT("login: mutex unlocked");
  DOUT("login:exit");
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
#ifdef USE_WIN_QTWIN
  ulong pid = QtWin::getWindowProcessId(hwnd);
  if (pid) {
    log(tr("found process id for window") + QString(" (pid = %1)").arg(QString::number(pid)));
    openProcessId(pid);
  } else
    warn(tr("process id for window was not found"));
#endif USE_WIN_QTWIN
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
  if (!pid)
    warn(tr("failed to start process") + QString(" (name = %1)").arg(processName));
  else
    openProcessId(pid);
  signalView_->processView()->refresh();
  openProcess();
  addRecent(path);
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

    if (hookId != messageHandler_->hookId())
      messageHandler_->clearRecentMessages();

    messageHandler_->setProcessInfo(pi);
    messageHandler_->setHookId(hookId);
  }

  hub_->setSignalTokenMode();
  if (pi.isValid())
    setToken(pi.executablePath);

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
}

#endif // USE_MODE_SIGNAL

// - User -

void
MainWindow::setUserAnonymous(bool t, bool async)
{
  DOUT("setUserAnonymous:enter: async =" << async);

  if (server_->isConnected() && server_->isAuthorized()) {

    if (async) {
      log(tr("connecting server to change anonymous status ..."));
      QThreadPool::globalInstance()->start(new task_::setUserAnonymous(t, this));
      DOUT("setUserAnonymous:exit: returned from async branch");
      return;
    }

    DOUT("setUserAnonymous: mutex locking");
    mutex_.lock();
    DOUT("setUserAnonymous: mutex locked");

    bool ok = server_->setUserAnonymous(t);
    if (!ok)
      warn(tr("failed to change user anonymous state"));

    DOUT("setUserAnonymous: mutex unlocking");
    mutex_.unlock();
    DOUT("setUserAnonymous: mutex unlocked");

  } else
    server_->user().setAnonymous(t);

  if (server_->user().isAnonymous())
    log(tr("you are anonymous now"));
  else
    log(tr("you are not anonymous now"));

  DOUT("setUserAnonymous:exit");
}

void
MainWindow::setUserLanguage(int lang, bool async)
{
  DOUT("setUserLanguage:enter: async =" << async);

  if (server_->isConnected() && server_->isAuthorized()) {

    if (async) {
      log(tr("connecting server to change language ..."));
      QThreadPool::globalInstance()->start(new task_::setUserLanguage(lang, this));
      DOUT("setUserLanguage:exit: returned from async branch");
      return;
    }

    DOUT("setUserLanguage: mutex locking");
    mutex_.lock();
    DOUT("setUserLanguage: mutex locked");

    bool ok = server_->setUserLanguage(lang);
    if (!ok)
      warn(tr("failed to change user language"));

    DOUT("setUserLanguage: mutex unlocking");
    mutex_.unlock();
    DOUT("setUserLanguage: mutex unlocked");

  } else
    server_->user().setLanguage(lang);

  log(tr("your language is ") + languageToString(server_->user().language()));

  DOUT("setUserLanguage:exit");
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

    if (toggleAnnotationLanguageToEnglishAct_->isChecked())       languages |= Traits::EnglishBit;
    if (toggleAnnotationLanguageToJapaneseAct_->isChecked())      languages |= Traits::JapaneseBit;
    if (toggleAnnotationLanguageToChineseAct_->isChecked())       languages |= Traits::ChineseBit;
    if (toggleAnnotationLanguageToKoreanAct_->isChecked())        languages |= Traits::KoreanBit;
  }


  annotationView_->setLanguages(languages);
  Settings::globalInstance()->setAnnotationLanguages(languages);
}

// - Language -

void
MainWindow::setAppLanguageToEnglish()
{ Translator::globalInstance()->setLanguage(QLocale::English); }

void
MainWindow::setAppLanguageToJapanese()
{ Translator::globalInstance()->setLanguage(QLocale::Japanese); }

// TODO: Simplified Chinese and Traditional Chinese are not differed until qt 4.8
void
MainWindow::setAppLanguageToTraditionalChinese()
{ Translator::globalInstance()->setLanguage(Translator::TraditionalChinese); }

void
MainWindow::setAppLanguageToSimplifiedChinese()
{ Translator::globalInstance()->setLanguage(QLocale::Chinese); }

void
MainWindow::invalidateAppLanguage()
{ log(tr("hot plug unimplemented; please restart the app to use the new language")); }

// - Helpers -

QString
MainWindow::languageToString(int lang)
{
  switch(lang) {
  case Traits::AnyLanguage:     return tr("Any");
  case Traits::English:         return tr("English");
  case Traits::Japanese:        return tr("Japanese");
  case Traits::Chinese:         return tr("Chinese");
  case Traits::Korean:          return tr("Korean");

  case Traits::UnknownLanguage:
  default : return tr("Alien");
  }
}

// - Recent -

void
MainWindow::clearRecent()
{ recent_.clear(); }

void
MainWindow::openRecent(const QString &path)
{ openSource(path); }

void
MainWindow::openSource(const QString &path)
{
  if (path.isEmpty())
    return;

  QFileInfo fi(path);
  if (!fi.exists()) {
    warn(tr("file does not exist") + ": " + path);
    return;
  }

#ifdef USE_WIN_QTWIN
  if (fi.suffix().compare("lnk", Qt::CaseInsensitive) == 0) {
    QString lnk = QtWin::resolveLink(path, winId());
    if (!lnk.isEmpty()) {
      openSource(lnk);
      addRecent(path);
    } else
      warn(tr("invalid lnk") + ": " + path );
  } else
#endif // USE_WIN_QTWIN
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
  int i = recent_.indexOf(path);
  if (i >= 0)
    recent_.removeAll(path);
  recent_.prepend(path);
}

void MainWindow::openRecent0() { if (recent_.size() > 0) openRecent(recent_[0]); }
void MainWindow::openRecent1() { if (recent_.size() > 1) openRecent(recent_[1]); }
void MainWindow::openRecent2() { if (recent_.size() > 2) openRecent(recent_[2]); }
void MainWindow::openRecent3() { if (recent_.size() > 3) openRecent(recent_[3]); }
void MainWindow::openRecent4() { if (recent_.size() > 4) openRecent(recent_[4]); }
void MainWindow::openRecent5() { if (recent_.size() > 5) openRecent(recent_[5]); }
void MainWindow::openRecent6() { if (recent_.size() > 6) openRecent(recent_[6]); }
void MainWindow::openRecent7() { if (recent_.size() > 7) openRecent(recent_[7]); }
void MainWindow::openRecent8() { if (recent_.size() > 8) openRecent(recent_[8]); }
void MainWindow::openRecent9() { if (recent_.size() > 9) openRecent(recent_[9]); }

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
