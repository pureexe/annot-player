// signalhub.h
// 10/16/2011
#include "signalhub.h"
#include "logger.h"
#include "module/player/player.h"
#include <QtCore>
#include <cmath>

//#define DEBUG "signalhub"
//#include "module/debug/debug.h"

using namespace Logger;

// - Constructions -
SignalHub::SignalHub(Player *player, QObject *parent)
  : Base(parent),
    player_(player),
    tokenMode_(MediaTokenMode),
    playMode_(NormalPlayMode),
    playerMode_(NormalPlayerMode),
    windowMode_(NormalWindowMode),
    playing_(false), paused_(false), stopped_(true)
{
  Q_ASSERT(player_);
}

// - Properties -

qreal
SignalHub::volume() const
{
  qreal ret = 0;
  switch (tokenMode_) {
  case MediaTokenMode:
    ret = player_->volume();
    break;

  case SignalTokenMode:
    break;

  default: Q_ASSERT(0); break;
  }

#ifdef Q_WS_X11
  // de-expand percentage
  if (ret > 0)
    ret = ::sqrt(ret);
#endif // Q_WS_X11
  return ret;
}

void
SignalHub::setVolume(qreal percentage)
{
#ifdef Q_WS_X11
  // expand percentage
  percentage = percentage * percentage;
#endif // Q_WS_X11

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
    case EmbeddedPlayerMode: log(tr("switched to embedded player mode")); break;
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
    setPlayerMode(isFullScreenWindowMode() ? EmbeddedPlayerMode : NormalPlayerMode);
}

void
SignalHub::setEmbeddedPlayerMode(bool t)
{ setPlayerMode(t ? EmbeddedPlayerMode : NormalPlayerMode); }

// + WindowMode +

void
SignalHub::setWindowMode(WindowMode mode)
{
  if (windowMode_ != mode) {
    windowMode_ = mode;

#ifdef DEBUG
    switch (mode) {
    case NormalWindowMode: log(tr("switched to normal video mode")); break;
    case FullScreenWindowMode: log(tr("switched to full screen video mode")); break;
    }
#endif // DEBUG
    emit windowModeChanged(mode);
  }
}

void
SignalHub::setNormalWindowMode(bool t)
{ setWindowMode(t ? NormalWindowMode : FullScreenWindowMode); }

void
SignalHub::setFullScreenWindowMode(bool t)
{ setWindowMode(t ? FullScreenWindowMode : NormalWindowMode); }

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
