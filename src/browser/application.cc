// application.cc
// 11/18/2011
#include "application.h"
#include "global.h"
#include "src/common/acpaths.h"
#include "lib/download/downloader.h"
#ifdef Q_OS_WIN
# include "win/qtwin/qtwin.h"
#endif // Q_OS_WIN
#ifdef Q_OS_MAC
# include "mac/qtmac/qtmac.h"
#endif // Q_OS_MAC
#include <QtCore>

#define DEBUG "application"
#include "qtx/qxdebug.h"

// - Constructions -

Application::Application(int &argc, char **argv)
  : Base(argc, argv, true), closed_(false)
{
  DOUT("enter");
  setOrganizationDomain(G_DOMAIN);
  setOrganizationName(G_ORGANIZATION);
  setApplicationName(G_APPLICATION);
  setApplicationVersion(G_VERSION);

  setLogFileName(G_PATH_DEBUG);
#ifdef Q_OS_LINUX
  setLockFileName(G_PATH_LOCK_RUNNING);
#endif // Q_OS_LINUX

  createDirectories();
  DOUT("exit");
}

void
Application::createDirectories()
{
  QDir d(AC_PATH_CACHES);
  if (!d.exists())
    d.mkpath(d.absolutePath());

  d = QDir(G_PATH_PROFILE);
  if (!d.exists())
    d.mkpath(d.absolutePath());

  d = QDir(G_PATH_CACHES);
  if (!d.exists())
    d.mkpath(d.absolutePath());

  d = QDir(G_PATH_LOGS);
  if (!d.exists())
    d.mkpath(d.absolutePath());
}

void
Application::close()
{
  if (closed_)
    return;
  DOUT("enter");
  closed_ = true;

  DownloaderController::globalController()->abort();

  if (QThreadPool::globalInstance()->activeThreadCount()) {
    // wait for at most 2 seconds ant kill all threads
    enum { CloseTimeout = 2000 };
    QThreadPool::globalInstance()->waitForDone(CloseTimeout);
  }

  closeAllWindows();
  DOUT("abort in 3 seconds");
  QTimer::singleShot(3000, this, SLOT(abort()));
  DOUT("exit");
}

// EOF
