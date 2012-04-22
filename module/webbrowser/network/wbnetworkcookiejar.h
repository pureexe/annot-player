#ifndef WBNETWORKCOOKIEJAR_H
#define WBNETWORKCOOKIEJAR_H

// wbnetworkcookiejar.h
// 1/27/2012

#include "module/qtext/network.h"

#ifdef ANNOT_PROXY_DOMAIN

typedef QtExt::NetworkCookieJarWithDomainAlias WbNetworkCookieJarBase;
class WbNetworkCookieJar : public WbNetworkCookieJarBase
{
  Q_OBJECT
  typedef WbNetworkCookieJarBase Base;

public:
  explicit WbNetworkCookieJar(QObject *parent = 0)
    : Base(".nicovideo.jp", ANNOT_PROXY_DOMAIN, parent)
  { }
};

#else
#warning "nico proxy domain is not defined"

class WbNetworkCookieJar : public QNetworkCookieJar
{
  Q_OBJECT
  typedef QNetworkCookieJar Base;

public:
  explicit WbNetworkCookieJar(QObject *parent = 0)
    : Base(parent)
  { }
};

#endif // ANNOT_PROXY_DOMAIN

#endif // WBNETWORKCOOKIEJAR_H
