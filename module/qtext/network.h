#ifndef _QTEXT_NETWORK_H
#define _QTEXT_NETWORK_H

// qtext/network.h
// 2/21/2011

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

namespace QtExt {

  // - Constants -

  enum { MaxConcurrentNetworkRequestCount = 6 }; ///< See QNetworkAccessManager's documentation

  // - NetworkRequest -

  // See: http://www.developer.nokia.com/Community/Discussion/showthread.php?211356-QNetworkRequest-Authentication
  inline void
  setNetworkRequestAuthentificationHeader(QNetworkRequest *request, const QString &username, const QString &password)
  { request->setRawHeader("Authorization", "Basic " + QByteArray(QString("%1:%2").arg(username).arg(password).toAscii()).toBase64()); }

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

  // - Signals and slots -

  class NetworkReplyFinished : public QObject
  {
    Q_OBJECT
    Q_DISABLE_COPY(NetworkReplyFinished)
    typedef QObject Base;
    QNetworkReply *reply_;

  public:
    explicit NetworkReplyFinished(QNetworkReply *reply)
      : Base(0), reply_(reply)
    {
      Q_ASSERT(reply_);
      connect(reply_, SIGNAL(finished()), SLOT(trigger()));
    }

  signals:
    void finished(QNetworkReply *reply);

  protected slots:
    void trigger() { emit finished(reply_); }
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
    void trigger(qint64 receivedBytes, qint64 totalBytes)
    { emit progress(receivedBytes, totalBytes, id_); }
  };

} // namespace QtExt

#endif // _QTEXT_NETWORK_H
