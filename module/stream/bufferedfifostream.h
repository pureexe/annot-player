#ifndef BUFFEREDFIFOSTREAM_H
#define BUFFEREDFIFOSTREAM_H

// bufferedfifostream.h
// 2/9/2012

#include "fifostream.h"
#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QMutex>

class BufferedFifoStream : public QObject, public FifoStream
{
  Q_OBJECT
  typedef BufferedFifoStream Self;
  typedef QObject Base;

  QMutex m_;
  QByteArray data_; // buffer
  qint64 pos_;
  bool finished_;

public:
  explicit BufferedFifoStream(QObject *parent = 0)
    : Base(parent), pos_(0), finished_(false) { }

signals:
  void finished();
  void readyRead();

public:
  bool isFinished() const { return finished_; }

  // - Shared -
  virtual qint64 size() const ///< \override
  { return FifoStream::size() ? FifoStream::size() : data_.size(); }

  virtual qint64 availableSize() const ///< \override
  { return data_.size(); }

  // - Input -
  virtual qint64 pos() const { return pos_; } ///< \override

  virtual bool reset() { m_.lock(); pos_ = 0; m_.unlock(); return true; } ///< \override

  virtual qint64 skip(qint64 count); ///< \override

  virtual bool seek(qint64 pos); ///< \override

  virtual qint64 read(char *data, qint64 maxSize); ///< \override

  virtual QByteArray readAll() ///< \override
  { if (!isFinished()) waitForFinished(); return data_; }

  QByteArray &data() { return data_; }
  const QByteArray &data() const { return data_; }

  virtual bool writeFile(const QString &path); ///< \override

  // - Output -

  virtual qint64 write(const char *data, qint64 maxSize); ///< \override
  virtual void finish(); ///< \override

public slots:
  virtual void waitForFinished();
  virtual void waitForReadyRead();
};

#endif // BUFFEREDFIFOSTREAM_H
