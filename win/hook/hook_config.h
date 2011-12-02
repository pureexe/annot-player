#pragma once

// hook_config.h
// 7/22/2011

#include <QtGlobal>

#ifndef Q_WS_WIN
  #error "This file is for windows only."
#endif

#ifdef HOOK_LIB
  #define HOOKAPI Q_DECL_EXPORT
#elif defined (HOOK_STATICLIB)
  #define HOOKAPI
#else
  #define HOOKAPI Q_DECL_IMPORT
#endif // HOOK_LIB

// EOF
