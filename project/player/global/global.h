#ifndef GLOBAL_H
#define GLOBAL_H

// global.h
// 7/16/2011
// Global parameters.
#include "project/common/acglobal.h"
#include "project/common/acpaths.h"
#include <QtGlobal>

// - Alpha -
enum { ALPHA = 0 };

//#define USE_MODE_DEBUG

#define INIT_WINDOW_SIZE   QSize(570, 175)
#define LIBRARY_WINDOW_SIZE   QSize(804, 400)

#define G_PREFIXLINE_MAXWIDTH       100

// - About -
#define G_DOMAIN        AC_DOMAIN
#define G_ORGANIZATION  AC_ORGANIZATION
#define G_APPLICATION   "Player"
#define G_VERSION       VERSION
#define G_VERSION_MAJOR VERSION_MAJOR
//#define G_HOMEPAGE      "http://annot.me"
//#define G_HOMEPAGE      "http://annotcloud.com"
#define G_HOMEPAGE      AC_HOMEPAGE
#define G_WIKIPAGE      "http://" ANNOT_DOC_IP "/w"
#define G_WIKIPAGE_FAQ  G_WIKIPAGE "/index.php?title=Manual:FAQ"
#define G_UPDATEPAGE    AC_UPDATEPAGE
#define G_DOWNLOADPAGE  AC_DOWNLOADPAGE
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

//#define G_RECENT_COUNT      30

#define G_ANNOTATION_MAXSIZE 255

#define G_TRACKING_INTERVAL     200 // msec

// TO BE MOVED INTO mediaplayer.h
#define G_FILTER_ALL            "*"
#ifdef Q_OS_WIN
# define G_FILTER_AUDIO        PLAYER_FORMAT_AUDIO(" *.")      PLAYER_FORMAT_PLAYLIST(" *.") PLAYER_FORMAT_IMAGE(" *.")
#else
# define G_FILTER_AUDIO        PLAYER_FORMAT_AUDIO(" *.")      PLAYER_FORMAT_IMAGE(" *.")
#endif // Q_OS_WIN
#define G_FILTER_VIDEO          PLAYER_FORMAT_VIDEO(" *.")      PLAYER_FORMAT_IMAGE(" *.")
#define G_FILTER_PICTURE        PLAYER_FORMAT_PICTURE(" *.")
#define G_FILTER_SUBTITLE       PLAYER_FORMAT_SUBTITLE(" *.")

#define G_FORMAT_PROGRAM(...)   __VA_ARGS__ "exe" __VA_ARGS__ "lnk"
#define G_FORMAT_ANNOTATION(...) __VA_ARGS__ "xml" __VA_ARGS__ "json"
#define G_FORMAT_DEVICE(...)    __VA_ARGS__ "DVD" __VA_ARGS__ "AudioCD"
#define G_FORMAT_PREVIEW(...) \
  __VA_ARGS__ "bc!" \
  __VA_ARGS__ "jc!" \
  __VA_ARGS__ "met" \
  __VA_ARGS__ "td" \
  __VA_ARGS__ "ut!"

#define G_FILTER_PROGRAM        G_FORMAT_PROGRAM(" *.")
#define G_FILTER_ANNOTATION     G_FORMAT_ANNOTATION(" *.")
#define G_FILTER_PREVIEW        G_FORMAT_PREVIEW(" *.")
#define G_FILTER_MEDIA          G_FILTER_VIDEO G_FILTER_AUDIO G_FILTER_PICTURE
#ifdef AC_ENABLE_GAME
# define G_FILTER_SUPPORTED    G_FILTER_PROGRAM G_FILTER_MEDIA
#else
# define G_FILTER_SUPPORTED    G_FILTER_MEDIA
#endif // AC_ENABLE_GAME

#define VLC_AUTOHIDE_TIMEOUT    1500    // in msecs, same as VLC
#define G_AUTOHIDE_TIMEOUT (VLC_AUTOHIDE_TIMEOUT * 2)

#define G_LOGGER_PLAYING_WAITER_TIMEOUT 5100

// - Path -

#ifdef Q_OS_WIN
# define G_PATH_PROFILE        QtWin::getAppDataPath() + "/" "me.annot.player"
#elif defined(Q_OS_MAC)
# define G_PATH_PROFILE        QtMac::homeApplicationSupportPath() + "/" "me.annot.player"
#else
# define G_PATH_PROFILE        QDir::homePath() + "/.annot/player"
#endif // Q_OS_WIN

#ifdef Q_OS_WIN
# define G_PATH_LOGS   QCoreApplication::applicationDirPath() + "/.."
# define G_PATH_DEBUG  G_PATH_LOGS "/" "Debug Player.txt"
#elif defined(Q_OS_MAC)
# define G_PATH_LOGS   QtMac::homeLogsPath() + "/" G_ORGANIZATION "/" G_APPLICATION
# define G_PATH_DEBUG  G_PATH_LOGS "/" "Debug.txt"
#else
# define G_PATH_LOGS   G_PATH_PROFILE
# define G_PATH_DEBUG  G_PATH_LOGS "/" "debug.txt"
#endif // Q_OS_

#ifdef Q_OS_WIN
# define G_PATH_CACHES  G_PATH_PROFILE
#elif defined(Q_OS_MAC)
# define G_PATH_CACHES  QtMac::homeCachesPath() + "/me.annot.player"
#else
# define G_PATH_CACHES  G_PATH_PROFILE
#endif // Q_OS_

#define G_PATH_LOCK     G_PATH_PROFILE

#define G_PATH_CACHEDB  G_PATH_CACHES "/" "online.db"
#define G_PATH_QUEUEDB  G_PATH_CACHES "/" "offline.db"
#define G_PATH_MEDIADB  G_PATH_CACHES "/" "mediadb.xml"
#define G_PATH_GAMEDB   G_PATH_CACHES "/" "gamedb.xml"

#define G_PATH_DOWNLOADS AC_PATH_DOWNLOADS

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
//#ifdef WITH_WIN_QTH
//# define QTH           Qth::globalInstance()
//#else
//# define QTH           #error "Hook is not used"
//#endif // WITH_WIN_QTH

#endif // GLOBAL_H
