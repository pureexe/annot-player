#ifndef GLOBAL_H
#define GLOBAL_H

// global.h
// 7/16/2011
// Global parameters.
#include "ac/acglobal.h"
#include <QtGlobal>

// - About -
#define G_DOMAIN        AC_DOMAIN
#define G_ORGANIZATION  AC_ORGANIZATION
#define G_APPLICATION   "Browser"
#define G_VERSION       VERSION
#define G_EMAIL         AC_EMAIL
#define G_LICENSE       AC_LICENSE

// - Path -

#ifdef Q_WS_WIN
#  define G_PATH_PROFILE        QtWin::getAppDataPath() + "/" G_ORGANIZATION "/" G_APPLICATION
#elif defined Q_WS_MAC
#  define G_PATH_PROFILE        QtMac::homeApplicationSupportPath() + "/" G_ORGANIZATION "/" G_APPLICATION
#else
#  define G_PATH_PROFILE        QDir::homePath() + "/.annot/browser"
#endif // Q_OS_

#define G_PATH_LOCK     G_PATH_PROFILE
#define G_PATH_LOCK_RUNNING  G_PATH_LOCK "/" "running.lock"

#endif // GLOBAL_H
