#pragma once
#ifndef PICKER_CONFIG_H
#define PICKER_CONFIG_H

// picker_config.h
// 10/30/2011

#include <QtGlobal>

#ifndef Q_WS_WIN
  #error "This file is for windows only."
#endif

#ifdef PICKER_LIB
  #define PICKERAPI Q_DECL_EXPORT
#elif defined (PICKER_STATICLIB)
  #define PICKERAPI
#else
  #define PICKERAPI Q_DECL_IMPORT
#endif // PICKER_LIB

#endif // PICKER_CONFIG_H
