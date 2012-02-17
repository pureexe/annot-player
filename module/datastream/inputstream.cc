// inputstream.cc
// 2/14/2011

#include "inputstream.h"
#include <QtCore>

QByteArray
InputStream::readAll()
{
  int count = (int)size();
  if (count <= 0)
    return QByteArray();
  QByteArray ret(count, 0);
  int n = (int)read(ret.data(), count);
  if (n <= 0)
    return QByteArray();
  if (n < count)
    ret.resize(n);
  return ret;
}

bool
InputStream::writeToFile(const QString &path)
{
  QFile file(path);
  if (!file.open(QIODevice::WriteOnly))
    return false;
  //if (isEmpty())
  //  return true;

  int bufsize = qMin(1024, (int)size());
  if (bufsize <= 0)
    bufsize = 1024;
  char *buf = new char[bufsize];
  int count;
  reset();
  while ((count = read(buf, bufsize)) > 0)
    file.write(buf, count);

  delete[] buf;
  return true;
}

// EOF
