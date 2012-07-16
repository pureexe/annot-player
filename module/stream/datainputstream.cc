// datainputstream.cc
// 3/16/2012

#include "module/stream/datainputstream.h"
#include <QtCore/QFile>

//#define DEBUG "datainputstream"
#include "module/debug/debug.h"

bool
DataInputStream::writeFile(const QString &path)
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

qint64
DataInputStream::read(char *data, qint64 maxSize)
{
  qint64 count = qMin(maxSize, data_.size() - pos_);
  if (count > 0) {
    qMemCopy(data, data_.data() + pos_, count);
    pos_ += count;
  }
  return count;
}

// EOF
