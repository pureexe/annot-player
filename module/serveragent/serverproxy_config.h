#ifndef SERVERPROXY_CONFIG_H
#define SERVERPROXY_CONFIG_H

// module/serveragent/serverproxy_config.h
// 10/8/2011

#include <QtGlobal>

// SOAP imode and omode, enforcing UTF-8, and gzip compression
#define SERVER_SOAP_MODE        (SOAP_C_UTFSTRING | SOAP_ENC_ZLIB)

#ifdef Q_WS_WIN
  #define CLIENT_TYPE "player" "-" "win" "-" "0.1.0.3"
#elif defined(Q_WS_MAC)
  #define CLIENT_TYPE "player" "-" "mac" "-" "0.1.0.3"
#else
  #define CLIENT_TYPE "player" "-" "linux" "-" "0.1.0.3"
#endif // Q_WS_

#endif // SERVERPROXY_CONFIG_H
