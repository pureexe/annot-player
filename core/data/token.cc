// core/data/token.cc
// 8/7/2011

#include "core/data/token.h"
#include "core/util/crypt.h"

#include <QtCore>
#ifdef Q_WS_WIN
  #include "win/disk/disk.h"
#endif // Q_WS_WIN

#define DIGEST(_bytes)  Core::Crypt::md5(_bytes)
enum { DIGEST_SIZE = 10 * 1024 * 1024 }; // 10 MB

QString
Core::Data::
Token::digestFromFile(const QString &fileName)
{
  qDebug() << "Core::Token::hash: enter:" << fileName;

  QByteArray data;
#ifdef Q_WS_WIN
  QString deviceFileName = Disk::guessDeviceFileName(fileName);
  if (Disk::isValidDeviceFileName(deviceFileName)) {
    Disk disk(deviceFileName);
    bool succeed = disk.open(QIODevice::ReadOnly);
    if (!succeed) {
      qDebug() << "Core::Token: Failed to open device disk for hashing: " << fileName;
      return QByteArray();
    }
    data = disk.read(DIGEST_SIZE);
    disk.close();
  }
  else
#endif // Q_WS_WIN
  {
    QFile file(fileName);
    bool succeed = file.open(QIODevice::ReadOnly);
    if (!succeed) {
      qDebug() << "Core::Token: Failed to open file for hashing: " << fileName;
      return QByteArray();
    }

    if (file.size() <= DIGEST_SIZE)
      data = file.readAll();
    else
      data = file.read(DIGEST_SIZE);
    file.close();
  }

  if (data.isEmpty()) {
    qDebug() << "Core::Token: Error: get empty data to hash: " << fileName;
    return QByteArray();
  }

  QByteArray ret = DIGEST(data);
  qDebug() << "Core::Token::hash: exit";
  return ret.toHex().toUpper();
}

// EOF
