#ifndef _QTEXT_NETWORK_H
#define _QTEXT_NETWORK_H

// qtext/network.h
// 2/21/2011

#include <QtNetwork/QNetworkReply>

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

  // - Helpers -

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
