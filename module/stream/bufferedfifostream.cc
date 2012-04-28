// bufferedfifostream.cc
// 2/15/2012

#include "module/stream/bufferedfifostream.h"
#include <QtCore/QEventLoop>
#include <QtCore/QFile>

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
  DOUT("enter: maxSize =" << maxSize);
  while (!finished_ && maxSize > data_.size() - pos_) {
    DOUT("data.size =" << data_.size());
    waitForReadyRead();
  }
  DOUT("data.size =" << data_.size() << ", finished =" << finished_);

  m_.lock();
  qint64 ret = qMin(data_.size() - pos_, maxSize);
  if (ret > 0) {
    ::memcpy(data, data_.constData() + pos_, ret);
    pos_ += ret;
  }
  m_.unlock();
  DOUT("exit: ret =" << ret);
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
  QMutexLocker lock(&m_);
  if (!finished_ || pos < data_.size()) {
    pos_ = pos;
    return true;
  } else
    return false;
}

qint64
BufferedFifoStream::skip(qint64 count)
{
  QMutexLocker lock(&m_);
  if (!finished_) {
    pos_ += count;
    return count;
  }

  qint64 ret = qMin(data_.size() - pos_, count);
  if (ret > 0)
    pos_ += ret;
  Q_ASSERT(ret >= 0);
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

bool
BufferedFifoStream::writeFile(const QString &path)
{
  DOUT("enter: fileName =" << path);
  QFile file(path);
  if (!file.open(QIODevice::WriteOnly)) {
    DOUT("exit: failed to write to file:" << path);
    return false;
  }

  bool ok = file.write(data_) == data_.size();

  DOUT("exit: ret =" << ok);
  return ok;
}
// EOF
