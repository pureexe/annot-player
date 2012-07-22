#ifndef BUFFEREDREMOTESTREAM_H
#define BUFFEREDREMOTESTREAM_H

// bufferedremotestream.h
// 2/15/2012

#include "module/stream/remotestream.h"
#include <QtNetwork/QNetworkReply>
#include <QtCore/QMutex>

class BufferedRemoteStream :  public RemoteStream
{
  Q_OBJECT
  Q_DISABLE_COPY(BufferedRemoteStream)
  typedef BufferedRemoteStream Self;
  typedef RemoteStream Base;

  QMutex m_;
  QByteArray data_;
  qint64 pos_;
  QNetworkReply *reply_;
  QUrl redirectUrl_;
  qint64 bufferSize_;

  bool stopped_;

public:
  explicit BufferedRemoteStream(QNetworkAccessManager *nam, QObject *parent = nullptr)
    : Base(nam, parent), pos_(0), reply_(nullptr), bufferSize_(0), stopped_(false) { }

  ~BufferedRemoteStream();

public:
  qint64 size() const override { return Base::size() ? Base::size() : data_.size(); }
  qint64 availableSize() const;

  qint64 pos() const override { return pos_; }

  qint64 read(char *data, qint64 maxSize) override;
  qint64 tryRead(char *data, qint64 maxSize) override;

  QByteArray readAll() override;

  bool reset() override { m_.lock(); pos_ = 0; m_.unlock(); return true; }
  bool seek(qint64 pos) override;

  qint64 skip(qint64 count) override;

  QString contentType() const override;

  QByteArray &data() { return data_; }
  const QByteArray &data() const { return data_; }

  bool writeFile(const QString &path) override;

  bool isRunning() const { return reply_ && reply_->isRunning(); }
  bool isFinished() const { return reply_ && reply_->isFinished(); }

  void waitForFinished();
  void waitForReadyRead() override;

public slots:

  void run() override;
  void stop() override;

  void setBufferSize(qint64 size) override { bufferSize_ = size; }

protected slots:
  void finish();
  void updateSize();
  void networkError();

protected:
  bool isRedirected() const; ///< override
  bool redirect() override;
};

#endif // BUFFEREDREMOTESTREAM_H
