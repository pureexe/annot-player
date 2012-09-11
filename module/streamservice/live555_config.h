#ifndef LIVE555CONFIG_H
#define LIVE555CONFIG_H

// live555_config.h
// 10/16/2011

#ifdef _MSC_VER
# pragma warning (disable:4005)     // C4005: macro redifinition
#endif // _MSC_VER

#ifndef WITH_MODULE_STREAM
# error "require module stream"
#endif // WITH_MODULE_STREAM

#endif // LIVE555CONFIG_H
