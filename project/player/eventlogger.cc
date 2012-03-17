// eventlogger.cc
// 8/7/2011

#include "eventlogger.h"
#include "osdconsole.h"
#include "defines.h"
#include "logger.h"
#include "tr.h"
#include "defines.h"
#include "module/qtext/htmltag.h"
#include "module/qtext/datetime.h"
#include "module/player/player.h"
#include "module/serveragent/serveragent.h"
#include <QtCore>

#define DEBUG "eventlogger"
#include "module/debug/debug.h"

using namespace Logger;

EventLogger::EventLogger(Player *player, QObject *parent)
  : Base(parent), player_(player), logUntilPlayingTimer_(0), logCount_(0)
{
  Q_ASSERT(player_);
  createConnections();
}

void
EventLogger::createConnections()
{
  //connect(player_, SIGNAL(titleIdChanged(int)), SLOT(logTitleChanged()));
  connect(player_, SIGNAL(mediaChanged()), SLOT(logMediaChanged()));
  connect(player_, SIGNAL(mediaClosed()), SLOT(logMediaClosed()));
  connect(player_, SIGNAL(volumeChanged()), SLOT(logVolumeChanged()));
  connect(player_, SIGNAL(subtitleChanged()), SLOT(logSubtitleChanged()));
  connect(player_, SIGNAL(audioTrackChanged()), SLOT(logAudioTrackChanged()));
  connect(player_, SIGNAL(opening()), SLOT(logOpening()));
  connect(player_, SIGNAL(buffering()), SLOT(logBuffering()));
  connect(player_, SIGNAL(playing()), SLOT(logPlaying()));
  connect(player_, SIGNAL(paused()), SLOT(logPaused()));
  connect(player_, SIGNAL(stopped()), SLOT(logStopped()));
  connect(player_, SIGNAL(errorEncountered()), SLOT(logPlayerError()));
  connect(player_, SIGNAL(errorEncountered()), SLOT(stopLogUntilPlaying()));
  connect(player_, SIGNAL(trackNumberChanged(int)), SLOT(logTrackNumberChanged(int)));
  connect(player_, SIGNAL(rateChanged(qreal)), SLOT(logPlayRateChanged(qreal)));

  connect(player_, SIGNAL(buffering()), SLOT(startLogUntilPlaying()));
  connect(player_, SIGNAL(stopped()), SLOT(stopLogUntilPlaying()));
}

// - Logging -

void
EventLogger::startLogUntilPlaying()
{
  //DOUT("enter");
  if (player_->hasMedia()) {
    logCount_ = 0;
    if (!logUntilPlayingTimer_) {
      logUntilPlayingTimer_ = new QTimer(this);
      logUntilPlayingTimer_->setInterval(G_LOGGER_PLAYING_WAITER_TIMEOUT);
      connect(logUntilPlayingTimer_, SIGNAL(timeout()), SLOT(logUntilPlaying()));
    }
    if (!logUntilPlayingTimer_->isActive())
      logUntilPlayingTimer_->start();
  }
  //DOUT("exit");
}

void
EventLogger::stopLogUntilPlaying()
{
  DOUT("enter");
  Q_ASSERT(logUntilPlayingTimer_);
  if (logUntilPlayingTimer_ && logUntilPlayingTimer_->isActive())
    logUntilPlayingTimer_->stop();
  DOUT("exit");
}

void
EventLogger::logUntilPlaying()
{
  if (!player_->hasMedia())
    return;

  if (player_->isPlaying() || player_->isPaused()) {
    stopLogUntilPlaying();
    // FIXME: to be moved to other player_!!
    if (player_->isMouseEventEnabled())
      player_->startVoutTimer();
    return;
  }

  if (!logCount_)
    log(tr("caching fonts on first launch ... this could take up to 10min, don't panic!"));
  else {
    int msecs = logCount_ * G_LOGGER_PLAYING_WAITER_TIMEOUT;
    QTime t = QtExt::msecs2time(msecs);
    QString s = QString("%1 / %2").arg(t.toString("m:ss"));
    if (logCount_ < 100) // 5sec * 100 < 10:0min = 60sec * 10
        s = s.arg("10:00");
    else
        s = s.arg("15:00");
    log(tr("patient ... ") + s);
  }
  logCount_++;
}

void
EventLogger::logMediaChanged()
{ log(tr("media changed")); }

void
EventLogger::logTitleChanged()
{ log(tr("title changed")); }

void
EventLogger::logMediaClosed()
{ log(tr("media closed")); }

