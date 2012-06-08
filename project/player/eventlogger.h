#ifndef EVENTLOGGER_H
#define EVENTLOGGER_H

// eventlogger.h
// 8/7/2011

#include <QtCore/QList>
#include <QtCore/QObject>

QT_FORWARD_DECLARE_CLASS(QTimer)

class Player;
class SignalHub;

class EventLogger : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(EventLogger)
  typedef EventLogger Self;
  typedef QObject Base;

  Player *player_;
  SignalHub *hub_;
  QTimer *logUntilPlayingTimer_;
  int logCount_;

public:
  EventLogger(Player *player, SignalHub *hub, QObject *parent = 0);

public slots:
  void logSubtitleChanged();
  void logAudioTrackChanged();
  void logVolumeChanged();
  void logMediaChanged();
  void logMediaClosed();
  void logTitleChanged();
  void logOpening();
  void logBuffering();
  void logPlaying();
  void logPaused();
  void logStopped();
  void logPlayerError();
  void logTrackNumberChanged(int track);
  void logPlayRateChanged(qreal rate);

  void logCanvasEnabled(bool t);

  void logTranslatorNetworkError(const QString &message);

  void logLoginRequested(const QString &userName);
  void logLoginSucceeded(const QString &userName);
  void logLoginFailed(const QString &userName);
  void logLogoutRequested();
  void logLogoutFinished();

  void logSeeked(qint64 msecs);

  void logCacheCleared();

  void logTrackedWindowDestroyed();

  void logServerAgentConnectionError();
  void logServerAgentServerError();
  void logServerAgentError404();
  void logServerAgentUnknownError();

  void logClientAgentAuthorized();
  void logClientAgentDeauthorized();
  void logClientAgentAuthorizationError();

  void logAspectRatioChanged(const QString &ratio);
  void logContrastChanged(qreal value);
  void logBrightnessChanged(qreal value);
  void logHueChanged(int value);
  void logSaturationChanged(qreal value);
  void logGammaChanged(qreal value);

  void logAnnotationScaleChanged(qreal value);
  void logAnnotationRotationChanged(qreal value);
  void logAnnotationOffsetChanged(int value);
  void logAnnotationCountLimitedChanged(bool value);
  void logAnnotationSkipped();

  void logPauseHoveredAnnotations(bool t);
  void logResumeHoveredAnnotations(bool t);
  void logRemoveHoveredAnnotations(bool t);
  void logExpelNearbyAnnotations(bool t);
  void logAttractNearbyAnnotations(bool t);

  void logPreferLocalDatabaseChanged(bool t);

  void logSelectedUserIds(const QList<qint64> &uids);

  void logInternetConnectionChanged(bool t);
  void logPreferMotionlessAnnotationChanged(bool t);

protected slots:
  void logAeroEnabledChanged(bool t);
  void logMenuEnabledChanged(bool t);

protected slots:
  void logUntilPlaying();
  void startLogUntilPlaying();
  void stopLogUntilPlaying();

private:
  void createConnections();
};

#endif // EVENTLOGGER_H
