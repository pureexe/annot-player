// media.cc
// 8/4/2012

#include "media.h"
#include "icons.h"
#include <QtCore/QFile>
#include <QtCore/QHash>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QXmlStreamWriter>

//#define DEBUG "media"
#include "lib/debug/debug.h"

namespace { // anonymous
  struct init_ { init_() {
    qRegisterMetaType<Media>("Media");
  } };
  init_ static_init_;
} // anonymois namespace

#define FMT_VERSION "0.1"
#define TAG_ROOT    "library"
#define TAG_VALUE   "media"

#define TAG_TOKENID     "tokenId"
#define TAG_TYPE        "type"
#define TAG_DIGEST      "digest"
#define TAG_LOCATION    "location"
#define TAG_EXISTS      "exists"
#define TAG_TITLE       "title"
#define TAG_VISITTIME   "visitTime"
#define TAG_VISITCOUNT  "visitCount"

enum Hash {
  H_TokenId = 190956052,
  H_Type = 508005,
  H_Digest = 112188580,
  H_Location = 93889086,
  H_Exists = 114231987,
  H_Title = 8063781,
  H_VisitCount = 9030932,
  H_VisitTime = 168384053
};

// - Properties -

QIcon
Media::icon() const
{
  switch (type_) {
  case Game:
    return hasLocation() ? Icons::iconForFile(location()) : Icons::gameIcon();
  case Url:
    return hasLocation() ? Icons::iconForUrl(location()) : Icons::networkIcon();
  case Video:
    return Icons::videoIcon();
  case Audio:
    return Icons::audioIcon();
  case Picture:
    return Icons::imageIcon();
  default:
    return Icons::fileIcon();
  }
}

//bool
//Media::isRemote() const
//{
//  if (remote_ < 0)
//    remote_ = location_.startsWith("http://", Qt::CaseInsensitive) ? 1 : 0;
//  return remote_;
//}

// - Read-

QList<Media>
Media::readList(const QString &fileName)
{
  DOUT("enter: fileName =" << fileName);
  QList<Media> ret;

  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text))
    return ret;

  QDomDocument doc;
  doc.setContent(file.readAll());
  if (doc.isNull()) {
    DOUT("exit: invalid document root");
    return ret;
  }
  QDomElement root = doc.firstChildElement(TAG_ROOT);
  if (root.isNull()) {
    DOUT("exit: invalid root element");
    return ret;
  }

  QDomElement e = root.firstChildElement(TAG_VALUE);
  while (!e.isNull()) {
    QDomElement c = e.firstChildElement();
    Self v;
    while (!c.isNull()) {
      switch (qHash(c.tagName())) {
      case H_TokenId:    v.setTokenId(c.text().toLongLong()); break;
      case H_Digest:     v.setDigest(c.text()); break;
      case H_Location:   v.setLocation(c.text()); break;
      case H_Exists:     v.setExists(c.text() == "true"); break;
      case H_Title:      v.setTitle(c.text()); break;
      case H_Type:       v.setType(c.text().toInt()); break;
      case H_VisitTime:  v.setVisitTime(c.text().toLongLong()); break;
      case H_VisitCount: v.setVisitCount(c.text().toInt()); break;
      default: DOUT("warning: unknown tag:" << c.tagName());
      }
      c = c.nextSiblingElement();
    }
    ret.append(v);
    e = e.nextSiblingElement(TAG_VALUE);
  }
  DOUT("exit: size =" << ret.size());
  return ret;
}

QHash<QString, Media>
Media::readHash(const QString &fileName)
{
  DOUT("enter: fileName =" << fileName);
  QHash<QString, Media> ret;

  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text))
    return ret;

  QDomDocument doc;
  doc.setContent(file.readAll());
  if (doc.isNull()) {
    DOUT("exit: invalid document root");
    return ret;
  }
  QDomElement root = doc.firstChildElement(TAG_ROOT);
  if (root.isNull()) {
    DOUT("exit: invalid root element");
    return ret;
  }

  QDomElement e = root.firstChildElement(TAG_VALUE);
  while (!e.isNull()) {
    QDomElement c = e.firstChildElement();
    Self v;
    while (!c.isNull()) {
      switch (qHash(c.tagName())) {
      case H_TokenId:    v.setTokenId(c.text().toLongLong()); break;
      case H_Digest:     v.setDigest(c.text()); break;
      case H_Location:   v.setLocation(c.text()); break;
      case H_Exists:     v.setExists(c.text() == "true"); break;
      case H_Title:      v.setTitle(c.text()); break;
      case H_Type:       v.setType(c.text().toInt()); break;
      case H_VisitTime:  v.setVisitTime(c.text().toLongLong()); break;
      case H_VisitCount: v.setVisitCount(c.text().toInt()); break;
      default: DOUT("warning: unknown tag:" << c.tagName());
      }
      c = c.nextSiblingElement();
    }
    QString k = v.key();
    if (!k.isEmpty())
      ret[k] = v;
    e = e.nextSiblingElement(TAG_VALUE);
  }
  DOUT("exit: size =" << ret.size());
  return ret;
}


// - Write -

template <typename L>
bool
Media::write(const L &l, const QString &fileName)
{
  DOUT("enter: count =" << l.size() << ", fileName =" << fileName);
  //if (l.isEmpty()) {
  //  if (QFile::exists(fileName))
  //    QFile::remove(fileName);
  //  DOUT("exit: empty list");
  //  return;
  //}
  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly)) {
    DOUT("exit: ret = false, cannot  write to file");
    return false;
  }

  QXmlStreamWriter writer(&file);
  writer.setAutoFormatting(true);

  writer.writeStartDocument();
  writer.writeStartElement(TAG_ROOT);
  writer.writeAttribute("version", FMT_VERSION);

  foreach (const Self &v, l) {
    writer.writeStartElement(TAG_VALUE);

    writer.writeTextElement(TAG_TOKENID, QString::number(v.tokenId()));
    writer.writeTextElement(TAG_DIGEST, v.digest());
    writer.writeTextElement(TAG_LOCATION, v.location());
    writer.writeTextElement(TAG_EXISTS, v.exists() ? "true" : "false");
    writer.writeTextElement(TAG_TITLE, v.title());
    writer.writeTextElement(TAG_TYPE, QString::number(v.type()));
    writer.writeTextElement(TAG_VISITTIME, QString::number(v.visitTime()));
    writer.writeTextElement(TAG_VISITCOUNT, QString::number(v.visitCount()));

    writer.writeEndElement();
  }
  writer.writeEndElement();
  writer.writeEndDocument();

  file.close();

  bool ret = !writer.hasError() && file.error() == QFile::NoError;
  DOUT("exit: ret =" << ret);
  return ret;
}

// - Instantiate Templates -

template
bool
Media::write(const QList<Media> &l, const QString &fileName);

template
bool
Media::write(const QHash<QString, Media> &l, const QString &fileName);

// EOF
