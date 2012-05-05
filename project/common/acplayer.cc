// acplayer.cc
// 4/9/2012

#include "project/common/acplayer.h"
#include "project/common/acglobal.h"

#define APP_NAME        AC_PLAYER
#define APP_ID          AC_PLAYER_ID

#define DEBUG "acplayer"
#include "module/debug/debug.h"

// - Server -

AcPlayerServer::AcPlayerServer(QObject *parent)
  : Base(parent, APP_ID)
{ }

void
AcPlayerServer::start()
{
#ifdef WITH_MODULE_METACALL
  Base::startServer();
#endif // WITH_MODULE_METACALL
}

void
AcPlayerServer::stop()
{
#ifdef WITH_MODULE_METACALL
  Base::stop();
#endif // WITH_MODULE_METACALL
}

// - Client -

AcPlayer::AcPlayer(QObject *parent)
  : Base(parent), delegate_(0)
{
#ifdef WITH_MODULE_METACALL
  // It is essential that the queued signal is passed to delegate_ from another object
  delegate_ = new Delegate(0, APP_ID);
  connect(this, SIGNAL(arguments(QStringList)),
          delegate_, SIGNAL(arguments(QStringList)), Qt::QueuedConnection);
#endif // WITH_MODULE_METACALL
}

bool
AcPlayer::isRunning() const
{ return Delegate::isProcessRunning(APP_NAME); }

void
AcPlayer::open()
{ Delegate::open(APP_NAME); }

void
AcPlayer::openArguments(const QStringList &args)
{
  DOUT("enter");
#ifdef WITH_MODULE_METACALL
  if (isRunning()) {
    DOUT("isRunning = true");
    if (!delegate_->isActive())
      delegate_->startClient();
    emit arguments(args);
  } else
#endif // WITH_MODULE_METACALL
  Delegate::open(APP_NAME, args);
  DOUT("exit");
}

void
AcPlayer::importUrl(const QString &url)
{
  if (!url.isEmpty()) {
    QString a = url;
    a.replace(QRegExp("^http://", Qt::CaseInsensitive), ACSCHEME_PLAYER_IMPORT);
    openUrl(a);
  }
}

// EOF
