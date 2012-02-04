// acfuncodec.cc
// 2/4/2012

#include "acfuncodec.h"
#include "module/annotcloud/cmd.h"
#include "module/annotcloud/traits.h"
#include <QtCore>
#include <QtNetwork>
#include <QtXml>
#include <QtScript>
#include <climits>

//#define DEBUG "AcFunCodec"
#include "module/debug/debug.h"

using namespace AnnotCloud;

// - Construction -

AcFunCodec::AcFunCodec(QObject *parent)
  : Base(parent)
{
  qnam_ = new QNetworkAccessManager(this);
  connect(qnam_, SIGNAL(finished(QNetworkReply*)), SLOT(parseReply(QNetworkReply*)));
}

// - Fetch -

bool
AcFunCodec::match(const QString &url) const
{
  DOUT("enter: url =" << url);
  bool ret = url.contains("http://comment.acfun.tv/", Qt::CaseInsensitive);
  DOUT("exit: ret =" << ret);
  return ret;
}

void
AcFunCodec::fetch(const QString &url)
{
  Q_ASSERT(match(url));
  DOUT("enter: url =" << url);
  qnam_->get(QNetworkRequest(url));
  DOUT("exit");
}

// - Decode -

void
AcFunCodec::parseReply(QNetworkReply *reply)
{
  DOUT("enter");
  Q_ASSERT(reply);
  reply->deleteLater();
  if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
    //emit errorReceived(reply->errorString());
    emit errorReceived(tr("network error, failed to resolve media URL") + " <br/> " + reply->url().toString());
    DOUT("exit: network error");
    return;
  }
  AnnotationList l = parseDocument(reply->readAll());
  if (l.isEmpty())
    emit errorReceived(tr("failed to resolve annotations from URL") + ": " + reply->url().toString());
  else
    emit fetched(l, reply->url().toString());
  DOUT("exit: annots.size =" << l.size());
}

// - Decode -

AnnotationList
AcFunCodec::parseXmlDocument(const QByteArray &data)
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
  QDomElement root = doc.firstChildElement("c");
  if (root.isNull()) {
    DOUT("exit: invalid root element");
    return AnnotationList();
  }

  AnnotationList ret;
  QDomElement e = root.firstChildElement("l");
  while (!e.isNull()) {
    e = e.nextSiblingElement("l");
    QString attr = e.attribute("i");
    QString text = e.text();
    Annotation a = parseComment(attr, text);
    if (a.hasText()) {
      qint64 id = ret.size() + (qint64)INT_MAX;
      a.setId(-id);
      ret.append(a);
    }
    e = e.nextSiblingElement("l");
  }
  DOUT("exit: size =" << ret.size());
  return ret;
}

// Example: http://comment.acfun.tv/70590459.json?clientID=0.9264421034604311
AnnotationList
AcFunCodec::parseJsonDocument(const QByteArray &data)
{
  DOUT("enter: data.size =" << data.size());
  if (data.isEmpty()) {
    DOUT("exit: empty data");
    return AnnotationList();
  }

  // json parser: http://qtwiki.org/Parsing_JSON_with_QT_using_standard_QT_library
  QScriptEngine engine;
  QString json(data);
  QScriptValue root = engine.evaluate("(" + json + ")");
  if (!root.isValid()) {
    DOUT("exit: invalid JSON document");
    return AnnotationList();
  }

  QScriptValueIterator it(root);
  it.next();
  if (!it.hasNext()) {
    DOUT("exit: empty JSON list");
    return AnnotationList();
  }

  AnnotationList ret;
  while (it.hasNext()) {
    QScriptValue v = it.value();

    QScriptValue c = v.property("c");
    QScriptValue m = v.property("m");

    QString attr = c.toString();
    QString text = m.toString();
    if (attr.isEmpty() || text.isEmpty())
      continue;
    Annotation a = parseComment(attr, text);
    if (a.hasText()) {
      qint64 id = ret.size() + (qint64)INT_MAX;
      a.setId(-id);
      ret.append(a);
    }
    it.next();
  }
  DOUT("exit: size =" << ret.size());
  return ret;
}

QString
AcFunCodec::parseText(const QString &text)
{
  DOUT("text =" << text);
  if (text.isEmpty())
    return QString();
  QString ret = text;
  //ret.replace("/n", "\n ");
  return ret.trimmed();
}

// Example: <l i="104.5,25,16776960,1,smdhk79eda608,1328249735"><![CDATA[......]]></l>
Annotation
AcFunCodec::parseAttribute(const QString &attr)
{
  DOUT("attr =" << attr);
  enum { AttrPos = 0, AttrFontColor, AttrFloatStyle, AttrFontSize, AttrUserId, AttrCreateTime, AttrCount };

  if (attr.isEmpty())
    return Annotation();
  QStringList l = attr.split(',');
  if (l.size() != AttrCount)
    return Annotation();

  bool ok;
  double pos = l[AttrPos].toDouble();
  int fontSize = l[AttrFontSize].toInt();
  qint64 fontColor = l[AttrFontColor].toLongLong();
  int floatStyle = l[AttrFloatStyle].toInt();
  qint64 userId = l[AttrUserId].toLongLong(&ok, 16);
  qint64 createTime = l[AttrCreateTime].toLongLong();

  Annotation ret;
  QString t;
  // 1
  ret.setPos(static_cast<qint64>(pos * 1000));

  // 2
  //fontSize
  if (fontSize > 0)
    t += CORE_CMD_SIZE  "[" + QString::number(fontSize) + "]";

  // 3
  //fontColor
  if (fontColor > 0)
    t += CORE_CMD_COLOR "[" "#" + QString::number(fontColor, 16) + "]";

  // 4
  switch (floatStyle) {
  case 5: case 6: case 7:
    t += CORE_CMD_VIEW_TOP;
    break;
  case 4:
    t += CORE_CMD_VIEW_BOTTOM;
    break;
  }

  // 5
  ret.setUserId(-userId);

  // 6
  ret.setCreateTime(createTime);

  // Others
  if (t.isEmpty())
    t = CORE_CMD_VERBATIM;
  ret.setText(t);

  ret.setLanguage(Traits::Chinese);

  return ret;
}

// EOF
