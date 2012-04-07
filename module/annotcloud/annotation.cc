// annotcloud/annotation.cc
// 7/24/2011

#include "module/annotcloud/annotation.h"
#include "module/qtext/hash.h"
#include "module/crypt/crypt.h"
#include <QMetaType>

using namespace AnnotCloud;
using namespace QtExt;

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
AnnotCloud::
Annotation::isValidText(const QString &text)
{
  return !text.isEmpty() && !text.trimmed().isEmpty();
}

// - Digest -

QByteArray
AnnotCloud::
Annotation::digest(const QByteArray &input)
{
  QByteArray ret = Crypt::hex(Crypt::sha1(input));
  return ret.toUpper();
}

// - Hash -

qint64
AnnotCloud::
Annotation::hash(const QByteArray &input)
{
  quint64 ret = Hash::djb2(input.constData());
  return (qint64)ret;
}

qint64
AnnotCloud::
Annotation::rehash(const QByteArray &input, qint64 h)
{
  quint64 ret =Hash::djb2(input.constData(), (quint64)h);
  return (qint64)ret;
}

// EOF
