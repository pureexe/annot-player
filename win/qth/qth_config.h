#pragma once

// qth_config.h
// 10/20/2011

#ifdef QTH_LIB
#  define QTHAPI Q_DECL_EXPORT
#elif defined(QTH_STATICLIB)
#  define QTHAPI
#else
#  define QTHAPI Q_DECL_IMPORT
#endif // QTH_LIB

// EOF
