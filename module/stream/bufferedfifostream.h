#ifndef BUFFEREDFIFOSTREAM_H
#define BUFFEREDFIFOSTREAM_H

// bufferedfifostream.h
// 2/9/2012

#include "module/stream/fifostream.h"
#include <QtCore/QByteArray>
#include <QtCore/QMutex>
#include <QtCore/QObject>

class BufferedFifoStream : public QObject, public FifoStream
{
  Q_OBJECT
  Q_DISABLE_COPY(BufferedFifoStream)
  typedef BufferedFifoStream Self;
  typedef QObject Base;

  QMutex m_;
  QByteArray data_; // buffer
  qint64 pos_;
  bool finished_;

public:
  explicit BufferedFifoStream(QObject *parent = nullptr)
    : Base(parent), pos_(0), finished_(false) { }

signals:
  void finished();
  void readyRead();

public:
  bool isFinished() const { return finished_; }

  // - Shared -
  qint64 size() const override
  { return FifoStream::size() ? FifoStream::size() : data_.size(); }

  qint64 availableSize() const override
  { return data_.size(); }

  // - Input -
  qint64 pos() const override { return pos_; }

  bool reset() override { m_.lock(); pos_ = 0; m_.unlock(); return true; }

  qint64 skip(qint64 count) override;

  bool seek(qint64 pos) override;

  qint64 read(char *data, qint64 maxSize) override;
  qint64 tryRead(char *data, qint64 maxSize) override;

  QByteArray readAll() override
  { if (!isFinished()) waitForFinished(); return data_; }

  QByteArray &data() { return data_; }
  const QByteArray &data() const { return data_; }

  bool writeFile(const QString &path) override;

  // - Output -

  qint64 write(const char *data, qint64 maxSize) override;
  void finish() override;

public slots:
  virtual void waitForFinished();
  virtual void waitForReadyRead();
};

#endif // BUFFEREDFIFOSTREAM_H
