// bufferedfifostream.cc
// 2/15/2012

#include "bufferedfifostream.h"
#include <QtCore>

//#define DEBUG "bufferedfifostream"
#include "module/debug/debug.h"

void
BufferedFifoStream::finish()
{
  flush();
  setSize(data_.size());
  finished_ = true;
  emit finished();
}

qint64
BufferedFifoStream::read(char *data, qint64 maxSize) ///< \override
{
  while (!finished_ && maxSize > data_.size())
    waitForReadyRead();

  m_.lock();
  qint64 ret = qMin(data_.size() - pos_, maxSize);
  if (ret > 0) {
    ::memcpy(data, data_.constData() + pos_, ret);
    pos_ += ret;
  }
  m_.unlock();
  return ret;
}

qint64
BufferedFifoStream::write(const char *data, qint64 maxSize)
{
  Q_ASSERT(!finished_);
  if (maxSize > 0) {
    m_.lock();
    data_.append(data, maxSize);
    m_.unlock();
    emit readyRead();
  }
  return maxSize;
}

bool
BufferedFifoStream::seek(qint64 pos)
{
  m_.lock();
  if (!finished_ || pos < data_.size()) {
    pos_ = pos;
    m_.unlock();
    return true;
  } else {
    m_.unlock();
    return false;
  }
}

qint64
BufferedFifoStream::skip(qint64 count)
{
  m_.lock();
  if (!finished_) {
    pos_ += count;
    m_.unlock();
    return count;
  }

  qint64 ret = qMin(data_.size() - pos_, count);
  if (ret > 0)
    pos_ += ret;
  Q_ASSERT(ret >= 0);
  m_.unlock();
  return ret;
}

void
BufferedFifoStream::waitForReadyRead()
{
  DOUT("enter");
  QEventLoop loop;
  connect(this, SIGNAL(readyRead()), &loop, SLOT(quit()));
  connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
  loop.exec();
  DOUT("exit");
}

void
BufferedFifoStream::waitForFinished()
{
  DOUT("enter");
  QEventLoop loop;
  connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
  loop.exec();
  DOUT("exit");
}

// EOF
