#ifndef CRYPT_H
#define CRYPT_H

// crypt.h
// 9/4/2011

#include <QByteArray>
#include <QString>

namespace Crypt {

  QByteArray hex(const QByteArray &input);
  QByteArray sha1(const QByteArray &input);
  QByteArray md5(const QByteArray &input);

  enum { SimpleCryptKey = 0 };
  QByteArray encrypt(const QByteArray &input, quint64 key = SimpleCryptKey);
  QByteArray decrypt(const QByteArray &input, quint64 key = SimpleCryptKey);
  QString encrypt(const QString &input, quint64 key = SimpleCryptKey);
  QString decrypt(const QString &input, quint64 key = SimpleCryptKey);

} // namespace Crypt

#endif // CRYPT_H
