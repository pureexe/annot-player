// acplayer.cc
// 4/9/2012

#include "ac/acplayer.h"
#include "ac/acglobal.h"

#define APP_NAME        AC_PLAYER
#define APP_ID          AC_PLAYER_ID
#ifdef PROJECT_PLAYER
#  define APP_ROLE      Server
#else
#  define APP_ROLE      Client
#endif // PROJECT_PLAYER

// - Construction -

AcPlayerController::AcPlayerController(QObject *parent, Role role)
  : Base(role ? role : APP_ROLE, parent, APP_ID)
{ }

// - Queries -

bool
AcPlayerController::isRunning() const
{ return isProcessRunning(APP_NAME); }

// - Actions -

void
AcPlayerController::open()
{ Base::open(APP_NAME); }

void
AcPlayerController::openUrl(const QString &url)
{ openUrls(QStringList(url)); }

void
AcPlayerController::openUrls(const QStringList &urls)
{
#ifdef WITH_MODULE_IPC
  if (isEnabled() && isRunning())
    emit queuedArguments(urls);
  else
#endif // WITH_MODULE_IPC
  Base::open(APP_NAME, urls);
}

// EOF
