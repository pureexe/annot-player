#ifndef CLIENTSERVICE_CONFIG_H
#define CLIENTSERVICE_CONFIG_H

// module/clientagent/clientservice_config.h
// 10/7/2011

#define CLIENT_SERVICE_PORT     233 // infact useless

// port range: [5000, 7000]
#define CLIENT_SERVICE_MIN_PORT 5000
#define CLIENT_SERVICE_MAX_PORT 7000

// SOAP imode and omode, enforcing UTF-8, and gzip compression
#define CLIENT_SOAP_MODE        (SOAP_C_UTFSTRING | SOAP_ENC_ZLIB)

#endif // CLIENTSERVICE_CONFIG_H
