#ifndef SERVERPROXY_CONFIG_H
#define SERVERPROXY_CONFIG_H

// module/serveragent/serverproxy_config.h
// 10/8/2011

#ifndef WITH_MODULE_GSOAP
#  error "gsoap is required"
#endif // WITH_MODULE_GSOAP

#include <QtGlobal>

// SOAP imode and omode, enforcing UTF-8, and gzip compression
#define SERVER_SOAP_MODE        (SOAP_C_UTFSTRING | SOAP_ENC_ZLIB)

#ifdef Q_WS_WIN
  #define CLIENT_TYPE "player" "-" "win" "-" VERSION
#elif defined(Q_WS_MAC)
  #define CLIENT_TYPE "player" "-" "mac" "-" VERSION
#else
  #define CLIENT_TYPE "player" "-" "linux" "-" VERSION
#endif // Q_WS_

#endif // SERVERPROXY_CONFIG_H
