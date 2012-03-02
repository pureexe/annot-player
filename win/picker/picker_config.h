#pragma once

// picker_config.h
// 10/30/2011

#ifdef PICKER_LIB
  #define PICKERAPI Q_DECL_EXPORT
#elif defined (PICKER_STATICLIB)
  #define PICKERAPI
#else
  #define PICKERAPI Q_DECL_IMPORT
#endif // PICKER_LIB

// EOF
