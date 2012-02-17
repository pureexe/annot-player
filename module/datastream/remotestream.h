#ifndef REMOTESTREAM_H
#define REMOTESTREAM_H

// remotestream.h
// 2/15/2012

#include "inputstream.h"
#include <QObject>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QRunnable>

class RemoteStream : public QObject, public QRunnable, public InputStream
{
  Q_OBJECT
  typedef RemoteStream Self;
  typedef QObject Base;

  QNetworkAccessManager *nam_;
  qint64 size_;
  QNetworkRequest request_;

public:
  explicit RemoteStream(QNetworkAccessManager *nam, QObject *parent = 0)
    : Base(parent), nam_(nam), size_(0) { init(); }

  explicit RemoteStream(QObject *parent = 0)
    : Base(parent), nam_(0), size_(0) { init(); }

private:
  void init() { if (!nam_) nam_ = new QNetworkAccessManager(this); }

signals:
  void finished();
  void readyRead();
  void progress(qint64 bytesReceived, qint64 bytesTotal);

public:
  QNetworkAccessManager *networkAccessManager() const { return nam_; }
  const QNetworkRequest &request() const { return request_; }
  QNetworkRequest &request() { return request_; }
  QUrl url() const { return request_.url(); }
  virtual qint64 size() const { return size_; } ///< \override

public slots:
  virtual void run() = 0;

  void setRequest(const QNetworkRequest &req) { request_ = req; }
  void setUrl(const QUrl &url) { request_.setUrl(url); }
  void setSize(const qint64 &size) { size_ = size; }
};

#endif // IREMOTESTREAM_H
