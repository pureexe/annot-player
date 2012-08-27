// networkcookie.cc
// 3/2/2012
#include "module/qtext/networkcookie.h"
#include <QtCore/QFile>

//#define DEBUG "qtext::networkcookie"
#include "module/debug/debug.h"

// - NetworkCookieJarWithDomainAlias -
// See: http://qt.gitorious.org/qt/mwenges-qt/blobs/755eceb17f6668620026cc2067ef91dd5aaf448d/src/network/access/qnetworkcookiejar.cpp
// See: http://qt.gitorious.org/qt/qt/blobs/67376be28ca51930ff0f4fad2dd58f53968655a9/src/network/access/qnetworkaccessmanager.cpp

// FIXME: partial implemented
bool
QtExt::
NetworkCookieJarWithDomainAlias::isParentDomain(const QString &domain, const QUrl &url)
{ return url.host().endsWith(domain, Qt::CaseInsensitive); }
//{ return url.toString().contains(domain, Qt::CaseInsensitive); }

// Used in QNetworkAccessManagerPrivate::createRequest
// See: qnetworkaccessmanager.cpp, http://qt.gitorious.org/qt/qt/blobs/67376be28ca51930ff0f4fad2dd58f53968655a9/src/network/access/qnetworkaccessmanager.cpp
QList<QNetworkCookie>
QtExt::
NetworkCookieJarWithDomainAlias::cookiesForUrl(const QUrl &url) const
{
  DOUT("enter: url =" << url);
  //DOUT("all cookies =" << allCookies());

  Q_ASSERT(!domain_.isEmpty() && !alias_.isEmpty());

  if (!(isParentDomain(domain_, url) ^ isParentDomain(alias_, url))) {
    DOUT("exit: url is not aliased");
    return Base::cookiesForUrl(url);
  }

  QList<QNetworkCookie> ret = Base::cookiesForUrl(toDomainUrl(domain_));
  ret.append(Base::cookiesForUrl(toAliasUrl(alias_)));
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
  Q_ASSERT(!domain_.isEmpty() && !alias_.isEmpty());
  if (isParentDomain(domain_, url) || !isParentDomain(alias_, url)) {
    DOUT("exit: url is not aliased");
    return Base::setCookiesFromUrl(cookies, url);
  }

  QList<QNetworkCookie> domainCookies;
  foreach (QNetworkCookie c, cookies) {
    c.setDomain(domain_);
    domainCookies.append(c);
  }

  DOUT("exit: aliased");
  return Base::setCookiesFromUrl(domainCookies, toDomainUrl(url));
}

QUrl
QtExt::
NetworkCookieJarWithDomainAlias::toDomainUrl(const QUrl &url) const
{
  Q_UNUSED(url)
  QString ret = domain_;
  if (ret.startsWith('.'))
    ret.prepend("www");
  if (!ret.contains("://"))
    ret.prepend("http://");
  return ret;
}

QUrl
QtExt::
NetworkCookieJarWithDomainAlias::toAliasUrl(const QUrl &url) const
{
  Q_UNUSED(url)
  QString ret = alias_;
  if (ret.startsWith('.'))
    ret.prepend("www");
  if (!ret.contains("://"))
    ret.prepend("http://");
  return ret;
}

bool
QtExt::
writeCookiesToFile(const QList<QNetworkCookie> &cookies, const QString &fileName)
{
  QFile f(fileName);
  bool ok = f.open(QIODevice::WriteOnly);
  if (ok) {
    QByteArray data = unparseNetworkCookies(cookies);
    ok = f.write(data) == data.size();
  }
  return ok;
}

QList<QNetworkCookie>
QtExt::
readCookiesfromFile(const QString &fileName)
{
  QList<QNetworkCookie> ret;
  QFile f(fileName);
  if (f.open(QIODevice::ReadOnly))
    ret = QNetworkCookie::parseCookies(f.readAll());
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
