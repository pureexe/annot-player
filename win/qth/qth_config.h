#pragma once

// qth_config.h
// 10/20/2011

#include <QtGlobal>

#ifndef Q_WS_WIN
  #error "This file is for windows only."
#endif

#ifdef QTH_LIB
  #define QTHAPI Q_DECL_EXPORT
#elif defined(QTH_STATICLIB)
  #define QTHAPI
#else
  #define QTHAPI Q_DECL_IMPORT
#endif // QTH_LIB

// EOF
