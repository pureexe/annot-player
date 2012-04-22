#ifndef GLOBAL_H
#define GLOBAL_H

// global.h
// 7/16/2011
// Global parameters.
#include "ac/acglobal.h"
#include <QtGlobal>

// - Alpha -
enum { ALPHA = 0 };

// - Options -
#define INIT_WINDOW_SIZE   QSize(640, 480)

//#define USE_MODE_DEBUG

#define G_PREFIXLINE_MAXWIDTH       100

// - About -
#define G_DOMAIN        AC_DOMAIN
#define G_ORGANIZATION  AC_ORGANIZATION
#define G_APPLICATION   "Player"
#define G_VERSION       VERSION
//#define G_HOMEPAGE      "http://annot.me"
//#define G_HOMEPAGE      "http://annotcloud.com"
#define G_HOMEPAGE      "http://" ANNOT_HOST_IP
#define G_WIKIPAGE      "http://" ANNOT_DOC_IP "/w"
#define G_WIKIPAGE_FAQ  G_WIKIPAGE "/index.php?title=Manual:FAQ"
#define G_UPDATEPAGE    "http://code.google.com/p/annot-player"
#define G_DOWNLOADPAGE  "http://code.google.com/p/annot-player/downloads"
#define G_EMAIL         AC_EMAIL
#define G_LICENSE       AC_LICENSE

// - Shared options -
#define G_POLLING_TIMEOUT  100     // In msecs
#define G_VOLUME_MAX       100.0   // The volume is between 0 and 100
#define G_POSITION_MAX     10000.0
#define G_POSITION_STEP    100

#define G_OPENPROCESS_TIMEOUT 5000 // 5 sec
#define G_WINDOWONTOP_TIMEOUT 4000 // 4 sec

#define G_STARTPAGE_URL    G_HOMEPAGE
#define G_TOKENVIEW_URL    G_HOMEPAGE
#define G_UPDATEPAGE_URL   G_DOWNLOADPAGE
#define G_HELPPAGE_URL     G_WIKIPAGE_FAQ

#define G_FORWARD_INTERVAL  8000   // 8 sec
#define G_BACKWARD_INTERVAL 10000  // 10 sec

#define G_VOLUME_DELTA      0.05   // 5 %

#define G_RECENT_COUNT      20

#define G_ANNOTATION_MAXSIZE 255

#define G_ANNOT_CHAR_WIDTH      20

#define G_TRACKING_INTERVAL     200 // msec

// TO BE MOVED INTO mediaplayer.h
#define G_FORMAT_ALL            "*"
#define G_FORMAT_AUDIO          PLAYER_FORMAT_PLAYLIST(" *.") PLAYER_FORMAT_AUDIO(" *.")
#define G_FORMAT_VIDEO          PLAYER_FORMAT_VIDEO(" *.")
#define G_FORMAT_PICTURE        PLAYER_FORMAT_PICTURE(" *.")
#define G_FORMAT_SUBTITLE       PLAYER_FORMAT_SUBTITLE(" *.")

#define G_FORMAT_PROGRAM_(...)  __VA_ARGS__ "exe" __VA_ARGS__ "lnk"
#define G_FORMAT_PROGRAM        G_FORMAT_PROGRAM_(" *.")
#define G_FORMAT_MEDIA          G_FORMAT_VIDEO G_FORMAT_AUDIO G_FORMAT_PICTURE
#ifdef USE_MODE_SIGNAL
#  define G_FORMAT_SUPPORTED    G_FORMAT_PROGRAM G_FORMAT_MEDIA
#else
#  define G_FORMAT_SUPPORTED    G_FORMAT_MEDIA
#endif // USE_MODE_SIGNAL

#define VLC_AUTOHIDE_TIMEOUT    1500    // in msecs, same as VLC
#define G_AUTOHIDE_TIMEOUT (VLC_AUTOHIDE_TIMEOUT * 2)

#define G_CONSOLE_AUTOClEAR_TIMEOUT     5000
#define G_LOGGER_PLAYING_WAITER_TIMEOUT 5100

// - Path -

#ifdef Q_OS_WIN
#  define G_PATH_PROFILE        QtWin::getAppDataPath() + "/" G_ORGANIZATION "/" G_APPLICATION
#elif defined(Q_OS_MAC)
#  define G_PATH_PROFILE        QtMac::homeApplicationSupportPath() + "/" G_ORGANIZATION "/" G_APPLICATION
#else
#  define G_PATH_PROFILE        QDir::homePath() + "/.annot/player"
#endif // Q_OS_WIN

#ifdef Q_WS_WIN
#  define G_PATH_LOGS   ".."
#  define G_PATH_DEBUG    G_PATH_LOGS "/" "Debug.txt"
#elif defined Q_WS_MAC
#  define G_PATH_LOGS   QtMac::homeLogsPath() + "/" G_ORGANIZATION "/" G_APPLICATION
#  define G_PATH_DEBUG    G_PATH_LOGS "/" "Debug.txt"
#else
#  define G_PATH_LOGS   G_PATH_PROFILE
#  define G_PATH_DEBUG    G_PATH_LOGS "/" "debug.txt"
#endif // Q_WS_

#ifdef Q_WS_WIN
#  define G_PATH_CACHES  "./caches"
#elif defined Q_WS_MAC
#  define G_PATH_CACHES  QtMac::homeCachesPath() + "/" G_ORGANIZATION "/" G_APPLICATION
#else
#  define G_PATH_CACHES  G_PATH_PROFILE
#endif // Q_WS_

#define G_PATH_LOCK     G_PATH_PROFILE

#define G_PATH_CACHEDB  G_PATH_CACHES "/" "cache.db"
#define G_PATH_QUEUEDB  G_PATH_CACHES "/" "queue.db"

#define G_PATH_DOWNLOADS QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + "/" + TR(T_VIDEO)

#define G_PATH_LOCK_RUNNING  G_PATH_LOCK "/" "running.lock"

// - Helpers -
#define BAD_POS     QPoint(-1, -1)

// - Annotations -
#define ANNOT_PARSE_CODE(_code)  AnnotCloud::AnnotationHtmlParser::globalInstance()->toHtml(_code)
#define ANNOT_REDUCE_HTML(_html) AnnotCloud::AnnotationHtmlParser::globalInstance()->reduceHtml(_html)

// - Hook -
#ifdef WITH_WIN_HOOK
  #define HOOK          WindowsHookManager::globalInstance()
#else
  #define HOOK          #error "Hook is not used"
#endif // WITH_WIN_HOOK

// - QTH -
#ifdef WITH_WIN_QTH
  #define QTH           Qth::globalInstance()
#else
  #define QTH           #error "Hook is not used"
#endif // WITH_WIN_QTH

#endif // GLOBAL_H
