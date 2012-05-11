// application.cc
// 11/18/2011
#include "application.h"
#include "global.h"
#ifdef Q_WS_WIN
#  include "win/qtwin/qtwin.h"
#endif // Q_WS_WIN
#ifdef Q_WS_MAC
#  include "mac/qtmac/qtmac.h"
#endif // Q_WS_MAC
#include <QtCore/QDir>

#define DEBUG "application"
#include "module/debug/debug.h"

// - Construction -

Application::Application(int &argc, char **argv)
  : Base(argc, argv)
{
  DOUT("enter");
  setOrganizationDomain(G_DOMAIN);
  setOrganizationName(G_ORGANIZATION);
  setApplicationName(G_APPLICATION);
  setApplicationVersion(G_VERSION);

  setLogFileName(G_PATH_DEBUG);
#ifdef Q_WS_X11
  setLockFileName(G_PATH_LOCK_RUNNING);
#endif // Q_WS_X11

  createDirectories();
  DOUT("exit");
}

Application::~Application()
{
  DOUT("enter");
  DOUT("exit");
}

void
Application::createDirectories()
{
  QDir profile(G_PATH_PROFILE);
  if (!profile.exists())
    profile.mkpath(profile.absolutePath());

  QDir lock(G_PATH_LOCK);
  if (!lock.exists())
    lock.mkpath(lock.absolutePath());

  QDir caches(G_PATH_CACHES);
  if (!caches.exists())
    caches.mkpath(caches.absolutePath());

  QDir logs(G_PATH_LOGS);
  if (!logs.exists())
    logs.mkpath(logs.absolutePath());
}

// EOF
