#ifndef SIGNALHUB_H
#define SIGNALHUB_H

// signalhub.h
// 10/16/2011
#include "core/cloud/traits.h"
#include <QObject>

using namespace Core::Cloud;

// TODO: Gradually replace signals connections in MainWindow, ServerAgent and Player with this hub
class SignalHub : public QObject
{
  Q_OBJECT
  typedef SignalHub Self;
  typedef QObject Base;

  // - Constructions -
public:
  explicit SignalHub(QObject *parent);

  // - States -
public:
  enum TokenMode { MediaTokenMode = 0, SignalTokenMode };
  TokenMode tokenMode() const           { return tokenMode_; }
  bool isMediaTokenMode() const         { return tokenMode() == MediaTokenMode; }
  bool isSignalTokenMode() const        { return tokenMode() == SignalTokenMode; }
public slots:
  void setTokenMode(TokenMode mode);
  void setMediaTokenMode(bool t = true);
  void setSignalTokenMode(bool t = true);
  void toggleMediaTokenMode()           { setMediaTokenMode(!isMediaTokenMode()); }
  void toggleSignalTokenMode()          { setSignalTokenMode(!isSignalTokenMode()); }
  void stopMediaTokenMode()             { setMediaTokenMode(false); }
  void stopSignalTokenMode()            { setSignalTokenMode(false); }
signals:
  void tokenModeChanged(SignalHub::TokenMode mode);
public:
  int tokenType() const {
    switch (tokenMode_) {
    case MediaTokenMode: return Traits::MediaType;
    case SignalTokenMode: return Traits::GameType;
    default: return Traits::NoType;
    }
  }

public:
  enum PlayerMode { NormalPlayerMode = 0, MiniPlayerMode, FullScreenPlayerMode };
  PlayerMode playerMode() const         { return playerMode_; }
  bool isNormalPlayerMode() const       { return playerMode() == NormalPlayerMode; }
  bool isMiniPlayerMode() const         { return playerMode() == MiniPlayerMode; }
  bool isFullScreenPlayerMode() const   { return playerMode() == FullScreenPlayerMode; }
public slots:
  void setPlayerMode(PlayerMode mode);
  void setNormalPlayerMode();
  void setMiniPlayerMode(bool t = true);
  void setFullScreenPlayerMode(bool t = true);
  void toggleMiniPlayerMode()           { setMiniPlayerMode(!isMiniPlayerMode()); }
  void toggleFullScreenPlayerMode()     { setFullScreenPlayerMode(!isFullScreenPlayerMode()); }
  void stopMiniPlayerMode()             { setMiniPlayerMode(false); }
  void stopFullScreenPlayerMode()       { setFullScreenPlayerMode(false); }
signals:
  void playerModeChanged(SignalHub::PlayerMode mode);

public:
  enum VideoMode { NormalVideoMode = 0, FullScreenVideoMode };
  VideoMode videoMode() const           { return videoMode_; }
  bool isNormalVideoMode() const        { return videoMode() == NormalVideoMode; }
  bool isFullScreenVideoMode() const    { return videoMode() == FullScreenVideoMode; }
public slots:
  void setVideoMode(VideoMode mode);
  void setNormalVideoMode(bool t = true);
  void setFullScreenVideoMode(bool t = true);
  void toggleNormalVideoMode()          { setNormalVideoMode(!isNormalVideoMode()); }
  void toggleFullScreenVideoMode()      { setFullScreenVideoMode(!isFullScreenVideoMode()); }
  void stopNormalVideoMode()            { setNormalVideoMode(false); }
  void stopFullScreenVideoMode()        { setFullScreenVideoMode(false); }
signals:
  void videoModeChanged(SignalHub::VideoMode mode);

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
  TokenMode tokenMode_;
  PlayerMode playerMode_;
  VideoMode videoMode_;
  PlayMode playMode_;

  bool playing_, paused_, stopped_;
};

#endif SIGNALHUB_H
