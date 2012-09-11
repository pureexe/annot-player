#ifdef CONFIG_PROXY_DOMAIN

// network/wbnetworkcookiejar.cc
// 4/21/2012

#include "network/wbnetworkcookiejar.h"
#include <QtCore/QRegExp>

#define DEBUG "wbnetworkcookiejar"
#include "module/debug/debug.h"

QUrl
WbNetworkCookieJar::toDomainUrl(const QUrl &url) const
{
  if (url == domain())
    return Base::toDomainUrl(url);
  else if (url.host() == CONFIG_PROXY_DOMAIN && url.path().startsWith("/nico/"))
    return decodeNicoUrl(url);
  else
    return Base::toDomainUrl(url);
}

QUrl
WbNetworkCookieJar::decodeNicoUrl(const QUrl &url) const
{
  Q_ASSERT(url.host() == CONFIG_PROXY_DOMAIN);

  QRegExp rx("/nico/([a-z]+)(.*)", Qt::CaseInsensitive);
  if (!rx.exactMatch(url.path()))
    return url;
  Q_ASSERT(rx.captureCount() == 2);
  QString host = rx.cap(1),
          path = rx.cap(2);

  if (host.isEmpty())
    host = "nicovideo.jp";
  else
    host.append(".nicovideo.jp");

  QUrl ret = url;
  ret.setHost(host);
  ret.setPath(path);
  DOUT("ret =" << ret.toString());
  return ret;
}

#endif // CONFIG_PROXY_DOMAIN
