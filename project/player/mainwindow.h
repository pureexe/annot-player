#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// mainwindow.h
// 6/30/2011

#include "config.h"
#include "core/cloud/alias.h"
#include "core/cloud/annotation.h"
#ifdef USE_MODE_SIGNAL
  #include "processinfo.h"
#endif // USE_MODE_SIGNAL
#include <QMainWindow>
#include <QUrl>
#include <QMutex>
#include <QStringList>
#include <QFileInfoList>

QT_FORWARD_DECLARE_CLASS(QUrl)
QT_FORWARD_DECLARE_CLASS(QMimeData)

// Objects
class AnnotationFilter;
class ClientAgent;
class Database;
class DataManager;
class DataServer;
class EventLogger;
class Grabber;
class Player;
class ServerAgent;
class SignalHub;
class Tray;
class Translator;

// Views
class AnnotationGraphicsView;
class AnnotationBrowser;
class AnnotationEditor;
class CloudView;
class CommentView;
class MainPlayerUi;
class MessageHandler;
class MiniPlayerUi;
//class MiniPlayerDock;
//class OsdDock;
class OsdWindow;
class OsdConsole;
class EmbeddedPlayerUi;
class PlayerUi;
class MessageView;
class SignalView;
class TokenView;
class VideoView;

// Dialogs
class AboutDialog;
class BlacklistView;
class DeviceDialog;
class HelpDialog;
class LiveDialog;
class LoginDialog;
class PickDialog;
class SeekDialog;
class SyncDialog;

// MainWindow
class MainWindow : public QMainWindow
{
  Q_OBJECT
  typedef MainWindow Self;
  typedef QMainWindow Base;

  typedef Core::Cloud::Alias Alias;
  typedef Core::Cloud::AliasList AliasList;
  typedef Core::Cloud::Annotation Annotation;
  typedef Core::Cloud::AnnotationList AnnotationList;

  // - Types -
public:
  enum Color {
    DefaultColor = 0,
    White, Black, Blue, Red, Purple, Orange
  };

  // - Constructions -
public:
  explicit MainWindow(QWidget *parent = 0, Qt::WindowFlags f = 0);
  bool isValid() const;
  void parseArguments(const QStringList &args);

  // - Signals -
signals:
  void responded(const QString &text);
  void said(const QString &text, const QString &color);
  void showTextRequested(const QString &text);
  void windowClosed();

  void seeked();
  void addAndShowAnnotationRequested(const Annotation &a);
  void setAnnotationsRequested(const AnnotationList &l);

  // - States -
public:
  bool isPlaying() const;
  bool isEditing() const;

  bool hasVisiblePlayer() const;
  PlayerUi *visiblePlayer(); // One of three player Uis, or nullptr.

  bool inputLineHasFocus() const;
  bool prefixLineHasFocus() const;

  bool isTranslateEnabled() const;
  bool isSubtitleStaysOnTop() const;

  bool isAutoPlayNext() const;

public slots:
  void setTranslateEnabled(bool enabled);
  void translate(const QString &text);

  void setSubtitleStaysOnTop(bool t);
  void setSubtitleColorToDefault();
  void setSubtitleColorToWhite();
  void setSubtitleColorToBlue();
  void setSubtitleColorToRed();
  void setSubtitleColorToPurple();
  void setSubtitleColorToOrange();
  void setSubtitleColorToBlack();
  int subtitleColor() const;
  void setSubtitleColor(int colorId);
private:
  void uncheckSubtitleColorActions();

public slots:
  void onFocusedWidgetChanged(QWidget *w_old, QWidget *w_new);

  void setEmbeddedWindow(WId winId);

  void about();
  void help();

  // PlayMode
  void login(const QString &userName, const QString &encryptedPassword, bool async = true);
  void logout(bool async = true);

  void openSource(const QString &source);
  void open();  ///< By default the same as openFile()
  void openFile();
  void openProcess();
  void openWindow();
  void openDevice();
  void openDevicePath(const QString &path, bool isAudioCD);
  void openAudioDevice();
  void openVideoDevice();
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

  void previous();
  void next();
  void autoPlayNext();

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
  void syncInputLineText(const QString &text);
  void syncPrefixLineText(const QString &text);

  void enableWindowTransparency();
  void disableWindowTransparency();

  //void setWindowStaysOnTop(bool enabled = true); // TODO

  void showVideoViewIfAvailable();

public slots:
  void invalidateMediaAndPlay(bool async = true);

  // - Dialogs -
public slots:
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

  void showWindowPickDialog();
  void setWindowPickDialogVisible(bool visible);

  void setProcessPickDialogVisible(bool visible);

  // - User -
public slots:
  void setUserAnonymous(bool t, bool async = true);
  void setUserLanguage(int lang, bool async = true);

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

  // - Error handling -
protected slots:
  void handlePlayerError();

