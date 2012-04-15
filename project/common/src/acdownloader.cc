// acdownloader.cc
// 4/9/2012

#include "ac/acdownloader.h"
#include "ac/acglobal.h"

#define APP_NAME        AC_DOWNLOADER
#define APP_ID          AC_DOWNLOADER_ID
#ifdef PROJECT_DOWNLOADER
#  define APP_ROLE      Server
#else
#  define APP_ROLE      Client
#endif // PROJECT_DOWNLOADER

// - Construction -

AcDownloaderController::AcDownloaderController(QObject *parent, Role role)
  : Base(role ? role : APP_ROLE, parent, APP_ID)
{ }

// - Queries -

bool
AcDownloaderController::isRunning() const
{ return isProcessRunning(APP_NAME); }

// - Actions -

void
AcDownloaderController::open()
{ Base::open(APP_NAME); }

void
AcDownloaderController::openUrl(const QString &url)
{ openUrls(QStringList(url)); }

void
AcDownloaderController::openUrls(const QStringList &urls)
{
#ifdef WITH_MODULE_IPC
  if (isEnabled() && isRunning())
    emit queuedArguments(urls);
  else
#endif // WITH_MODULE_IPC
  Base::open(APP_NAME, urls);
}

// EOF
