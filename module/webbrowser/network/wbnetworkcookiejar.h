#ifndef WBNETWORKCOOKIEJAR_H
#define WBNETWORKCOOKIEJAR_H

// network/wbnetworkcookiejar.h
// 1/27/2012

#ifdef CONFIG_PROXY_DOMAIN
# include "module/qtext/networkcookie.h"

typedef QtExt::NetworkCookieJarWithDomainAlias WbNetworkCookieJarBase;
class WbNetworkCookieJar : public WbNetworkCookieJarBase
{
  Q_OBJECT
  Q_DISABLE_COPY(WbNetworkCookieJar)
  typedef WbNetworkCookieJarBase Base;

public:
  explicit WbNetworkCookieJar(QObject *parent = nullptr)
    : Base(".nicovideo.jp", CONFIG_PROXY_DOMAIN, parent)
  { }

protected:
  QUrl toDomainUrl(const QUrl &aliasUrl) const override;
  QUrl decodeNicoUrl(const QUrl &url) const;
};

#else
# warning "nico proxy domain is not defined"
# include <QtNetwork/QNetworkCookieJar>

class WbNetworkCookieJar : public QNetworkCookieJar
{
  Q_OBJECT
  typedef QNetworkCookieJar Base;

public:
  explicit WbNetworkCookieJar(QObject *parent = nullptr)
    : Base(parent)
  { }
};

#endif // CONFIG_PROXY_DOMAIN

#endif // WBNETWORKCOOKIEJAR_H
