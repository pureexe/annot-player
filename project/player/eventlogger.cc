// eventlogger.cc
// 8/7/2011

#include "eventlogger.h"
#include "osdconsole.h"
#include "global.h"
#include "logger.h"
#include "tr.h"
#include "signalhub.h"
#include "application.h"
#include "project/common/acui.h"
#include "module/qtext/htmltag.h"
#include "module/qtext/datetime.h"
#include "module/player/player.h"
#include "module/serveragent/serveragent.h"
#include <QtCore>

#define DEBUG "eventlogger"
#include "module/debug/debug.h"

using namespace Logger;

EventLogger::EventLogger(Player *player, SignalHub *hub, QObject *parent)
  : Base(parent), player_(player), hub_(hub), logUntilPlayingTimer_(0), logCount_(0)
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
  connect(player_, SIGNAL(aspectRatioChanged(QString)), SLOT(logAspectRatioChanged(QString)));
  connect(player_, SIGNAL(contrastChanged(qreal)), SLOT(logContrastChanged(qreal)));
  connect(player_, SIGNAL(brightnessChanged(qreal)), SLOT(logBrightnessChanged(qreal)));
  connect(player_, SIGNAL(hueChanged(int)), SLOT(logHueChanged(int)));
  connect(player_, SIGNAL(saturationChanged(qreal)), SLOT(logSaturationChanged(qreal)));
  connect(player_, SIGNAL(gammaChanged(qreal)), SLOT(logGammaChanged(qreal)));

  connect(player_, SIGNAL(buffering()), SLOT(startLogUntilPlaying()));
  connect(player_, SIGNAL(stopped()), SLOT(stopLogUntilPlaying()));

  connect(AcUi::globalInstance(), SIGNAL(aeroEnabledChanged(bool)), SLOT(logAeroEnabledChanged(bool)));
  connect(AcUi::globalInstance(), SIGNAL(menuEnabledChanged(bool)), SLOT(logMenuEnabledChanged(bool)));
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
    if (!logUntilPlayingTimer_->isActive()) {
      Application::globalInstance()->setCursor(Qt::BusyCursor);
      logUntilPlayingTimer_->start();
    }
  }
  //DOUT("exit");
}

void
EventLogger::stopLogUntilPlaying()
{
  DOUT("enter");
  Q_ASSERT(logUntilPlayingTimer_);
  if (logUntilPlayingTimer_ && logUntilPlayingTimer_->isActive()) {
    Application::globalInstance()->setCursor(Qt::ArrowCursor);
    logUntilPlayingTimer_->stop();
  }
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
  enum { interval = 2000 }; // 2 seconds
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
  log(tr("playing") + ": " + title);
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
{ log(tr("logging in as %1 ...").arg(userName)); }

void
EventLogger::logLoginSucceeded(const QString &userName)
{ log(tr("login succeeded as %1").arg(userName)); }

void
EventLogger::logLoginFailed(const QString &userName)
{ warn(tr("failed to login as %1").arg(userName)); }

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
      msg += QString(" (" HTML_STYLE_OPEN(color:orange) "%1%" HTML_STYLE_CLOSE() ")")
             .arg(QString::number(msecs * 100.0 / len, 'f', 1));
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
{ warn(tr("translator got network error") + ": " + message); }

void
EventLogger::logAspectRatioChanged(const QString &ratio)
{
  if (ratio.isEmpty())
    log(tr("video aspect ratio set to default"));
  else
    log(tr("video aspect ratio") + ": "
        HTML_STYLE_OPEN(color:orange) + ratio + HTML_STYLE_CLOSE());
}

void
EventLogger::logAeroEnabledChanged(bool t)
{
  if (AcUi::globalInstance()->isAeroEnabled())
    notify(tr("Aero is enabled, please restart the program"));
  else if (t)
    warn(tr("failed to enable Aero"));
  else
    notify(tr("Aero is disabled, please restart the program"));
}

void
EventLogger::logMenuEnabledChanged(bool t)
{
  if (t)
    notify(tr("Menu theme is enabled, please restart the program"));
  else
    notify(tr("Menu theme is disabled, please restart the program"));
}

void
EventLogger::logContrastChanged(qreal value)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (qFuzzyCompare(value, 1.0))
    log(tr("contrast reset"));
  else
    log(tr("contrast") + ": "
        HTML_STYLE_OPEN(color:orange) + QString::number(value) + HTML_STYLE_CLOSE());
}

void
EventLogger::logBrightnessChanged(qreal value)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (qFuzzyCompare(value, 1.0))
    log(tr("brightness reset"));
  else
    log(tr("brightness") + ": "
        HTML_STYLE_OPEN(color:orange) + QString::number(value) + HTML_STYLE_CLOSE());
}

