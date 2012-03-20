// datainputstream.cc
// 3/16/2012

#include "datainputstream.h"
#include <QtCore>

//#define DEBUG "datainputstream"
#include "module/debug/debug.h"

bool
DataInputStream::writeToFile(const QString &path)
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
