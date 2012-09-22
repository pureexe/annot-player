#ifndef CLOUDSERVICE_CONFIG_H
#define CLOUDSERVICE_CONFIG_H

// serverproxy_config.h
// 10/8/2011

#include <QtGlobal>

// SOAP imode and omode, enforcing UTF-8, and gzip compression
#define CLOUD_SERVICE_SOAP_MODE     (SOAP_C_UTFSTRING | SOAP_ENC_ZLIB)

#define AC_VERSION  "0.1.1"

#ifdef Q_OS_WIN
# define APP_VERSION "player" "-" "win" "-" AC_VERSION
#elif defined(Q_OS_MAC)
# define APP_VERSION "player" "-" "mac" "-" AC_VERSION
#else
# define APP_VERSION "player" "-" "linux" "-" AC_VERSION
#endif // Q_OS_

#endif // CLOUTSERVICE_CONFIG_H
