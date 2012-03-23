// inputstream.cc
// 2/14/2011

#include "inputstream.h"
#include <QtCore>

#define DEBUG "inputstream"
#include "module/debug/debug.h"

QByteArray
InputStream::readAll()
{
  DOUT("enter");
  qint64 count = size() - pos();
  if (count <= 0) {
    DOUT("exit: no left data");
    return QByteArray();
  }
  QByteArray ret(count, 0);
  qint64 n = read(ret);
  if (n <= 0) {
    DOUT("exit: read nothing");
    return QByteArray();
  }
  if (n < count) {
    DOUT("warning: read less then real size");
    ret.resize(n);
  }
  DOUT("exit: ret.size =" << ret.size());
  return ret;
}

bool
InputStream::writeFile(const QString &path)
{
  DOUT("enter: fileName =" << path);
  enum { BufferSize = 10240 };
  QFile file(path);
  if (!file.open(QIODevice::WriteOnly)) {
    DOUT("exit: failed to write to file:" << path);
    return false;
  }
  //if (isEmpty())
  //  return true;

  int bufsize = qMin((int)BufferSize, (int)size());
  if (bufsize <= 0)
    bufsize = BufferSize;
  char *buf = new char[bufsize];
  int count;
  reset();
  while ((count = read(buf, bufsize)) > 0)
    file.write(buf, count);

  delete[] buf;
  DOUT("exit: true");
  return true;
}

// EOF