  // - Annotations -
public slots:
  void eval(const QString &cmd);
  void chat(const QString &text, bool async = true);
  void respond(const QString &text);
  void say(const QString &text, const QString &color = "blue");

  void submitText(const QString &text, bool async = true);
  void showText(const QString &text);

  void showTextAsSubtitle(const QString &text);

  void setToken(const QString &filePath, bool async = true);
  void invalidateToken();
  void submitAlias(const Alias &alias, bool async = true);

  void blessTokenWithId(qint64 tid, bool async = true);
  void curseTokenWithId(qint64 tid, bool async = true);

  void blessAliasWithId(qint64 tid, bool async = true);
  void curseAliasWithId(qint64 tid, bool async = true);
  void blockAliasWithId(qint64 tid, bool async = true);

  void blessAnnotationWithId(qint64 tid, bool async = true);
  void curseAnnotationWithId(qint64 tid, bool async = true);
  void blockAnnotationWithId(qint64 tid, bool async = true);

  void updateAnnotationTextWithId(const QString &text, qint64 id);

  // - Recent -
protected slots:
  void addRecent(const QString &path);
  void openRecent(int i);
  void clearRecent();
  void invalidateRecent();
  void invalidateRecentMenu();

  // - Playlist -
protected slots:
  void setRecentOpenedFile(const QString &path);
  void openPlaylistItem(int i);
  void openNextPlaylistItem();
  void openPreviousPlaylistItem();
  void clearPlaylist();
  void invalidatePlaylist();
  void invalidatePlaylistMenu();

  // - Events -
public slots:
  virtual void setVisible(bool visible); ///< \override
protected:
  virtual bool event(QEvent *event); ///< \override
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

  void invalidateContextMenu();
  void invalidateUserMenu();
  void invalidateTrackMenu();

protected:
  bool isGlobalPosNearEmbeddedPlayer(const QPoint &pos) const;

public slots:
  void updatePlayMode();
  void updatePlayerMode();
  void updateTokenMode();
  void updateWindowMode();

  void invalidatePlayerMode();

  // - Themes -
public slots:
  void setThemeToDefault(); void setThemeToRandom();
  void setThemeToBlack1(); void setThemeToBlack2();
  void setThemeToBlue1(); void setThemeToBlue2();
  void setThemeToBrown1(); void setThemeToBrown2();
  void setThemeToGreen1(); void setThemeToGreen2();
  void setThemeToLightBlue1(); void setThemeToLightBlue2();
  void setThemeToOrange1(); void setThemeToOrange2();
  void setThemeToPink1(); void setThemeToPink2();
  void setThemeToPurple1(); void setThemeToPurple2();
  void setThemeToRed1(); void setThemeToRed2();
  void setThemeToYellow1(); void setThemeToYellow2();

  // - Browse -
public slots:
  void setBrowsedFile(const QString &filePath);
  void openBrowsedFile(int id);
  void openPreviousFile();
  void openNextFile();

protected:
  int currentBrowsedFileId() const;

  // - Signal mode -
#ifdef USE_WIN_QTH
public slots:
  void openProcessPath(const QString &path);
  void openProcessHook(int hookId, const ProcessInfo &pi = ProcessInfo());
  void openProcessWindow(WId hwnd);
  void openProcessId(ulong pid);
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
protected:
  static bool isMimeDataSupported(const QMimeData *urls);
  static bool isDevicePath(const QString &path);
  static QString removeUrlPrefix(const QString &url);

  static QString digestFromFile(const QString &path);

  static QString languageToString(int lang);
  static int fileType(const QString &fileName);

  bool isPosOutOfScreen(const QPoint &globalPos) const;
  bool isRectOutOfScreen(const QRect &globalRect) const;

  bool isDigestReady(const QString &path) const;
  bool isAliasReady(const QString &path) const;

  // - Members for initialization. -
private:
  void createComponents();
  void createActions();
  void createMenus();
  void createDockWindows();
  void createConnections();

  // - Attributes -
private:
  QMutex inetMutex_;    // mutext for remote communication
  QMutex playerMutex_;  // mutex for local player
  Tray *tray_;
  SignalHub *hub_;

  QStringList recentFiles_;
  QFileInfoList browsedFiles_;

  Translator *translator_;

#ifdef USE_MODULE_SERVERAGENT
  ServerAgent *server_;
#endif // USE_MODULE_SERVERAGENT
#ifdef USE_MODULE_CLIENTAGENT
  ClientAgent *client_;
#endif // USE_MODULE_CLIENTAGENT

  Player *player_;

  Database *cache_;
  Database *queue_;
  DataManager *dataManager_;
  DataServer *dataServer_;

  EventLogger *logger_;
  OsdConsole *globalOsdConsole_;

  //OsdDock *osdDock_;
  OsdWindow *osdWindow_;
  TokenView *tokenView_;
  VideoView *videoView_;

