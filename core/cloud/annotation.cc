// core/cloud/annotation.cc
// 7/24/2011

#include "core/cloud/annotation.h"
#include "core/util/crypt.h"
#include "core/util/hash.h"
#include <QtCore>

using namespace Core::Cloud;

// - Meta type -

namespace { // anonymous
  struct init_ { init_() {
    qRegisterMetaType<Annotation>("Annotation");
    qRegisterMetaType<AnnotationList>("AnnotationList");
  } };
  init_ static_init_;
} // anonymous namespace


// - Validations -

bool
Core::Cloud::
Annotation::isValidText(const QString &text)
{
  return !text.isEmpty() && !text.trimmed().isEmpty();
}

// - Digest -

QByteArray
Core::Cloud::
Annotation::digest(const QByteArray &input)
{
  QByteArray ret = Crypt::hex(Crypt::sha1(input));
  return ret.toUpper();
}

// - Hash -

qint64
Core::Cloud::
Annotation::hash(const QByteArray &input)
{
  quint64 ret = Hash::djb2(input.constData());
  return (qint64)ret;
}

qint64
Core::Cloud::
Annotation::rehash(const QByteArray &input, qint64 h)
{
  quint64 ret =Hash::djb2(input.constData(), (quint64)h);
  return (qint64)ret;
}

// EOF
