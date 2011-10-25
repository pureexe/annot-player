// core/data/annotation.cc
// 7/24/2011

#include "core/data/annotation.h"
#include "core/util/crypt.h"
#include "core/util/hash.h"
#include <QtCore>

// - Validations -

bool
Core::Data::
Annotation::isValidText(const QString &text)
{
  return !text.isEmpty() && !text.trimmed().isEmpty();
}

// - Digest -

QByteArray
Core::Data::
Annotation::digest(const QByteArray &input)
{
  QByteArray ret = Crypt::hex(Crypt::sha1(input));
  return ret.toUpper();
}

// - Hash -

qint64
Core::Data::
Annotation::hash(const QByteArray &input)
{
  quint64 ret =Hash::djb2(input.constData());
  return (qint64)ret;
}

qint64
Core::Data::
Annotation::rehash(const QByteArray &input, qint64 h)
{
  quint64 ret =Hash::djb2(input.constData(), (quint64)h);
  return (qint64)ret;
}

// EOF
