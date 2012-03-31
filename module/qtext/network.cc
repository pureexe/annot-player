// network.cc
// 3/2/2012
#include "module/qtext/network.h"
#include <QtNetwork>

//#define DEBUG "qtext::network"
#include "module/debug/debug.h"

// - NetworkCookieJarWithDomainAlias -
// See: http://qt.gitorious.org/qt/mwenges-qt/blobs/755eceb17f6668620026cc2067ef91dd5aaf448d/src/network/access/qnetworkcookiejar.cpp
// See: http://qt.gitorious.org/qt/qt/blobs/67376be28ca51930ff0f4fad2dd58f53968655a9/src/network/access/qnetworkaccessmanager.cpp

// FIXME: partial implemented
bool
QtExt::
NetworkCookieJarWithDomainAlias::isParentDomain(const QString &domain, const QUrl &url)
{ return url.host().contains(domain, Qt::CaseInsensitive); }
//{ return url.toString().contains(domain, Qt::CaseInsensitive); }

// Used in QNetworkAccessManagerPrivate::createRequest
// See: qnetworkaccessmanager.cpp, http://qt.gitorious.org/qt/qt/blobs/67376be28ca51930ff0f4fad2dd58f53968655a9/src/network/access/qnetworkaccessmanager.cpp
QList<QNetworkCookie>
QtExt::
NetworkCookieJarWithDomainAlias::cookiesForUrl(const QUrl &url) const
{
  DOUT("enter: url =" << url);
  //DOUT("all cookies =" << allCookies());

  if (domain_.isEmpty() || alias_.isEmpty() ||
      !(isParentDomain(domain_, url) ^ isParentDomain(alias_, url))) {
    DOUT("exit: url is not aliased");
    return Base::cookiesForUrl(url);
  }

  QList<QNetworkCookie> ret = Base::cookiesForUrl(domainUrl(domain_));
  ret.append(Base::cookiesForUrl(domainUrl(alias_)));
  DOUT("exit: ret =" << ret);
  return ret;
}

// Used in QNetworkReply,
// See: qt/src/access/qnetworkreplyimpl.cpp
bool
QtExt::
NetworkCookieJarWithDomainAlias::setCookiesFromUrl(const QList<QNetworkCookie> &cookies, const QUrl &url)
{
  DOUT("enter: url =" << url);
  if (cookies.isEmpty()) {
    DOUT("exit: empty cookies");
    return false;
  }
  if (domain_.isEmpty() || alias_.isEmpty() ||
      isParentDomain(domain_, url) || !isParentDomain(alias_, url)) {
    DOUT("exit: url is not aliased");
    return Base::setCookiesFromUrl(cookies, url);
  }

  QList<QNetworkCookie> domainCookies;
  foreach (QNetworkCookie c, cookies) {
    c.setDomain(domain_);
    domainCookies.append(c);
  }

  DOUT("exit: aliased");
  return Base::setCookiesFromUrl(domainCookies, domainUrl(domain_));
}

QUrl
QtExt::
NetworkCookieJarWithDomainAlias::domainUrl(const QString &domain)
{
  QString ret = domain;
  if (ret.startsWith('.'))
    ret.prepend("www");
  if (!ret.contains("://"))
    ret.prepend("http://");
  return ret;
}

// EOF

/*
// See: qnetworkreplyimpl.cpp
void QNetworkReplyImplPrivate::metaDataChanged()
{
    Q_Q(QNetworkReplyImpl);
    // 1. do we have cookies?
    // 2. are we allowed to set them?
    if (cookedHeaders.contains(QNetworkRequest::SetCookieHeader) && !manager.isNull()
        && (static_cast<QNetworkRequest::LoadControl>
            (request.attribute(QNetworkRequest::CookieSaveControlAttribute,
                               QNetworkRequest::Automatic).toInt()) == QNetworkRequest::Automatic)) {
        QList<QNetworkCookie> cookies =
            qvariant_cast<QList<QNetworkCookie> >(cookedHeaders.value(QNetworkRequest::SetCookieHeader));
        QNetworkCookieJar *jar = manager->cookieJar();
        if (jar)
            jar->setCookiesFromUrl(cookies, url);
    }
    emit q->metaDataChanged();
}
*/
