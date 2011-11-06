#ifndef SERVERPROXY_CONFIG_H
#define SERVERPROXY_CONFIG_H

// module/serveragent/serverproxy_config.h
// 10/8/2011

// SOAP imode and omode, enforcing UTF-8, and gzip compression
#define SERVER_SOAP_MODE        (SOAP_C_UTFSTRING | SOAP_ENC_ZLIB)

#define CLIENT_TYPE "player" "-" "win" "-" "0.0.8.3"

#endif // SERVERPROXY_CONFIG_H
