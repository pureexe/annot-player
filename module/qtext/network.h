#ifndef _QTEXT_NETWORK_H
#define _QTEXT_NETWORK_H

// qtext/network.h
// 2/21/2011

#include <QNetworkCookieJar>

namespace QtExt {

  // - NetworkCookieJarWithCookies -

  class NetworkCookieJarWithCookies : public QNetworkCookieJar
  {
    typedef QNetworkCookieJar Base;
    typedef NetworkCookieJarWithCookies Self;

    NetworkCookieJarWithCookies() { }

  public:
    static Self *fromCookieJar(Base *base)
    { return reinterpret_cast<Self *>(base); }

    static const Self *fromCookieJar(const Base *base)
    { return reinterpret_cast<const Self *>(base); }

  public:
    QList<QNetworkCookie> allCookies() const
    { return Base::allCookies(); }

    void setAllCookies(const QList<QNetworkCookie> &cookies)
    { return Base::setAllCookies(cookies); }
  };

  inline QList<QNetworkCookie>
  allCookiesInNetworkCookieJar(const QNetworkCookieJar *jar)
  {
    Q_ASSERT(jar);
    return NetworkCookieJarWithCookies::fromCookieJar(jar)->allCookies();

  }

  inline void
  setAllCookiesInNetworkCookieJar(QNetworkCookieJar *jar, const QList<QNetworkCookie> &cookies)
  {
    Q_ASSERT(jar);
    NetworkCookieJarWithCookies::fromCookieJar(jar)->setAllCookies(cookies);
  }

  // - NetworkCookieJarWithDomainAlias -

  class NetworkCookieJarWithDomainAlias : public QNetworkCookieJar
  {
    Q_OBJECT
    typedef QNetworkCookieJar Base;
    typedef NetworkCookieJarWithDomainAlias Self;

    QString domain_, alias_;

  public:
    explicit NetworkCookieJarWithDomainAlias(QObject *parent = 0)
      : Base(parent) { }

   NetworkCookieJarWithDomainAlias(const QString &domain, const QString &alias,
                                   QObject *parent = 0)
      : Base(parent), domain_(domain), alias_(alias) { }

  public:
    virtual QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const; ///< \override
    virtual bool setCookiesFromUrl(const QList<QNetworkCookie> &cookies, const QUrl &url); ///< \override

  public:
    void setDomainAlias(const QString &domain, const QString &alias)
    { domain_ = domain; alias_ = alias; }

  protected:
    static bool isParentDomain(const QString &domain, const QUrl &url);
    static QUrl domainUrl(const QString &domain);
  };


} // namespace QtExt

#endif // _QTEXT_NETWORK_H