  MainPlayerUi *mainPlayer_;
  MiniPlayerUi *miniPlayer_;
  EmbeddedPlayerUi *embeddedPlayer_;

  AnnotationGraphicsView *annotationView_;
  AnnotationBrowser *annotationBrowser_;
  AnnotationEditor *annotationEditor_;
  AnnotationFilter *annotationFilter_;

  AboutDialog *aboutDialog_;
  BlacklistView *blacklistView_;
  HelpDialog *helpDialog_;
  LoginDialog *loginDialog_;
  LiveDialog *liveDialog_;
  PickDialog *windowPickDialog_;
  PickDialog *processPickDialog_;
  SeekDialog *seekDialog_;
  SyncDialog *syncDialog_;
  DeviceDialog *deviceDialog_;

  CloudView *cloudView_;
  CommentView *commentView_;

  QPoint dragPos_;

  qint32 tokenType_; // current token type tt

  Grabber *grabber_;

  QString recentOpenedFile_;
  QString recentPath_;
  QString recentDigest_;

  QStringList playlist_;

  // - Menus and actions -

//#ifndef Q_WS_WIN
  //QMenuBar *menuBar_;
  QMenu *fileMenu_,
        //*viewMenu_,
        *helpMenu_;
//#endif // !Q_WS_WIN

  QMenu *contextMenu_,
        *advancedMenu_,
        *helpContextMenu_,
        *recentMenu_,
        *userMenu_,
        *openMenu_,
        *subtitleMenu_,
        *browseMenu_,
        *trackMenu_,
        *sectionMenu_,
        *backwardMenu_,
        *forwardMenu_,
        *appLanguageMenu_,
        *userLanguageMenu_,
        *annotationLanguageMenu_,
        *themeMenu_,
        *playlistMenu_,
        *subtitleStyleMenu_;
  QList<QAction*> contextMenuActions_;

  QAction *previousSectionAct_,
          *nextSectionAct_,
          *previousFileAct_,
          *nextFileAct_;

  QAction *setSubtitleColorToDefaultAct_,
          *setSubtitleColorToWhiteAct_,
          *setSubtitleColorToBlueAct_,
          *setSubtitleColorToRedAct_,
          *setSubtitleColorToPurpleAct_,
          *setSubtitleColorToOrangeAct_,
          *setSubtitleColorToBlackAct_;

  QAction *openAct_,
          *openFileAct_,
          *openDeviceAct_,
          *openVideoDeviceAct_,
          *openAudioDeviceAct_,
          *openSubtitleAct_,
          *playAct_,
          *pauseAct_,
          *stopAct_,
          *replayAct_,
          *nextFrameAct_,
          *previousAct_,
          *nextAct_,
          *snapshotAct_,
          *toggleAnnotationVisibleAct_,
          *toggleMenuBarVisibleAct_,
          *toggleFullScreenModeAct_,
          *toggleEmbeddedModeAct_,
          *toggleMiniModeAct_,
          *toggleLiveModeAct_,
          *toggleSyncModeAct_,
          *toggleSubtitleVisibleAct_,
          *toggleTranslateAct_,
          *toggleSubtitleStaysOnTopAct_,
          *toggleEmbeddedPlayerOnTopAct_,
          *toggleAutoPlayNextAct_;

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
          *toggleCloudViewVisibleAct_,
          *toggleLiveDialogVisibleAct_,
          *toggleLoginDialogVisibleAct_,
          *toggleWindowPickDialogVisibleAct_,
          *toggleProcessPickDialogVisibleAct_,
          *toggleSeekDialogVisibleAct_,
          *toggleSyncDialogVisibleAct_,
          //*toggleUserPanelVisibleAct_,
          *toggleBlacklistViewVisibleAct_;

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

  QAction *clearRecentAct_;
  QAction *clearPlaylistAct_,
          *nextPlaylistItemAct_,
          *previousPlaylistItemAct_;

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

  QAction *setThemeToDefaultAct_, *setThemeToRandomAct_,
          *setThemeToBlack1Act_, *setThemeToBlack2Act_,
          *setThemeToBlue1Act_, *setThemeToBlue2Act_,
          *setThemeToBrown1Act_, *setThemeToBrown2Act_,
          *setThemeToGreen1Act_, *setThemeToGreen2Act_,
          *setThemeToLightBlue1Act_, *setThemeToLightBlue2Act_,
          *setThemeToOrange1Act_, *setThemeToOrange2Act_,
          *setThemeToPink1Act_, *setThemeToPink2Act_,
          *setThemeToPurple1Act_, *setThemeToPurple2Act_,
          *setThemeToRed1Act_, *setThemeToRed2Act_,
          *setThemeToYellow1Act_, *setThemeToYellow2Act_;

  QAction *showMaximizedAct_,
          *showMinimizedAct_,
          *showNormalAct_,
          *helpAct_,
          *aboutAct_,
          *quitAct_;
};

#endif // MAINWINDOW_H
