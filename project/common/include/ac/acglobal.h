#ifndef _AC_ACGLOBAL_H
#define _AC_ACGLOBAL_H

// ac/acglobal.h
// 7/16/2011
#include <QtGlobal>

// - About -
#define AC_DOMAIN        "annot.me"
#define AC_ORGANIZATION  "Annot"
#define AC_APPLICATION   "Cloud"
#define AC_VERSION       "0.1.0.0"
#define AC_EMAIL         "AnnotCloud@gmail.com"
#define AC_LICENSE       "GNU GPL v3"

// - Apps -

#define AC_PLAYER_ID     1
#define AC_DOWNLOADER_ID 2
#define AC_BROWSER_ID    3

#ifdef Q_WS_MAC
#  define AC_BROWSER    "Annot Browser"
#  define AC_DOWNLOADER "Annot Downloader"
#  define AC_PLAYER     "Annot Player"
#else
#  define AC_BROWSER    "annot-browser"
#  define AC_DOWNLOADER "annot-down"
#  define AC_PLAYER     "annot-player"
#endif // Q_WS_MAC

#endif // _AC_ACGLOBAL_H
