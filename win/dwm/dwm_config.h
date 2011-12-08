#pragma once

// dwm_config.h
// 7/24/2011

#include <QtGlobal>

#ifndef Q_WS_WIN
  #error "This file is for windows only."
#endif

//#ifdef DWM_LIB
//  #define DWM_EXPORT Q_DECL_EXPORT
//#else
//  #define DWM_EXPORT Q_DECL_IMPORT
//#endif // DWM_LIB

// If defined, Windows Notifier for monitoring DWM effect will be used.
// If enabled, cause problem if the widget is not valid.
//#define USE_WINDOW_NOTIFIER

// If not defined, dwmapi will be resolved at runtime through QLibrary.
//#define USE_STATIC_LIB

// EOF
