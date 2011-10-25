#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// mainwindow.h
// 6/30/2011

#include "core/data/alias.h"
#ifdef USE_MODE_SIGNAL
  #include "processinfo.h"
#endif // USE_MODE_SIGNAL
#include <QMainWindow>
#include <QStringList>

QT_FORWARD_DECLARE_CLASS(QUrl)
QT_FORWARD_DECLARE_CLASS(QMimeData)

// Objects
class ClientAgent;
class Database;
class DataManager;
class EventLogger;
class Grabber;
class Player;
class ServerAgent;
class Settings;
class SignalHub;

// Views
class AnnotationGraphicsView;
class AnnotationBrowser;
class AnnotationEditor;
class CommentView;
class MainPlayerUi;
class MessageHandler;
class MiniPlayerUi;
//class MiniPlayerDock;
class OSDDock;
class OSDWindow;
class OSDConsole;
class OSDPlayerUi;
class PlayerUi;
class MessageView;
class SignalView;
class TokenView;
class TrendView;
class UserPanel;
class UserPanelDock;
class VideoView;

// Dialogs
class LiveDialog;
class LoginDialog;
class SeekDialog;
class SyncDialog;

// Hook
#ifdef USE_WIN_HOOK
#include "win/hook/hook.h"

class MainWindow;
class HookEventForwarder : public QObject
{
  Q_OBJECT
  typedef HookEventForwarder Self;
  typedef QObject Base;

  MainWindow *w_;

public:
  explicit HookEventForwarder(MainWindow *w, QObject *parent = 0);

protected:
  /// \override  Hook event filter.
  virtual bool eventFilter(QObject *hook, QEvent *event);
};
#endif // USE_WIN_HOOK

// MainWindow
class MainWindow : public QMainWindow
{
  Q_OBJECT
  typedef MainWindow Self;
  typedef QMainWindow Base;

  typedef Core::Data::Alias Alias;

  // - Helpers
public:
  static bool isMimeDataSupported(const QMimeData *urls);
  static bool isDevicePath(const QString &path);

  // - Constructions -
public:
  explicit MainWindow(QWidget *parent = 0, Qt::WindowFlags f = 0);
  bool isValid() const;
  void parseArguments(const QStringList &args);

  // - Signals -
signals:
  void seeked();

  // - States -
public:
  bool isPlaying() const;
  bool isEditing() const;

  bool hasVisiblePlayer() const;
  PlayerUi *visiblePlayer(); // One of three player Uis, or nullptr.

  bool commandLineEditHasFocus() const;
  bool prefixLineEditHasFocus() const;

public slots:

  void onFocusedWidgetChanged(QWidget *w_old, QWidget *w_new);

  void about();
  void help();

  // PlayMode
  void login(const QString &userName, const QString &encryptedPassword);
  void logout();
  void sync();  ///< Synchronize offline queue

  void openSource(const QString &source);
  void open();  ///< By default the same as openFile()
  void openFile();
  void openProcess();
  void openDevice();
  void openSubtitle();
  void openDirectory();
  void openPath(const QString &path, bool checkPath = true); ///< \param checkPath determines if validate input \param path, such as existance
  void openUrl(const QUrl &url);
  void openUrls(const QList<QUrl> &urls);
  void openMimeData(const QMimeData *urls);
  void openSubtitlePath(const QString &path);
  void play();
  void playPause();
  void replay(); /// Restart from the beginning.
  void pause();
  void stop();
  void nextFrame();
  void snapshot();

  void seek(qint64 msecs);
  void forward(qint64 msecs = 0);  ///< If <=0, default delta time is used.
  void backward(qint64 msecs = 0); ///< If <=0, default delta time is used.

  void forward5s()      { forward(5 * 1000); }
  void backward5s()     { backward(5 * 1000); }
  void forward10s()     { forward(10 * 1000); }
  void backward10s()    { backward(10 * 1000); }
  void forward30s()     { forward(30 * 1000); }
  void backward30s()    { backward(30 * 1000); }
  void forward60s()     { forward(60 * 1000); }
  void backward60s()    { backward(60 * 1000); }
  void forward90s()     { forward(90 * 1000); }
  void backward90s()    { backward(90 * 1000); }
  void forward1m()      { forward(1 * 60000); }
  void backward1m()     { backward(1 * 60000); }
  void forward5m()      { forward(5 * 60000); }
  void backward5m()     { backward(5 * 60000); }
  void forward10m()     { forward(10 * 60000); }
  void backward10m()    { backward(10 * 60000); }

  void volumeUp(qreal delta = 0);  ///< If <=0, default delta percent is used.
  void volumeDown(qreal delta = 0);///< If <=0, default delta percent is used.

