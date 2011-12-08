#ifndef IOUTIL_H
#define IOUTIL_H

// ioutil.h
// 12/6/2011

#include <QString>

namespace IOUtil {

  QByteArray readBytes(const QString &filePath, qint64 readSize);

} // namespace IOUtil

#endif // IOUTIL_H
