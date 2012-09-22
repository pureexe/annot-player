#ifndef ACGLOBAL_H
#define ACGLOBAL_H

// acglobal.h
// 7/16/2011
#include <QtGlobal>

// - About -
#define AC_DOMAIN        "annot.me"
#define AC_ORGANIZATION  "Annot"
#define AC_APPLICATION   "Cloud"
#define AC_VERSION       "0.1.0.0"
#define AC_EMAIL         "AnnotCloud@gmail.com"
#define AC_LICENSE       "GNU GPL v3"
#define AC_HOMEPAGE      "http://" CONFIG_ANNOT_IP
#define AC_UPDATEPAGE    "http://code.google.com/p/annot-player"
#define AC_DOWNLOADPAGE  "http://code.google.com/p/annot-player/downloads"

// - Apps -

#define AC_UPDATER_ID    1
#define AC_PLAYER_ID     2
#define AC_READER_ID     3
#define AC_DOWNLOADER_ID 4
#define AC_BROWSER_ID    5
#define AC_TRANSLATOR_ID 6

#ifdef Q_OS_MAC
# define AC_BROWSER    "Annot Browser"
# define AC_DOWNLOADER "Annot Downloader"
# define AC_PLAYER     "Annot Player"
# define AC_READER     "Annot Reader"
# define AC_TRANSLATOR "Annot Translator"
# define AC_UPDATER    "Annot Updater"
#else
# define AC_BROWSER    "annot-browser"
# define AC_DOWNLOADER "annot-down"
# define AC_PLAYER     "annot-player"
# define AC_READER     "annot-reader"
# define AC_TRANSLATOR "annot-dict"
# define AC_UPDATER    "annot-update"
#endif // Q_OS_MAC

#endif // ACGLOBAL_H