  //void about(); // TODO
  //void help(); // TODO

  void invalidateWindowTitle();
  void syncLineEditText(const QString &text);
  void syncPrefixLineEditText(const QString &text);

  //void setWindowStaysOnTop(bool enabled = true); // TODO

  // - Dialogs -
protected slots:
  void showUserPanel();
  void hideUserPanel();
  void setUserPanelVisible(bool visible);

  void showLoginDialog();
  void hideLoginDialog();
  void setLoginDialogVisible(bool visible);

  void showSeekDialog();
  void hideSeekDialog();
  void setSeekDialogVisible(bool visible);

  void showLiveDialog();
  void hideLiveDialog();
  void setLiveDialogVisible(bool visible);

  void showSyncDialog();
  void hideSyncDialog();
  void setSyncDialogVisible(bool visible);

  void showCommentView();
  void hideCommentView();
  void setCommentViewVisible(bool visible);

  // - User -
  void setUserAnonymous(bool t);
  void setUserLanguage(int lang);

  // - Languages -
protected slots:
  void setAppLanguageToEnglish();
  void setAppLanguageToJapanese();
  void setAppLanguageToTraditionalChinese();
  void setAppLanguageToSimplifiedChinese();
  void invalidateAppLanguage();

  void setUserLanguageToEnglish();
  void setUserLanguageToJapanese();
  void setUserLanguageToChinese();
  void setUserLanguageToKorean();
  void setUserLanguageToUnknown();

  void invalidateAnnotationLanguages();

  // - Annotations -
protected slots:
  void eval(const QString &cmd);
  void chat(const QString &text);
  void respond(const QString &text);
  void say(const QString &text, const QString &color = "blue");

  void submitText(const QString &text);
  void showText(const QString &text);

  void setToken(const QString &filePath);
  void submitAlias(const Alias &alias);

  void updateAnnotationTextWithId(const QString &text, qint64 id);

  // - Recent -
protected slots:
  void addRecent(const QString &path);
  void openRecent(const QString &path);
  void openRecent0(); void openRecent1(); void openRecent2();
  void openRecent3(); void openRecent4(); void openRecent5();
  void openRecent6(); void openRecent7(); void openRecent8();
  void openRecent9();
  void clearRecent();

  // - Events -
protected:
  virtual void mousePressEvent(QMouseEvent *event); ///< \override
  virtual void mouseMoveEvent(QMouseEvent *event); ///< \override
  virtual void mouseReleaseEvent(QMouseEvent *event); ///< \override
  virtual void mouseDoubleClickEvent(QMouseEvent *event); ///< \override
  virtual void wheelEvent(QWheelEvent *event); ///< \override

  virtual void moveEvent(QMoveEvent *event); ///< \override
  virtual void resizeEvent(QResizeEvent *event); ///< \override

  virtual void changeEvent(QEvent *event); ///< \override

  virtual void keyPressEvent(QKeyEvent *event); ///< \override

  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override

  virtual void closeEvent(QCloseEvent *event); ///< \override

protected slots:
  virtual void dragEnterEvent(QDragEnterEvent *event); ///< \override
  virtual void dragMoveEvent(QDragMoveEvent *event); ///< \override
  virtual void dragLeaveEvent(QDragLeaveEvent *event); ///< \override
  virtual void dropEvent(QDropEvent *event); ///< \override

protected:
  bool isPosNearOSDPlayer(const QPoint &pos) const;

public slots:
  void updatePlayMode();
  void updatePlayerMode();
  void updateTokenMode();
  void updateVideoMode();

  // - Signal mode -
#ifdef USE_WIN_QTH
protected slots:
  void openProcessPath(const QString &path);
  void openProcessHook(int hookId, const ProcessInfo &pi = ProcessInfo());
#endif // USE_WIN_QTH

#ifdef USE_MODE_SIGNAL
public slots:
  void showSignalView();
  void hideSignalView();
  void setSignalViewVisible(bool visible);

  void showRecentMessageView();
  void hideRecentMessageView();
  void setRecentMessageViewVisible(bool visible);

private:
  SignalView *signalView_;
  MessageView *recentMessageView_;
  MessageHandler *messageHandler_;

  QAction *toggleSignalViewVisibleAct_,
          *toggleRecentMessageViewVisibleAct_;
#endif // USE_MODE_SIGNAL

  // - Helpers -
  static QString languageToString(int lang);

  // - Members for initialization. -
private:
  void createComponents();
  void createActions();
  void createMenus();
  void createDockWindows();
  void createConnections();

  // - Attributes -
private:
  SignalHub *hub_;

