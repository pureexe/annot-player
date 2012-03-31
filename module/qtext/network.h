#ifndef _QTEXT_NETWORK_H
#define _QTEXT_NETWORK_H

// qtext/network.h
// 2/21/2011

#include <QNetworkCookieJar>
#include <QNetworkReply>

namespace QtExt {

  // - PublicNetworkReply -
  class PublicNetworkReply : public QNetworkReply
  {
    typedef PublicNetworkReply Self;
    typedef QNetworkReply Base;

  public:
    static Self *fromReply(Base *base)
    { return reinterpret_cast<Self *>(base); }

    static const Self *fromReply(const Base *base)
    { return reinterpret_cast<const Self *>(base); }

  public:
    void setAttribute(QNetworkRequest::Attribute code, const QVariant &value)
    { Base::setAttribute(code, value); }

    void setError(NetworkError errorCode, const QString &errorString )
    { Base::setError(errorCode, errorString); }

    void setHeader(QNetworkRequest::KnownHeaders header, const QVariant &value)
    { Base::setHeader(header, value); }

    void setOperation(QNetworkAccessManager::Operation operation)
    { Base::setOperation(operation); }

    void setRawHeader(const QByteArray & headerName, const QByteArray &value)
    { Base::setRawHeader(headerName, value); }

    void setRequest(const QNetworkRequest &request)
    { Base::setRequest(request); }

    void setUrl(const QUrl &url)
    { Base::setUrl(url); }
  };

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
    typedef NetworkCookieJarWithDomainAlias Self;
    typedef QNetworkCookieJar Base;

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


  class ProgressWithId : public QObject
  {
    Q_OBJECT
    typedef QObject Base;

    long id_;

  public:
    ProgressWithId(long id, QObject *parent = 0)
      : Base(parent), id_(id) { }

  signals:
    void progress(qint64 receivedBytes, qint64 totalBytes, long id);

  public slots:
    void emit_progress(qint64 receivedBytes, qint64 totalBytes)
    { emit progress(receivedBytes, totalBytes, id_); }
  };

} // namespace QtExt

#endif // _QTEXT_NETWORK_H
