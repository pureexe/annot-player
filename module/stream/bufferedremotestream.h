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
  explicit BufferedRemoteStream(QNetworkAccessManager *nam, QObject *parent = 0)
    : Base(nam, parent), pos_(0), reply_(0), bufferSize_(0), stopped_(false) { }

  ~BufferedRemoteStream();

public:
  virtual qint64 size() const { return Base::size() ? Base::size() : data_.size(); } ///< \reimp
  qint64 availableSize() const;

  virtual qint64 pos() const { return pos_; } ///< \reimp

  virtual qint64 read(char *data, qint64 maxSize); ///< \reimp
  virtual qint64 tryRead(char *data, qint64 maxSize); ///< \reimp

  virtual QByteArray readAll(); ///< \reimp

  virtual bool reset() { m_.lock(); pos_ = 0; m_.unlock(); return true; } ///< \reimp
  virtual bool seek(qint64 pos); ///< \reimp

  virtual qint64 skip(qint64 count); ///< \reimp

  virtual QString contentType() const; ///< \reimp

  QByteArray &data() { return data_; }
  const QByteArray &data() const { return data_; }

  virtual bool writeFile(const QString &path); ///< \reimp

  bool isRunning() const { return reply_ && reply_->isRunning(); }
  bool isFinished() const { return reply_ && reply_->isFinished(); }

  void waitForFinished();
  virtual void waitForReadyRead(); ///< \reimp

public slots:

  virtual void run(); ///< \reimp
  virtual void stop(); ///< \reimp

  virtual void setBufferSize(qint64 size) { bufferSize_ = size; } ///< \reimp

protected slots:
  void finish();
  void updateSize();
  void networkError();

protected:
  bool isRedirected() const; ///< override
  bool redirect(); ///< \reimp
};

#endif // BUFFEREDREMOTESTREAM_H
