#ifndef NICOUTIL_H
#define NICOUTIL_H

// nicoutil.h
// 3/14/2012

#include <QString>

QT_FORWARD_DECLARE_CLASS(QNetworkCookieJar)

namespace nico {

  bool login(const QString &username, const QString &password, QNetworkCookieJar *cookieJar, int retries = 3);

} // namespace nico

namespace bilibili {

  bool login(const QString &username, const QString &password, QNetworkCookieJar *cookieJar, int retries = 3);

} // namespace bilibili

#endif // NICOUTIL_H
