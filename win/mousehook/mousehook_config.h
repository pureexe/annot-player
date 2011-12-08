#pragma once

// mousehook_config.h
// 11/26/2011

#include <QtGlobal>

#ifndef Q_WS_WIN
  #error "This file is for windows only."
#endif

#ifdef MOUSEHOOK_LIB
  #define MOUSEHOOKAPI Q_DECL_EXPORT
#elif defined (MOUSEHOOK_STATICLIB)
  #define MOUSEHOOKAPI
#else
  #define MOUSEHOOKAPI Q_DECL_IMPORT
#endif // MOUSEHOOK_LIB

// EOF
