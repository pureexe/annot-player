// annotationcodecmanager.cc
// 2/4/2012
#include "module/annotcodec/annotationcodecmanager.h"
#include "module/annotcodec/acfuncodec.h"
#include "module/annotcodec/bilibilicodec.h"
#include "module/annotcodec/nicovideocodec.h"

//#define DEBUG "annotationcodecmanager"
#include "module/debug/debug.h"

using namespace AnnotCloud;

// - Construction -

AnnotationCodecManager::AnnotationCodecManager(QObject *parent)
  : Base(parent)
{
  AnnotationCodec *c;
#define ADD(_codec) \
  c = new _codec(this); { \
    connect(c, SIGNAL(error(QString)), SIGNAL(error(QString))); \
    connect(c, SIGNAL(message(QString)), SIGNAL(message(QString))); \
    connect(c, SIGNAL(fetched(AnnotationList,QString)), SIGNAL(fetched(AnnotationList,QString))); \
  } codecs_.append(c);

  ADD(NicovideoCodec)
  ADD(AcfunCodec)
  ADD(BilibiliCodec)
#undef ADD
}

// - Analysis -

int
AnnotationCodecManager::match(const QString &url) const
{
  for (int i = 0; i < codecs_.size(); i++)
    if (codecs_[i]->match(url))
      return i;
  return -1;
}

void
AnnotationCodecManager::fetch(int id, const QString &url)
{
  if (id < 0 || id > codecs_.size()) {
    Q_ASSERT(0);
    DOUT("illegal state: codec id out of bounds");
    return;
  }
  codecs_[id]->fetch(url);
}

// - Parse -

AnnotationList
AnnotationCodecManager::parseFile(const QString &fileName)
{
  Site site = fileSite(fileName);
  DOUT("site =" << site << ", fileName =" << fileName);
  switch (site) {
  case Nicovideo: return NicovideoCodec::parseFile(fileName);
  case Acfun:     return AcfunCodec::parseFile(fileName);
  case Bilibili:  return BilibiliCodec::parseFile(fileName);
  case UnknownSite:
  default:        return AnnotationList();
  }
}

AnnotationCodecManager::Site
AnnotationCodecManager::fileSite(const QString &fileName)
{
  enum { HeadSize = 100 };
  if (!QFileInfo(fileName).suffix().compare("json", Qt::CaseInsensitive))
    return Acfun;

  QFile f(fileName);
  if (!f.open(QIODevice::ReadOnly))
    return UnknownSite;

  QString head(f.read(HeadSize * 2)); // wchar_t for UTF-8 file
  DOUT("head =" << head);
  if (head.isEmpty())
    return UnknownSite;
  if (head.contains("<packet>"))
    return Nicovideo;
  if (head.contains("<chatserver>"))
    return Bilibili;
  if (head.contains("<c>"))
    return Acfun;
  return UnknownSite;
}

// EOF
