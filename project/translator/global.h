#ifndef GLOBAL_H
#define GLOBAL_H

// global.h
// 7/16/2011
// Global parameters.
#include "project/common/acglobal.h"
#include "project/common/acpaths.h"
#include <QtGlobal>

// - About -
#define G_DOMAIN        AC_DOMAIN
#define G_ORGANIZATION  AC_ORGANIZATION
#define G_APPLICATION   "Translator"
#define G_VERSION       VERSION
#define G_EMAIL         AC_EMAIL
#define G_LICENSE       AC_LICENSE

// - Path -

#ifdef Q_OS_WIN
# define G_PATH_PROFILE        QtWin::getAppDataPath() + "/" "me.annot.translator"
#elif defined(Q_OS_MAC)
# define G_PATH_PROFILE        QtMac::homeApplicationSupportPath() + "/me.annot.translator"
#else
# define G_PATH_PROFILE        QDir::homePath() + "/.annot/dict"
#endif // Q_OS_

#define G_PATH_LOCK             G_PATH_PROFILE
#define G_PATH_LOCK_RUNNING     G_PATH_LOCK "/" "running.lock"

#ifdef Q_OS_WIN
# define G_PATH_LOGS   QCoreApplication::applicationDirPath() + "/" ".."
# define G_PATH_DEBUG  G_PATH_LOGS "/" "Debug Translator.txt"
#elif defined(Q_OS_MAC)
# define G_PATH_LOGS   QtMac::homeLogsPath() + "/" G_ORGANIZATION "/" G_APPLICATION
# define G_PATH_DEBUG  G_PATH_LOGS "/" "Debug.txt"
#else
# define G_PATH_LOGS   G_PATH_PROFILE
# define G_PATH_DEBUG  G_PATH_LOGS "/" "debug.txt"
#endif // Q_OS_

#ifdef Q_OS_WIN
# define G_PATH_CACHES G_PATH_PROFILE "/" "Caches"
#elif defined(Q_OS_MAC)
# define G_PATH_CACHES QtMac::homeCachesPath() + "/me.annot.translator"
#else
# define G_PATH_CACHES G_PATH_PROFILE
#endif // Q_OS_

#endif // GLOBAL_H
