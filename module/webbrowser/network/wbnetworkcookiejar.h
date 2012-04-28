#ifndef WBNETWORKCOOKIEJAR_H
#define WBNETWORKCOOKIEJAR_H

// network/wbnetworkcookiejar.h
// 1/27/2012

#ifdef ANNOT_PROXY_DOMAIN
#  include "module/qtext/networkcookie.h"

typedef QtExt::NetworkCookieJarWithDomainAlias WbNetworkCookieJarBase;
class WbNetworkCookieJar : public WbNetworkCookieJarBase
{
  Q_OBJECT
  typedef WbNetworkCookieJarBase Base;

public:
  explicit WbNetworkCookieJar(QObject *parent = 0)
    : Base(".nicovideo.jp", ANNOT_PROXY_DOMAIN, parent)
  { }

protected:
  virtual QUrl toDomainUrl(const QUrl &aliasUrl) const; ///< \override
  QUrl decodeNicoUrl(const QUrl &url) const; ///< \overrride
};

#else
#  warning "nico proxy domain is not defined"
#  include <QtNetwork/QNetworkCookieJar>

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