void
EventLogger::logHueChanged(int value)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (value)
    log(tr("hue") + ": "
        HTML_STYLE_OPEN(color:orange) + QString::number(value) + HTML_STYLE_CLOSE());
  else
    log(tr("hue reset"));
}

void
EventLogger::logSaturationChanged(qreal value)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (qFuzzyCompare(value, 1.0))
    log(tr("saturation reset"));
  else
    log(tr("saturation") + ": "
        HTML_STYLE_OPEN(color:orange) + QString::number(value) + HTML_STYLE_CLOSE());
}

void
EventLogger::logGammaChanged(qreal value)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (qFuzzyCompare(value, 1.0))
    log(tr("gamma reset"));
  else
    log(tr("gamma") + ": "
        HTML_STYLE_OPEN(color:orange) + QString::number(value) + HTML_STYLE_CLOSE());
}

void
EventLogger::logAnnotationScaleChanged(qreal value)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (qFuzzyCompare(value, 1.0))
    log(tr("scale reset"));
  else
    log(tr("scale") + ": "
        HTML_STYLE_OPEN(color:orange) + QString::number(value) + HTML_STYLE_CLOSE());
}

void
EventLogger::logAnnotationRotationChanged(qreal value)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (qFuzzyCompare(value + 1, 1.0))
    log(tr("rotation reset"));
  else
    log(tr("rotate") + ": "
        HTML_STYLE_OPEN(color:orange) + QString::number(value) + HTML_STYLE_CLOSE());
}

void
EventLogger::logAnnotationOffsetChanged(int value)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (value)
    log(tr("annotation offset") + ": "
        HTML_STYLE_OPEN(color:orange) + QString::number(value) + HTML_STYLE_CLOSE() +
        tr(" sec"));
  else
    log(tr("annotation offset reset"));
}

void
EventLogger::logAnnotationCountLimitedChanged(bool value)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (value)
    log(tr("limit annotations bandwidth"));
  else
    log(tr("display all annotations"));
}

void
EventLogger::logAnnotationSkipped()
{
  enum { interval = 2000 }; // 2 seconds
  static qint64 time = 0;
  qint64 now = QDateTime::currentMSecsSinceEpoch();
  if (now <= time + interval) {
    time = now;
    return;
  }
  time = now;
  notify(tr("so many annotations, skipped"));
}

void
EventLogger::logPauseHoveredAnnotations(bool t)
{
  if (t)
    log(tr("capture hovered annotations"));
}

void
EventLogger::logResumeHoveredAnnotations(bool t)
{
  if (t)
    log(tr("release hovered annotations"));
}

void
EventLogger::logRemoveHoveredAnnotations(bool t)
{
  Q_UNUSED(t);
  //if (t && !hub_->isFullScreenWindowMode())
  //  log(tr("remove hovered annotations"));
}

void
EventLogger::logExpelNearbyAnnotations(bool t)
{
  Q_UNUSED(t);
  //if (t && !hub_->isFullScreenWindowMode())
  //  log(tr("scatter nearby annotations"));
}

void
EventLogger::logAttractNearbyAnnotations(bool t)
{
  Q_UNUSED(t);
  //if (t && !hub_->isFullScreenWindowMode())
  //  log(tr("collect nearby annotations"));
}

void
EventLogger::logInternetConnectionChanged(bool t)
{
  if (t)
    log(tr("connected to the Internet"));
  else
    log(tr("disconnected from the Internet"));
}

void
EventLogger::logSelectedUserIds(const QList<qint64> &uids)
{
  enum { timeout = 500 }; // msecs
  static qint64 ts = 0;
  qint64 count = uids.size();
  if (count) {
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (now > ts + timeout)
      log(tr("found %1 users").arg(
        HTML_STYLE_OPEN(color:orange) + QString::number(count) + HTML_STYLE_CLOSE()
      ));
    ts = now;
  }
}

void
EventLogger::logPreferMotionlessAnnotationChanged(bool t)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (t)
    log(tr("prefer motionless annotations"));
  else
    log(tr("prefer floating annotations"));
}

void
EventLogger::logPreferLocalDatabaseChanged(bool t)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }

  if (t)
    log(tr("prefer offline annotations over online ones"));
  else
    log(tr("prefer online annotations over offline ones"));
}

void
EventLogger::logCanvasEnabled(bool t)
{
  static bool once = true;
  if (once) {
    once = false;
    return;
  }
  if (t)
    log(tr("show annotation analytics while playing"));
  else
    log(tr("hide annotation analytics while playing"));
}

// EOF
