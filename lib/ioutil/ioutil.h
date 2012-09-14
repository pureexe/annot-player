#ifndef IOUTIL_H
#define IOUTIL_H

// ioutil.h
// 12/6/2011

#include <QtCore/QString>

namespace IOUtil {

  QByteArray readBytes(const QString &filePath, qint64 readSize);

  bool isAudioCD(const QString &filePath);

} // namespace IOUtil

#endif // IOUTIL_H
