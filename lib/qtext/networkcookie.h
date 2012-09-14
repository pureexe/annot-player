#ifndef _QTEXT_NETWORKCOOKIE_H
#define _QTEXT_NETWORKCOOKIE_H

// qtext/networkcookie.h
// 2/21/2011

#include <QtNetwork/QNetworkCookieJar>

namespace QtExt {

  // - NetworkCookieJarWithCookies -

  class NetworkCookieJarWithCookies : public QNetworkCookieJar
  {
    typedef NetworkCookieJarWithCookies Self;
    typedef QNetworkCookieJar Base;

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

    QByteArray rawCookies() const
    { return unparseCookies(allCookies()); }

    static QByteArray unparseCookies(const QList<QNetworkCookie> &cookies)
    {
      QByteArray ret;
      foreach (const QNetworkCookie &c, cookies)
        ret.append(c.toRawForm()).append('\n');
      return ret;
    }
  };

  inline QList<QNetworkCookie>
  allCookiesInNetworkCookieJar(const QNetworkCookieJar *jar)
  { return NetworkCookieJarWithCookies::fromCookieJar(jar)->allCookies();  }

  inline void
  setAllCookiesInNetworkCookieJar(QNetworkCookieJar *jar, const QList<QNetworkCookie> &cookies)
  { NetworkCookieJarWithCookies::fromCookieJar(jar)->setAllCookies(cookies); }

  inline QByteArray
  unparseNetworkCookies(const QList<QNetworkCookie> &cookies)
  { return NetworkCookieJarWithCookies::unparseCookies(cookies); }

  inline QByteArray
  unparseNetworkCookies(const QNetworkCookieJar *jar)
  { return unparseNetworkCookies(allCookiesInNetworkCookieJar(jar)); }

  template <typename T>
  inline T*
  cloneNetworkCookieJar(const T *jar)
  {
    Q_ASSERT(jar);
    T *ret = new T(jar->parent());
    setAllCookiesInNetworkCookieJar(ret, allCookiesInNetworkCookieJar(jar));
    return ret;
  }

  bool
  writeCookiesToFile(const QList<QNetworkCookie> &cookies, const QString &fileName);

  QList<QNetworkCookie>
  readCookiesfromFile(const QString &fileName);

  inline bool
  writeCookiesToFile(const QNetworkCookieJar *jar, const QString &fileName)
  { return writeCookiesToFile(allCookiesInNetworkCookieJar(jar), fileName); }

  inline bool
  readCookiesfromFile(QNetworkCookieJar *jar, const QString &fileName)
  {
    QList<QNetworkCookie> c = readCookiesfromFile(fileName);
    if (c.isEmpty())
      return false;
    setAllCookiesInNetworkCookieJar(jar, c);
    return true;
  }

  // - NetworkCookieJarWithDomainAlias -

  class NetworkCookieJarWithDomainAlias : public QNetworkCookieJar
  {
    Q_OBJECT
    Q_DISABLE_COPY(NetworkCookieJarWithDomainAlias)
    typedef NetworkCookieJarWithDomainAlias Self;
    typedef QNetworkCookieJar Base;

    QString domain_, alias_;

  public:
    explicit NetworkCookieJarWithDomainAlias(QObject *parent = nullptr)
      : Base(parent) { }

   NetworkCookieJarWithDomainAlias(const QString &domain, const QString &alias,
                                   QObject *parent = nullptr)
      : Base(parent), domain_(domain), alias_(alias) { }

   const QString &domain() const { return domain_; }
   const QString &alias() const { return alias_; }

  public:
    QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const override;
    bool setCookiesFromUrl(const QList<QNetworkCookie> &cookies, const QUrl &url) override;

  public:
    void setDomainAlias(const QString &domain, const QString &alias)
    { domain_ = domain; alias_ = alias; }

  protected:
    virtual QUrl toDomainUrl(const QUrl &aliasUrl) const;
    virtual QUrl toAliasUrl(const QUrl &domainUrl) const;
    static bool isParentDomain(const QString &domain, const QUrl &url);
  };

} // namespace QtExt

#endif // _QTEXT_NETWORKCOOKIE_H
