// annotationcodecmanager.cc
// 2/4/2012
#include "module/annotcodec/annotationcodecmanager.h"
#include "module/annotcodec/acfuncodec.h"
#include "module/annotcodec/bilibilicodec.h"
#include "module/annotcodec/nicovideocodec.h"
#include <QtCore>

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
    connect(c, SIGNAL(fetched(AnnotationList,QString,QString)), SIGNAL(fetched(AnnotationList,QString,QString))); \
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
AnnotationCodecManager::fetch(int id, const QString &url, const QString &originalUrl)
{
  if (id < 0 || id > codecs_.size()) {
    Q_ASSERT(0);
    DOUT("illegal state: codec id out of bounds");
    return;
  }
  codecs_[id]->fetch(url, originalUrl);
}

// - Parse -

AnnotationList
AnnotationCodecManager::parseFile(const QString &fileName)
{
  Format site = fileFormat(fileName);
  DOUT("site =" << site << ", fileName =" << fileName);
  switch (site) {
  case Nicovideo: return NicovideoCodec::parseFile(fileName);
  case Acfun:     return AcfunCodec::parseFile(fileName);
  case Bilibili:  return BilibiliCodec::parseFile(fileName);
  case UnknownFormat:
  default:        return AnnotationList();
  }
}

AnnotationCodecManager::Format
AnnotationCodecManager::fileFormat(const QString &fileName)
{
  enum { HeaderSize = 150 };
  if (!QFileInfo(fileName).suffix().compare("json", Qt::CaseInsensitive))
    return Acfun;

  QFile f(fileName);
  if (!f.open(QIODevice::ReadOnly))
    return UnknownFormat;

  QString head(f.read(HeaderSize * 2)); // wchar_t for UTF-8 file
  DOUT("head =" << head);
  if (head.isEmpty())
    return UnknownFormat;
  if (head.startsWith("<!-- annotation")) {
    if (head.contains("www.nicovideo.jp"))
      return Nicovideo;
    if (head.contains("www.bilibili.tv"))
      return Bilibili;
    if (head.contains("www.acfun.tv"))
      return Acfun;
  } else {
    if (head.contains("<packet>"))
      return Nicovideo;
    if (head.contains("<chatserver>"))
      return Bilibili;
    if (head.contains("<c>"))
      return Acfun;
  }
  return UnknownFormat;
}

// - Annotation -

bool
AnnotationCodecManager::isAnnotatedFile(const QString &fileName)
{
  // <!-- annotation    -- len: 15
  // /** annotation     -- len: 14
  enum { HeaderSize = 15 };
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly))
    return false;
  QByteArray data = file.read(HeaderSize);
  return
    data.startsWith("<!-- annotation") ||
    data.startsWith("/** annotation");
}

QString
AnnotationCodecManager::parseAnnotatedHeader(const QString &fileName)
{
  enum { HeaderSize = 200 };
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly))
    return QString();
  QByteArray data = file.read(HeaderSize);
  int pos = data.startsWith("<!-- annotation") ? data.indexOf("-->") :
            data.startsWith("/** annotation") ? data.indexOf("*/") :
            -1;
  return pos < 0 ? QString() :
         QString::fromLocal8Bit(data.left(pos));
}

QString
AnnotationCodecManager::parseAnnotatedUrl(const QString &fileName)
{
  QString header = parseAnnotatedHeader(fileName);
  if (header.isEmpty())
    return QString();
  QStringList l = header.split(QRegExp("\\s"), QString::SkipEmptyParts);
  if (l.size() < 2 || l[1] != "annotation")
    return QString();
  bool isUrl = false;
  foreach (const QString &key, l)
    if (isUrl)
      return key;
    else if (key == "\\url" || key == "@url")
      isUrl = true;
  return QString();
}

// EOF
