// annotcloud/annotation.cc
// 7/24/2011

#include "lib/annotcloud/annotation.h"
#include "lib/crypt/crypt.h"
#include "qtx/qxhash.h"

using namespace AnnotCloud;

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
  quint64 ret = QxHash::djb2(input.constData());
  return qint64(ret);
}

qint64
AnnotCloud::
Annotation::rehash(const QByteArray &input, qint64 h)
{
  quint64 ret = QxHash::djb2(input.constData(), quint64(h));
  return qint64(ret);
}

qint64
AnnotCloud::
Annotation::hash(const QList<QByteArray> &l)
{
  qint64 h;
  if (l.isEmpty())
    h = hash(QByteArray());
  else for (int i = 0; i < l.size(); i++)
    if (i == 0)
      h = hash(l.first());
    else
      h = rehash(l[i], h);
  return h;
}

// EOF
