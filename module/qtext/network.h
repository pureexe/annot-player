#ifndef _QTEXT_NETWORK_H
#define _QTEXT_NETWORK_H

// qtext/network.h
// 2/21/2011

#include <QNetworkCookieJar>

namespace QtExt {

  class NetworkCookieJarWithCookies : public QNetworkCookieJar
  {
  public:
    QList<QNetworkCookie> allCookies() const
    { return QNetworkCookieJar::allCookies(); }
  };

  inline QList<QNetworkCookie>
  allCookiesInNetworkCookieJar(const QNetworkCookieJar *jar)
  {
    Q_ASSERT(jar);
    return reinterpret_cast<const NetworkCookieJarWithCookies*>(jar)->allCookies();

  }

} // namespace QtExt

#endif // _QTEXT_NETWORK_H