void
EventLogger::logVolumeChanged()
{
  //static bool first = true;
  //if (first) {
  //  first = false;
  //  return;
  //}

  if (player_->isValid()) {
    int v = player_->volume() * 100;
    if (v == 50)
      return;
    QString msg = QString("%1: " HTML_STYLE_OPEN(color:orange) "%2%" HTML_STYLE_CLOSE())
        .arg(TR(T_VOLUME)).arg(QString::number(v));
    log(msg);
  }
}

void
EventLogger::logSubtitleChanged()
{ log(tr("subtitle changed")); }

void
EventLogger::logAudioTrackChanged()
{ log(tr("audio track changed")); }

void
EventLogger::logOpening()
{ log(tr("opening ...")); }

void
EventLogger::logBuffering()
{
  enum { interval = 2000 }; // 1 second
  static qint64 time = 0;
  qint64 now = QDateTime::currentMSecsSinceEpoch();
  if (now <= time + interval) {
    time = now;
    return;
  }
  time = now;
  log(tr("buffering ..."));
}

void
EventLogger::logPlaying()
{
  QString title;
  if (player_->hasMedia()) {
    title = player_->mediaTitle();
    if (title.isEmpty()) {
      title = player_->mediaPath();
      if (!title.isEmpty())
        title = QFileInfo(title).fileName();
    }
  }
  log(tr("playing: ") + title);
}

void
EventLogger::logStopped()
{ log(tr("stopped")); }

void
EventLogger::logPaused()
{ log(tr("paused")); }

void
EventLogger::logPlayerError()
{ warn(tr("player error")); }

void
EventLogger::logTrackNumberChanged(int track)
{ log(tr("openning track %1").arg(QString::number(track))); }

void
EventLogger::logPlayRateChanged(qreal rate)
{
  int r = qRound(rate);
  if (r == 1)
    log(tr("resume playing"));
  else
    log(tr("fast forward") +
        QString(": " HTML_STYLE_OPEN(color:orange) "x%1" HTML_STYLE_CLOSE())
        .arg(QString::number(r))
    );
}

// - Server -

void
EventLogger::logLoginRequested(const QString &userName)
{ log(tr("logging in ... using: ") + userName); }

void
EventLogger::logLoginSucceeded(const QString &userName)
{ log(tr("login succeeded as: ") + userName); }

void
EventLogger::logLoginFailed(const QString &userName)
{ warn(tr("failed to login as: ") + userName); }

void
EventLogger::logLogoutRequested()
{ log(tr("logging out ...")); }

void
EventLogger::logLogoutFinished()
{ log(tr("user logged out")); }

void
EventLogger::logSeeked(qint64 msecs)
{
  if (player_->isValid()) {
    qint64 len = player_->mediaLength();
    QTime t = QtExt::msecs2time(msecs);
    QTime l = QtExt::msecs2time(len);
    QString msg = QString("%1: " HTML_STYLE_OPEN(color:orange) "%2" HTML_STYLE_CLOSE() " / %3")
        .arg(tr("seek"))
        .arg(t.toString())
        .arg(l.toString());
    if (len)
      msg += QString().sprintf(
        " (" HTML_STYLE_OPEN(color:orange) "%.1f%%" HTML_STYLE_CLOSE() ")",
        msecs * 100.0 / len
      );
    log(msg);
  }
}

void
EventLogger::logCacheCleared()
{ log(tr("offline cache removed")); }

void
EventLogger::logTrackedWindowDestroyed()
{ log(tr("tracked window closed")); }

void
EventLogger::logServerAgentConnectionError()
{ warn(tr("serveragent: failed to connect to server")); }

void
EventLogger::logServerAgentError404()
{ warn(tr("serveragent: got error 404 from server")); }

void
EventLogger::logServerAgentServerError()
{ warn(tr("serveragent: got error reply from server")); }

void
EventLogger::logServerAgentUnknownError()
{ warn(tr("serveragent: unknown SOAP error")); }

void
EventLogger::logClientAgentAuthorized()
{ log(tr("clientagent: server authorization succeeded")); }

void
EventLogger::logClientAgentDeauthorized()
{ log(tr("clientagent: server deauthorized")); }

void
EventLogger::logClientAgentAuthorizationError()
{ warn(tr("clientagent: failed to authorize server, mismatched public key")); }

void
EventLogger::logTranslatorNetworkError(const QString &message)
{ warn(tr("translator: got network error: ") + message); }

// EOF
