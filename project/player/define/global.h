#ifndef GLOBAL_H
#define GLOBAL_H

// global.h
// 7/16/2011
// Global parameters.
#include <QtGlobal>

// - Alpha -
enum { ALPHA = 0 };

// - Options -
#define INIT_WINDOW_SIZE   QSize(640, 480)
#define INIT_OPEN_PATH     QDir::homePath()

//#define USE_MODE_DEBUG

#define G_PREFIXLINEEDIT_MAXWIDTH       100     // TODO: Make this auto-resize instead

// - About -
#define G_ORGANIZATION  "Annot"
#define G_APPLICATION   "Player"
#define G_VERSION       "0.0.8.0"
#define G_HOMEPAGE      "http://annot.me"
#define G_UPDATEPAGE    "http://code.google.com/p/annot-player"
#define G_EMAIL         "AnnotCloud@gmail.com"


// - Shared options -
#define G_POLLING_TIMEOUT  100     // In msecs
#define G_VOLUME_MAX       100.0   // The volume is between 0 and 100
#define G_POSITION_MAX     10000.0
#define G_POSITION_STEP    100

#define G_STARTPAGE_URL    G_HOMEPAGE
#define G_TOKENVIEW_URL    G_HOMEPAGE
#define G_UPDATEPAGE_URL   G_UPDATEPAGE

#define G_FORWARD_INTERVAL  10000  // 10 sec
#define G_BACKWARD_INTERVAL 10000  // 10 sec

#define G_VOLUME_DELTA      0.05   // 5 %

#define G_ANNOTATION_MAXSIZE 255

#define G_TRACKING_INTERVAL       200 // msec

// TO BE MOVED INTO mediaplayer.h
#define G_FORMAT_ALL            "*"
#define G_FORMAT_AUDIO          "*.mp3 " "*.mp4 " "*.m4a " "*.m4r "
#define G_FORMAT_VIDEO          "*.mp4 " "*.mkv " "*.rmvb " "*.rm " "*.flv " "*.avi " "*.mpg " "*.mpeg "
#define G_FORMAT_SUBTITLE       "*.ass " "*.ssa"
#define G_FORMAT_SUPPORTED      G_FORMAT_VIDEO G_FORMAT_AUDIO

#define VLC_AUTOHIDE_TIMEOUT    1500    // in msecs, same as VLC
#define G_AUTOHIDE_TIMEOUT (VLC_AUTOHIDE_TIMEOUT * 2)

#define G_CONSOLE_AUTOClEAR_TIMEOUT     5000
#define G_LOGGER_PLAYING_WAITER_TIMEOUT 5100

// - Path -

#ifdef Q_WS_WIN
  #define G_PATH_LOGS   "../logs"
#elif defined (Q_WS_MAC)
  #define G_PATH_LOGS   "../Resources/logs" // TODO: move to ~/Library/Logs instead
#else
  #define G_PATH_LOGS   "."
#endif
#define G_PATH_DEBUG    G_PATH_LOGS "/" "debug.txt"

#ifdef Q_WS_WIN
  #define G_PATH_CACHES  "caches"
#elif defined (Q_WS_MAC)
  #define G_PATH_CACHES  "../Resources/caches" // TODO: move to ~/Library/Caches instead
#else
  #define G_PATH_CACHES  "."
#endif
#define G_PATH_CACHEDB  G_PATH_CACHES "/" "cache.db"
#define G_PATH_QUEUEDB  G_PATH_CACHES "/" "queue.db"


// - UI Style -
#define G_CONTEXTMENU_OPACITY           0.8
#define G_CONTEXTMENU_COLOR             "orange"
#define G_CONTEXTMENU_COLOR_STRENGTH    1       // 0 = no color, 1 = full color (default)
#define G_TOOLBUTTON_BLUR_RADIUS        1.2     // By default, the blur radius is 5 pixels.

#define G_DIALOG_COLOR                  "blue"
#define G_DIALOG_COLOR_STRENGTH         1       // 0 = no color, 1 = full color (default)

#define G_WINDOW_ALPHA                  240

// - Helpers -
#define BAD_POS     QPoint(-1, -1)

// - Annotations -
#define ANNOT_PARSE_CODE(_code)  Core::AnnotationParser::globalInstance()->renderToHtml(_code)
#define ANNOT_REDUCE_HTML(_html) Core::AnnotationParser::globalInstance()->reduceHtml(_html)

// - Hook -
#ifdef USE_WIN_HOOK
  #define HOOK          WindowsHookManager::globalInstance()
#else
  #define HOOK          #error "Hook is not used"
#endif // USE_WIN_HOOK

// - Picker -
#ifdef USE_WIN_PICKER
  #define PICKER        WindowPicker::globalInstance()
#else
  #define PICKER        #error "Picker is not used"
#endif // USE_WIN_PICKER

// - QTH -
#ifdef USE_WIN_QTH
  #define QTH          Qth::globalInstance()
#else
  #define QTH          #error "Hook is not used"
#endif // USE_WIN_HOOK

// - DWM -
#ifdef USE_WIN_DWM
  #define DWM_ENABLE_AERO_WIDGET(_w) \
  { \
    Q_ASSERT(_w); \
    if (Dwm::isCompositionEnabled()) { \
      Dwm::enableBlurBehindWindow(_w); \
      Dwm::extendFrameIntoClientArea(_w); \
      _w->setAttribute(Qt::WA_TranslucentBackground); \
      _w->setAttribute(Qt::WA_NoSystemBackground); \
    } \
  }

  #define DWM_DISABLE_AERO_WIDGET(_w) \
  { \
    Q_ASSERT(_w); \
    if (Dwm::isCompositionEnabled()) { \
      Dwm::enableBlurBehindWindow(_w, false); \
      Dwm::extendFrameIntoClientArea(_w, 0, 0, 0, 0); \
      _w->setAttribute(Qt::WA_TranslucentBackground, false); \
      _w->setAttribute(Qt::WA_NoSystemBackground, false); \
    } \
  }

  #define DWM_ENABLE_ONETIME_AERO_WIDGET(_w) \
  { \
    Q_ASSERT(_w); \
    if (Dwm::isCompositionEnabled()) { \
      Dwm::enableBlurBehindWindow(_w->winId()); \
      Dwm::extendFrameIntoClientArea(_w->winId()); \
      _w->setAttribute(Qt::WA_TranslucentBackground); \
      _w->setAttribute(Qt::WA_NoSystemBackground); \
    } \
  }

#else
  #define DWM_ENABLE_AERO_WIDGET(_w)            ({ })
  #define DWM_DISABLE_AERO_WIDGET(_w)           ({ })
  #define DWM_ENABLE_ONETIME_AERO_WIDGET(_w) ({ })
#endif // USE_WIN_DWM

#endif // GLOBAL_H
