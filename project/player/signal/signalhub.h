#ifndef SIGNALHUB_H
#define SIGNALHUB_H

// signalhub.h
// 10/16/2011
#include <QtCore/QObject>

class Player;

// TODO: Gradually replace signals connections in MainWindow, ServerAgent and Player with this hub
// Use SignalHub to wrap Player
class SignalHub : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(SignalHub)
  typedef SignalHub Self;
  typedef QObject Base;

  // - Constructions -
public:
  explicit SignalHub(Player *player, QObject *parent);

  // - Properties -
signals:
  void volumeChanged(qreal volume);

public:
  qreal volume() const; ///< [0, 1], or -1 if failed
  void setVolume(qreal percentage);

  // - States -
public:
  enum TokenMode { MediaTokenMode = 0, LiveTokenMode, SignalTokenMode };
  TokenMode tokenMode() const           { return tokenMode_; }
  bool isMediaTokenMode() const         { return tokenMode() == MediaTokenMode; }
  bool isLiveTokenMode() const          { return tokenMode() == LiveTokenMode; }
  bool isSignalTokenMode() const        { return tokenMode() == SignalTokenMode; }
public slots:
  void setTokenMode(TokenMode mode);
  void setMediaTokenMode(bool t = true);
  void setLiveTokenMode(bool t = true);
  void setSignalTokenMode(bool t = true);
  void toggleMediaTokenMode()           { setMediaTokenMode(!isMediaTokenMode()); }
  void toggleLiveTokenMode()            { setLiveTokenMode(!isLiveTokenMode()); }
  void toggleSignalTokenMode()          { setSignalTokenMode(!isSignalTokenMode()); }
  void stopMediaTokenMode()             { setMediaTokenMode(false); }
  void stopLiveTokenMode()              { setLiveTokenMode(false); }
  void stopSignalTokenMode()            { setSignalTokenMode(false); }
signals:
  void tokenModeChanged(SignalHub::TokenMode mode);

public:
  enum PlayerMode { NormalPlayerMode = 0, MiniPlayerMode, EmbeddedPlayerMode };
  PlayerMode playerMode() const         { return playerMode_; }
  bool isNormalPlayerMode() const       { return playerMode() == NormalPlayerMode; }
  bool isMiniPlayerMode() const         { return playerMode() == MiniPlayerMode; }
  bool isEmbeddedPlayerMode() const     { return playerMode() == EmbeddedPlayerMode; }
public slots:
  void setPlayerMode(PlayerMode mode);
  void setNormalPlayerMode();
  void setMiniPlayerMode(bool t = true);
  void setEmbeddedPlayerMode(bool t = true);
  void toggleMiniPlayerMode()           { setMiniPlayerMode(!isMiniPlayerMode()); }
  void toggleEmbeddedPlayerMode()       { setEmbeddedPlayerMode(!isEmbeddedPlayerMode()); }
  void stopMiniPlayerMode()             { setMiniPlayerMode(false); }
  void stopEmbeddedPlayerMode()         { setEmbeddedPlayerMode(false); }
signals:
  void playerModeChanged(SignalHub::PlayerMode mode);

public:
  enum WindowMode { NormalWindowMode = 0, FullScreenWindowMode };
  WindowMode windowMode() const          { return windowMode_; }
  bool isNormalWindowMode() const        { return windowMode() == NormalWindowMode; }
  bool isFullScreenWindowMode() const    { return windowMode() == FullScreenWindowMode; }
public slots:
  void setWindowMode(WindowMode mode);
  void setNormalWindowMode(bool t = true);
  void setFullScreenWindowMode(bool t = true);
  void toggleNormalWindowMode()          { setNormalWindowMode(!isNormalWindowMode()); }
  void toggleFullScreenWindowMode()      { setFullScreenWindowMode(!isFullScreenWindowMode()); }
  void stopNormalWindowMode()            { setNormalWindowMode(false); }
  void stopFullScreenWindowMode()        { setFullScreenWindowMode(false); }
signals:
  void windowModeChanged(SignalHub::WindowMode mode);

public:
  enum PlayMode { NormalPlayMode = 0, SyncPlayMode, LivePlayMode };
  PlayMode playMode() const             { return playMode_; }
  bool isNormalPlayMode() const         { return playMode() == NormalPlayMode; }
  bool isSyncPlayMode() const           { return playMode() == SyncPlayMode; }
  bool isLivePlayMode() const           { return playMode() == LivePlayMode; }
public slots:
  void setPlayMode(PlayMode mode);
  void setSyncPlayMode(bool t = true);
  void setLivePlayMode(bool t = true);
  void toggleSyncPlayMode()             { setSyncPlayMode(!isSyncPlayMode()); }
  void toggleLivePlayMode()             { setLivePlayMode(!isLivePlayMode()); }
  void stopSyncPlayMode()               { setSyncPlayMode(false); }
  void stopLivePlayMode()               { setLivePlayMode(false); }
signals:
  void playModeChanged(SignalHub::PlayMode mode);

  // - Actions -
signals:
  void played();
public:
  bool isPlaying() const { return playing_; }
public slots:
  void play();

signals:
  void paused();
public:
  bool isPaused() const { return paused_; }
public slots:
  void pause();

signals:
  void stopped();
public:
  bool isStopped() const { return stopped_; }
public slots:
  void stop();

signals:
  void opened();
public slots:
  void open();

  // - Implementations -
private:
  Player *player_;

  TokenMode tokenMode_;
  PlayMode playMode_;
  PlayerMode playerMode_;
  WindowMode windowMode_;

  bool playing_, paused_, stopped_;
};

#endif // SIGNALHUB_H
