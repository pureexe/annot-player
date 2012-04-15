#ifndef REMOTESTREAM_H
#define REMOTESTREAM_H

// remotestream.h
// 2/15/2012

#include "inputstream.h"
#include "module/qtext/stoppable.h"
#include <QObject>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QEventLoop>

class RemoteStream : public QObject, public InputStream, public StoppableTask
{
  Q_OBJECT
  typedef RemoteStream Self;
  typedef QObject Base;

  QNetworkAccessManager *nam_;
  qint64 size_;
  QNetworkRequest request_;

public:
  explicit RemoteStream(QObject *parent = 0)
    : Base(parent), size_(0)
  { nam_ = new QNetworkAccessManager(this); }

signals:
  void finished();
  void readyRead();
  void progress(qint64 bytesReceived, qint64 bytesTotal);
  void error(QString message);
  void stopped();

public:
  QNetworkAccessManager *networkAccessManager() const { return nam_; }
  const QNetworkRequest &request() const { return request_; }
  QNetworkRequest &request() { return request_; }
  QUrl url() const { return request_.url(); }
  virtual qint64 size() const { return size_; } ///< \override

  virtual QString contentType() const { return QString::null; }

  void waitForReady()
  {
    enum { max_redirect = 5 };
    int retries = max_redirect;
    do waitForReadyRead();
    while (retries--  && tryRedirect());
  }

public slots:
  virtual void run() = 0; ///< \override as slot
  virtual void stop() = 0; ///< \override as slot

  void setRequest(const QNetworkRequest &req) { request_ = req; }
  void setUrl(const QUrl &url) { request_.setUrl(url); }
  void setSize(const qint64 &size) { size_ = size; }

protected:
  virtual void waitForReadyRead()
  {
    QEventLoop loop;
    connect(this, SIGNAL(error(QString)), &loop, SLOT(quit()));
    connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
    connect(this, SIGNAL(readyRead()), &loop, SLOT(quit()));
    connect(this, SIGNAL(stopped()), &loop, SLOT(quit()));
    loop.exec();
  }

  virtual bool isRedirected() const { return false; }
  virtual bool redirect() { return false; }
  bool tryRedirect() { return isRedirected() && redirect(); }
};

#endif // IREMOTESTREAM_H
