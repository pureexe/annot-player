#ifndef REMOTESTREAM_H
#define REMOTESTREAM_H

// remotestream.h
// 2/15/2012

#include "module/stream/inputstream.h"
#include "module/qtext/stoppable.h"
#include <QtNetwork/QNetworkRequest>
#include <QtCore/QObject>
#include <QtCore/QUrl>

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)

class RemoteStream : public QObject, public InputStream, public StoppableTask
{
  Q_OBJECT
  Q_DISABLE_COPY(RemoteStream)
  typedef RemoteStream Self;
  typedef QObject Base;

  QNetworkAccessManager *nam_;
  qint64 size_;
  QNetworkRequest request_;

public:
  explicit RemoteStream(QNetworkAccessManager *nam, QObject *parent = 0)
    : Base(parent), nam_(nam), size_(0)
  { request_.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true); }

signals:
  void finished();
  void readyRead();
  void progress(qint64 bytesReceived, qint64 bytesTotal);
  void error(QString message);
  void stopped();

public:
  QNetworkAccessManager *networkAccessManager() const { return nam_; }
  void setNetworkAccessManager(QNetworkAccessManager *nam) { nam_ = nam; }

  const QNetworkRequest &request() const { return request_; }
  QNetworkRequest &request() { return request_; }
  QUrl url() const { return request_.url(); }
  virtual qint64 size() const { return size_; } ///< \reimp

  virtual QString contentType() const { return QString(); }

  void waitForReady();

public slots:
  virtual void run() = 0; ///< \reimp as slot
  virtual void stop() = 0; ///< \reimp as slot

  void setRequest(const QNetworkRequest &req) { request_ = req; }
  void setUrl(const QUrl &url) { request_.setUrl(url); }
  void setSize(const qint64 &size) { size_ = size; }

protected:
  virtual void waitForReadyRead();
  virtual bool isRedirected() const { return false; }
  virtual bool redirect() { return false; }
  bool tryRedirect() { return isRedirected() && redirect(); }
};

#endif // IREMOTESTREAM_H
