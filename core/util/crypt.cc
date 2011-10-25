// core/util/crypt.cc
// 9/4/2011

#include "core/util/crypt.h"
#include <QtCore>

QByteArray
Core::Crypt::
sha1(const QByteArray &input)
{
  QCryptographicHash digest(QCryptographicHash::Sha1);
  digest.addData(input);
  return digest.result();
}

QByteArray
Core::Crypt::
md5(const QByteArray &input)
{
  QCryptographicHash digest(QCryptographicHash::Md5);
  digest.addData(input);
  return digest.result();
}

QByteArray
Core::Crypt::
hex(const QByteArray &input)
{ return input.toHex(); }

// EOF
