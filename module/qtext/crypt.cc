// qtext/crypt.cc
// 9/4/2011

#include "qtext/crypt.h"
#include <QtCore>

QByteArray
QtExt::Crypt::
sha1(const QByteArray &input)
{
  QCryptographicHash digest(QCryptographicHash::Sha1);
  digest.addData(input);
  return digest.result();
}

QByteArray
QtExt::Crypt::
md5(const QByteArray &input)
{
  QCryptographicHash digest(QCryptographicHash::Md5);
  digest.addData(input);
  return digest.result();
}

QByteArray
QtExt::Crypt::
hex(const QByteArray &input)
{ return input.toHex(); }

// EOF
