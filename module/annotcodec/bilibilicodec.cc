// bilibilicodec.cc
// 2/3/2012
// See:  http://9ch.co/t17836,1-1.html

#include "bilibilicodec.h"
#include "module/compress/qgzip.h"
#include "module/annotcloud/cmd.h"
#include "module/annotcloud/traits.h"
#include <QtCore>
#include <QtNetwork>
#include <QtXml>

//#define DEBUG "BilibiliCodec"
#include "module/debug/debug.h"

using namespace AnnotCloud;

// - Construction -

BilibiliCodec::BilibiliCodec(QObject *parent)
  : Base(parent)
{
  qnam_ = new QNetworkAccessManager(this);
  connect(qnam_, SIGNAL(finished(QNetworkReply*)), SLOT(parseReply(QNetworkReply*)));
}

// - Fetch -

bool
BilibiliCodec::match(const QString &url) const
{ return url.contains("http://comment.bilibili.tv/", Qt::CaseInsensitive); }

void
BilibiliCodec::fetch(const QString &url)
{
  Q_ASSERT(match(url));
  DOUT("enter: url =" << url);
  qnam_->get(QNetworkRequest(url));
  DOUT("exit");
}

// - Decode -

void
BilibiliCodec::parseReply(QNetworkReply *reply)
{
  DOUT("enter");
  Q_ASSERT(reply);
  reply->deleteLater();
  if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
    //emit errorReceived(reply->errorString());
    emit errorReceived(tr("network error, failed to resolve media URL") + ": " + reply->url().toString());
    DOUT("exit: network error");
    return;
  }
  QByteArray data = reply->readAll();
  AnnotationList l;
  if (!data.isEmpty()) {
    QByteArray unzipped = ::gHttpUncompress(data);
    if (!unzipped.isEmpty())
      l = parseDocument(unzipped);
    else
      l = parseDocument(data);
  }
  if (l.isEmpty())
    emit errorReceived(tr("failed to resolve annotations from URL") + ": " + reply->url().toString());
  else
    emit fetched(l, reply->url().toString());
  DOUT("exit: annots.size =" << l.size());
}

// - Decode -

AnnotationList
BilibiliCodec::parseDocument(const QByteArray &data)
{
  DOUT("enter: data.size =" << data.size());
  if (data.isEmpty()) {
    DOUT("exit: empty data");
    return AnnotationList();
  }
  QDomDocument doc;
  doc.setContent(data);
  if (doc.isNull()) {
    DOUT("exit: invalid document root");
    return AnnotationList();
  }
  QDomElement root = doc.firstChildElement("i");
  if (root.isNull()) {
    DOUT("exit: invalid root element");
    return AnnotationList();
  }

  AnnotationList ret;
  QDomElement e = root.firstChildElement("d");
  while (!e.isNull()) {
    e = e.nextSiblingElement("d");
    QString attr = e.attribute("p");
    QString text = e.text();
    Annotation a = parseComment(attr, text);
    if (a.hasText())
      ret.append(a);
    e = e.nextSiblingElement("d");
  }
  DOUT("exit: size =" << ret.size());
  return ret;
}

QString
BilibiliCodec::parseText(const QString &text)
{
  if (text.isEmpty())
    return QString();
  QString ret = text;
  ret.replace("/n", "\n ");
  return ret.trimmed();
}

// Example: <d p="0,1,25,16777215,1312863760,0,eff85771,42759017">Annotation</d>
Annotation
BilibiliCodec::parseAttribute(const QString &attr)
{
  enum { AttrPos = 0, AttrFloatStyle, AttrFontSize, AttrFontColor, AttrCreateTime, AttrSubType, AttrUserId, AttrRowId, AttrCount };

  if (attr.isEmpty())
    return Annotation();
  QStringList l = attr.split(',');
  if (l.size() != AttrCount)
    return Annotation();

  bool ok;
  double pos = l[AttrPos].toDouble();
  int floatStyle = l[AttrFloatStyle].toInt();
  int fontSize = l[AttrFontSize].toInt();
  qint64 fontColor = l[AttrFontColor].toLongLong();
  qint64 createTime = l[AttrCreateTime].toLongLong();
  int subType = l[AttrSubType].toInt();
  qint64 userId = l[AttrUserId].toLongLong(&ok, 16);
  qint64 rowId = l[AttrRowId].toLongLong();

  Annotation ret;
  QString t;
  // 0
  ret.setPos(static_cast<qint64>(pos * 1000));

  // 1
  switch (floatStyle) {
  case 5: case 6: case 7:
    t += CORE_CMD_VIEW_TOP;
    break;
  case 4:
    t += CORE_CMD_VIEW_BOTTOM;
    break;
  }

  // 2
  //fontSize
  if (fontSize > 0)
    t += CORE_CMD_SIZE  "[" + QString::number(fontSize) + "]";

  // 3
  //fontColor
  if (fontColor > 0)
    t += CORE_CMD_COLOR "[" "#" + QString::number(fontColor, 16) + "]";

  // 4
  ret.setCreateTime(createTime);

  // 5
  if (subType == 1)
    t += CORE_CMD_SUB;

  // 6
  qint64 uid = userId;
  if (uid <= 0)
    uid = rowId;
  ret.setUserId(-uid);

  // 7
  ret.setId(-rowId);

  // Others
  if (t.isEmpty())
    t = CORE_CMD_VERBATIM;
  ret.setText(t);

  ret.setLanguage(Traits::Chinese);

  return ret;
}

// EOF