  QStringList recent_;

#ifdef USE_MODULE_SERVERAGENT
  ServerAgent *server_;
#endif // USE_MODULE_SERVERAGENT
#ifdef USE_MODULE_CLIENTAGENT
  ClientAgent *client_;
#endif // USE_MODULE_CLIENTAGENT

  Player *player_;
  Settings *settings_;

  Database *cache_;
  Database *queue_;
  DataManager *dataManager_;

  EventLogger *logger_;
  OSDConsole *globalOsdConsole_;

  OSDDock *osdDock_;
  OSDWindow *osdWindow_;
  TokenView *tokenView_;
  VideoView *videoView_;

  MainPlayerUi *mainPlayer_;
  MiniPlayerUi *miniPlayer_;
  OSDPlayerUi *osdPlayer_;

  AnnotationGraphicsView *annotationView_;
  AnnotationBrowser *annotationBrowser_;
  AnnotationEditor *annotationEditor_;

  LoginDialog *loginDialog_;
  SeekDialog *seekDialog_;
  LiveDialog *liveDialog_;
  SyncDialog *syncDialog_;

  TrendView *trendView_;
  CommentView *commentView_;

  QPoint dragPos_;

  QString lastOpenedPath_;

  Grabber *grabber_;

  // - Menus and actions -

#ifdef Q_WS_MAC
  QMenu *fileMenu_,
        *viewMenu_,
        *helpMenu_;
#endif // Q_WS_MAC
  QMenu *contextMenu_,
        *advancedMenu_,
        *recentMenu_,
        *openContextMenu_,
        *subtitleContextMenu_,
        *backwardContextMenu_,
        *forwardContextMenu_,
        *appLanguageContextMenu_,
        *userLanguageContextMenu_,
        *annotationLanguageContextMenu_;

  QAction *openAct_,
          *openFileAct_,
          *openDeviceAct_,
          *openSubtitleAct_,
          *playAct_,
          *pauseAct_,
          *stopAct_,
          *replayAct_,
          *nextFrameAct_,
          *snapshotAct_,
          *toggleAnnotationVisibleAct_,
          *toggleFullScreenModeAct_,
          *toggleMiniModeAct_,
          *toggleLiveModeAct_,
          *toggleSyncModeAct_,
          *toggleSubtitleVisibleAct_;

  QAction *toggleWindowStaysOnTopAct_;

  QAction *loginAct_,
          *logoutAct_,
          *connectAct_,
          *disconnectAct_;

  QAction *clearCacheAct_;

  QAction *toggleAnnotationBrowserVisibleAct_,
          *toggleAnnotationEditorVisibleAct_,
          *toggleCommentViewVisibleAct_,
          *toggleTokenViewVisibleAct_,
          *toggleTrendViewVisibleAct_,
          *toggleLiveDialogVisibleAct_,
          *toggleLoginDialogVisibleAct_,
          *toggleSeekDialogVisibleAct_,
          *toggleSyncDialogVisibleAct_,
          *toggleUserPanelVisibleAct_;

  QAction *toggleUserAnonymousAct_;

  QAction *forward5sAct_,
          *backward5sAct_,
          *forward10sAct_,
          *backward10sAct_,
          *forward30sAct_,
          *backward30sAct_,
          *forward60sAct_,
          *backward60sAct_,
          *forward90sAct_,
          *backward90sAct_,
          *forward1mAct_,
          *backward1mAct_,
          *forward5mAct_,
          *backward5mAct_,
          *forward10mAct_,
          *backward10mAct_;

  QAction *recent0Act_, *recent1Act_, *recent2Act_,
          *recent3Act_, *recent4Act_, *recent5Act_,
          *recent6Act_, *recent7Act_, *recent8Act_,
          *recent9Act_,
          *clearRecentAct_;

  QAction *setAppLanguageToEnglishAct_,
          *setAppLanguageToJapaneseAct_,
          *setAppLanguageToTraditionalChineseAct_,
          *setAppLanguageToSimplifiedChineseAct_;

  QAction *setUserLanguageToEnglishAct_,
          *setUserLanguageToJapaneseAct_,
          *setUserLanguageToChineseAct_,
          *setUserLanguageToKoreanAct_,
          *setUserLanguageToUnknownAct_;

  QAction *toggleAnnotationLanguageToEnglishAct_,
          *toggleAnnotationLanguageToJapaneseAct_,
          *toggleAnnotationLanguageToChineseAct_,
          *toggleAnnotationLanguageToKoreanAct_,
          *toggleAnnotationLanguageToUnknownAct_,
          *toggleAnnotationLanguageToAnyAct_;

  QAction *helpAct_,
          *aboutAct_,
          *quitAct_;
};

#endif // MAINWINDOW_H
