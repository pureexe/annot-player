#ifndef CLOUDSERVICE_CONFIG_H
#define CLOUDSERVICE_CONFIG_H

// serverproxy_config.h
// 10/8/2011

#include <QtGlobal>

// SOAP imode and omode, enforcing UTF-8, and gzip compression
#define CLOUD_SERVICE_SOAP_MODE     (SOAP_C_UTFSTRING | SOAP_ENC_ZLIB)

#ifdef Q_OS_WIN
# define APP_VERSION "player" "-" "win" "-" VERSION
#elif defined(Q_OS_MAC)
# define APP_VERSION "player" "-" "mac" "-" VERSION
#else
# define APP_VERSION "player" "-" "linux" "-" VERSION
#endif // Q_OS_

#endif // CLOUTSERVICE_CONFIG_H
