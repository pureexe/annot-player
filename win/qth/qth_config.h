#pragma once
#ifndef QTH_CONFIG_H
#define QTH_CONFIG_H

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

#endif // QTH_CONFIG_H
