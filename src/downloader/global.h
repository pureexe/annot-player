#ifndef GLOBAL_H
#define GLOBAL_H

// global.h
// 7/16/2011
// Global parameters.
#include "src/common/acglobal.h"
#include "src/common/acpaths.h"
#include <QtGlobal>

// - About -
#define G_DOMAIN        AC_DOMAIN
#define G_ORGANIZATION  AC_ORGANIZATION
#define G_APPLICATION   "Downloader"
#define G_VERSION       VERSION
#define G_EMAIL         AC_EMAIL
#define G_LICENSE       AC_LICENSE

// - Path -

#ifdef Q_OS_WIN
# define G_PATH_PROFILE        QtWin::getAppDataPath() + "/" "me.annot.downloader"
#elif defined(Q_OS_MAC)
# define G_PATH_PROFILE        QtMac::homeApplicationSupportPath() + "/me.annot.downloader"
#else
# define G_PATH_PROFILE        QDir::homePath() + "/.annot/down"
#endif // Q_OS_

#define G_PATH_LOCK             G_PATH_PROFILE
#define G_PATH_LOCK_RUNNING     G_PATH_LOCK "/" "running.lock"

#define G_PATH_DOWNLOADS        AC_PATH_DOWNLOADS

#ifdef Q_OS_WIN
# define G_PATH_LOGS   QCoreApplication::applicationDirPath() + "/" ".."
# define G_PATH_DEBUG  G_PATH_LOGS "/" "Debug Downloader.txt"
#elif defined(Q_OS_MAC)
# define G_PATH_LOGS   QtMac::homeLogsPath() + "/" G_ORGANIZATION "/" G_APPLICATION
# define G_PATH_DEBUG  G_PATH_LOGS "/" "Debug.txt"
#else
# define G_PATH_LOGS   G_PATH_PROFILE
# define G_PATH_DEBUG  G_PATH_LOGS "/" "debug.txt"
#endif // Q_OS_

#endif // GLOBAL_H
