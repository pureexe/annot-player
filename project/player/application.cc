// application.cc
// 11/18/2011
#include "application.h"
#include "global.h"
#include "project/common/acpaths.h"
#ifdef Q_WS_WIN
# include "win/qtwin/qtwin.h"
#endif // Q_WS_WIN
#ifdef Q_WS_MAC
# include "mac/qtmac/qtmac.h"
#endif // Q_WS_MAC
#include <QtCore/QDir>
#include <QtCore/QProcess>

#define DEBUG "application"
#include "module/debug/debug.h"

// - Construction -

Application::Application(int &argc, char **argv)
  : Base(argc, argv), cursor_(Qt::ArrowCursor)
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

  ignoreWindowsExcetions();
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
  QDir d(AC_PATH_CACHES);
  if (!d.exists())
    d.mkpath(d.absolutePath());

  d = QDir(G_PATH_PROFILE);
  if (!d.exists())
    d.mkpath(d.absolutePath());

  d = QDir(G_PATH_LOCK);
  if (!d.exists())
    d.mkpath(d.absolutePath());

  d = QDir(G_PATH_CACHES);
  if (!d.exists())
    d.mkpath(d.absolutePath());

  d = QDir(G_PATH_LOGS);
  if (!d.exists())
    d.mkpath(d.absolutePath());
}

// - Properties -

void
Application::setCursor(Qt::CursorShape cursor)
{
  if (cursor_ != cursor) {
    if (overrideCursor())
      restoreOverrideCursor();
    cursor_ = cursor;
    if (cursor_ != Qt::ArrowCursor)
      setOverrideCursor(cursor_);
  }
}

// EOF
