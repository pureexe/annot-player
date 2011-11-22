// signalhub.h
// 10/16/2011
#include "signalhub.h"
#include "logger.h"
#include "module/player/player.h"
#include <QtCore>

//#define DEBUG "SignalHub"
//#include "module/debug/debug.h"

using namespace Logger;

// - Constructions -
SignalHub::SignalHub(Player *player, QObject *parent)
  : Base(parent),
    player_(player),
    tokenMode_(MediaTokenMode),
    playMode_(NormalPlayMode),
    playerMode_(NormalPlayerMode),
    videoMode_(NormalVideoMode),
    playing_(false), paused_(false), stopped_(true)
{
  Q_ASSERT(player_);
}

// - Properties -

qreal
SignalHub::volume() const
{
  switch (tokenMode_) {
  case MediaTokenMode:
    return player_->volume();

  case SignalTokenMode:
#ifdef Q_WS_WIN
    //return QtWin::getWaveVolume();
    return 0;
#else
    return 0;
#endif // Q_WS_WIN

  default: Q_ASSERT(0); return 0;
  }
}

void
SignalHub::setVolume(qreal percentage)
{
  if (percentage < 0)
    percentage = 0;
  else if (percentage > 1)
    percentage = 1;

  switch (tokenMode_) {
  case MediaTokenMode:
    player_->setVolume(percentage);
    emit volumeChanged(percentage);
    break;

  case SignalTokenMode:
#ifdef Q_WS_WIN
    //QtWin::setWaveVolume(percentage);
    emit volumeChanged(percentage);
#endif // Q_WS_WIN
    break;
  }
}

// - States -

// + TokenMode +

void
SignalHub::setTokenMode(TokenMode mode)
{
  if (tokenMode_ != mode) {
    tokenMode_ = mode;
    switch (mode) {
    case SignalTokenMode: log(tr("switched to signal mode")); break;
    case MediaTokenMode: log(tr("switched to media mode")); break;
    }
    emit tokenModeChanged(mode);
  }
}

void
SignalHub::setMediaTokenMode(bool t)
{ setTokenMode(t ? MediaTokenMode : SignalTokenMode); }

void
SignalHub::setSignalTokenMode(bool t)
{ setTokenMode(t ? SignalTokenMode : MediaTokenMode); }

// + PlayMode +

void
SignalHub::setPlayerMode(PlayerMode mode)
{
  if (playerMode_ != mode) {
    playerMode_ = mode;

#ifdef DEBUG
    switch (mode) {
    case NormalPlayerMode: log(tr("switched to normal player mode")); break;
    case FullScreenPlayerMode: log(tr("switched to full screen player mode")); break;
    case MiniPlayerMode: log(tr("switched to mini player mode")); break;
    }
#endif // DEBUG
    emit playerModeChanged(mode);
  }
}

void
SignalHub::setNormalPlayerMode()
{ setPlayerMode(NormalPlayerMode); }

void
SignalHub::setMiniPlayerMode(bool t)
{
  if (t)
    setPlayerMode(MiniPlayerMode);
  else
    setPlayerMode(isFullScreenVideoMode() ? FullScreenPlayerMode : NormalPlayerMode);
}

void
SignalHub::setFullScreenPlayerMode(bool t)
{ setPlayerMode(t ? FullScreenPlayerMode : NormalPlayerMode); }

// + VideoMode +

void
SignalHub::setVideoMode(VideoMode mode)
{
  if (videoMode_ != mode) {
    videoMode_ = mode;

#ifdef DEBUG
    switch (mode) {
    case NormalVideoMode: log(tr("switched to normal video mode")); break;
    case FullScreenVideoMode: log(tr("switched to full screen video mode")); break;
    }
#endif // DEBUG
    emit videoModeChanged(mode);
  }
}

void
SignalHub::setNormalVideoMode(bool t)
{ setVideoMode(t ? NormalVideoMode : FullScreenVideoMode); }

void
SignalHub::setFullScreenVideoMode(bool t)
{ setVideoMode(t ? FullScreenVideoMode : NormalVideoMode); }

// + PlayMode +

void
SignalHub::setPlayMode(PlayMode mode)
{
  if (playMode_ != mode) {
    playMode_ = mode;

    switch (mode) {
    case NormalPlayMode: log(tr("switched to normal play mode")); break;
    case SyncPlayMode: log(tr("switched to synchronized play mode")); break;
    case LivePlayMode: log(tr("switched to live play mode")); break;
    }
    emit playModeChanged(mode);
  }
}

void
SignalHub::setSyncPlayMode(bool t)
{ setPlayMode(t ? SyncPlayMode : NormalPlayMode); }

void
SignalHub::setLivePlayMode(bool t)
{ setPlayMode(t ? LivePlayMode : NormalPlayMode); }

// - Actions -

void
SignalHub::play()
{
  //if (!playing_) {
    playing_ = true;
    stopped_ = false;
    paused_ = false;
    emit played();
  //}
}

void
SignalHub::pause()
{
  //if (!paused_) {
    paused_ = true;
    playing_ = false;
    stopped_ = false;
    emit paused();
  //}
}

void
SignalHub::stop()
{
  //if (!stopped_) {
    stopped_ = true;
    playing_ = false;
    paused_ = false;
    emit stopped();
  //}
}

void
SignalHub::open()
{ emit opened(); }

// EOF
