// bilibilicodec.cc
// 2/3/2012
// See:  http://9ch.co/t17836,1-1.html

#include "bilibilicodec.h"
#include "module/annotcloud/annottag.h"
#include "module/annotcloud/traits.h"
#include "module/qtext/htmltag.h"
#ifdef WITH_MODULE_COMPRESS
#  include "module/compress/qgzip.h"
#else
#  error "gzip is required to uncompress bilibili reply"
#endif // WITH_MODULE_COMPRESS
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtCore/QStringList>

//#define DEBUG "bilibilicodec"
#include "module/debug/debug.h"

using namespace AnnotCloud;

enum { MaxRetries = 5 };

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
  QString url = reply->url().toString();
  if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
    int retry = ++retries_[url];
    if (retry <= MaxRetries) {
      emit error(
        tr("network error, retry")
        + QString(" (%1/%2):").arg(QString::number(retry)).arg(QString::number(MaxRetries))
        + url
      );
      fetch(url);
    } else
      emit error(tr("network error, failed to resolve media URL") + ": " + url);
    DOUT("exit: network error:" << reply->errorString());
    return;
  }
  retries_.remove(url);
  QByteArray data = reply->readAll();
  AnnotationList l;
  if (!data.isEmpty()) {
#ifdef WITH_MODULE_COMPRESS
    QByteArray unzipped = ::gHttpUncompress(data);
    if (!unzipped.isEmpty())
      l = parseDocument(unzipped);
    else
#endif // WITH_MODULE_COMPRESS
      l = parseDocument(data);
  }
  if (l.isEmpty())
    emit error(tr("failed to resolve annotations from URL") + ": " + reply->url().toString());
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
    QString text = e.text().trimmed();
    QString attr = e.attribute("p");
    if (!text.isEmpty() && !attr.isEmpty()) {
      Annotation a = parseComment(attr, text);
      if (a.hasText())
        ret.append(a);
    }
    e = e.nextSiblingElement("d");
  }
  DOUT("exit: size =" << ret.size());
  return ret;
 }

QString
BilibiliCodec::parseText(const QString &text)
{
  return text.isEmpty() ? text : ::html_escape(text)
    .replace("/n", "\n ")
    .trimmed();
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

  double pos = l[AttrPos].toDouble();
  int floatStyle = l[AttrFloatStyle].toInt();
  int fontSize = l[AttrFontSize].toInt();
  int fontColor = l[AttrFontColor].toInt();
  qint64 createTime = l[AttrCreateTime].toLongLong();
  int subType = l[AttrSubType].toInt();
  QString userAlias = l[AttrUserId];
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
#ifdef Q_WS_WIN
  fontSize *= 0.9;
#endif // Q_WS_WIN
  if (fontSize > 0)
    t += CORE_CMD_SIZE  "[" + QString::number(fontSize) + "]";

  // 3
  //fontColor
  if (fontColor > 0)
    t += CORE_CMD_COLOR "[" "#" + QString().sprintf("%06X", fontColor) + "]";

  // 4
  ret.setCreateTime(createTime);

  // 5
  if (subType == 1)
    t += CORE_CMD_SUB;

  // 6
  bool ok;
  ret.setUserAlias(userAlias);
  qint64 uid = userAlias.toLongLong(&ok, 16);
  if (uid <= 0)
    uid = rowId;
  ret.setUserId(-uid);

  // 7
  ret.setId(-rowId);

  // Others
  //if (t.isEmpty())
  //  t = CORE_CMD_VERBATIM;
  ret.setText(t);

  ret.setLanguage(Traits::Chinese);

  return ret;
}

// EOF
