#ifndef EVENTLOGGER_H
#define EVENTLOGGER_H

// eventlogger.h
// 8/7/2011

#include <QObject>

QT_FORWARD_DECLARE_CLASS(QTimer)

class Player;

class EventLogger : public QObject
{
  Q_OBJECT
  typedef EventLogger Self;
  typedef QObject Base;

  Player *player_;
  QTimer *logUntilPlayingTimer_;
  int logCount_;

public:
  explicit EventLogger(Player *player, QObject *parent = 0);

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

protected slots:
  void logUntilPlaying();
  void startLogUntilPlaying();
  void stopLogUntilPlaying();

private:
  void createConnections();
};

#endif // EVENTLOGGER_H
