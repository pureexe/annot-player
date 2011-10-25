// eventlogger.cc
// 8/7/2011

#include "eventlogger.h"
#include "osdconsole.h"
#include "global.h"
#include "logger.h"
#include "core/htmltag.h"
#include "core/util/datetime.h"
#include "module/player/player.h"
#include "module/serveragent/serveragent.h"
#include <QtCore>

using namespace Logger;

EventLogger::EventLogger(Player *player, QObject *parent)
  : Base(parent), player_(player)
{ createConnections(); }

void
EventLogger::createConnections()
{
  Q_ASSERT(player_);

  connect(player_, SIGNAL(mediaChanged()), SLOT(logMediaChanged()));
  connect(player_, SIGNAL(mediaClosed()), SLOT(logMediaClosed()));
  //connect(player_, SIGNAL(volumeChanged()), SLOT(logVolumeChanged()));
  connect(player_, SIGNAL(subtitleChanged()), SLOT(logSubtitleChanged()));
  connect(player_, SIGNAL(opening()), SLOT(logOpening()));
  connect(player_, SIGNAL(buffering()), SLOT(logBuffering()));
  connect(player_, SIGNAL(playing()), SLOT(logPlaying()));
  connect(player_, SIGNAL(paused()), SLOT(logPaused()));
  connect(player_, SIGNAL(stopped()), SLOT(logStopped()));
  connect(player_, SIGNAL(errorEncountered()), SLOT(logPlayerError()));
}

// - Logging -

void
EventLogger::logMediaChanged()
{ log(tr("media changed: ")); }

void
EventLogger::logMediaClosed()
{ log(tr("media closed")); }

void
EventLogger::logVolumeChanged()
{ log(tr("volume changed")); }

void
EventLogger::logSubtitleChanged()
{ log(tr("subtitle changed")); }

void
EventLogger::logOpening()
{ log(tr("opening ...")); }

void
EventLogger::logBuffering()
{ log(tr("buffering ...")); }

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
{ log(tr("seek") + ": " + Core::msecs2time(msecs).toString()); }

void
EventLogger::logCacheCleared()
{ log(tr("offline cache removed")); }

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

// EOF
