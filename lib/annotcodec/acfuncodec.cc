// acfuncodec.cc
// 2/4/2012

#include "lib/annotcodec/acfuncodec.h"
#include "lib/annotcloud/annottag.h"
#include "lib/annotcloud/traits.h"
#ifdef WITH_LIB_ANNOTCACHE
# include "lib/annotcache/annotationcachemanager.h"
#endif // WITH_LIB_ANNOTCACHE
#include "lib/qtext/htmltag.h"
#include "lib/qtext/network.h"
#include <QtCore/QStringList>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <climits>

//#define DEBUG "acfuncodec"
#include "lib/debug/debug.h"

#define RequestUrlAttribute QNetworkRequest::UserMax

enum { MaxRetries = 5 };

using namespace AnnotCloud;

// - Construction -

AcfunCodec::AcfunCodec(QObject *parent)
  : Base(parent)
{
  nam_ = new QNetworkAccessManager(this);
  connect(nam_, SIGNAL(finished(QNetworkReply*)), SLOT(parseReply(QNetworkReply*)));
}

// - Fetch -

bool
AcfunCodec::match(const QString &url) const
{
  DOUT("enter: url =" << url);
  bool ret = url.contains("http://comment.acfun.tv/", Qt::CaseInsensitive);
  DOUT("exit: ret =" << ret);
  return ret;
}

void
AcfunCodec::fetch(const QString &url, const QString &originalUrl)
{
  Q_ASSERT(match(url));
  DOUT("enter: url =" << url);
  QNetworkReply *reply = nam_->get(QNetworkRequest(url));
  QtExt::PublicNetworkReply::fromReply(reply)
      ->setAttribute(RequestUrlAttribute, originalUrl);
  DOUT("exit");
}

// - Decode -

void
AcfunCodec::parseReply(QNetworkReply *reply)
{
  DOUT("enter");
  Q_ASSERT(reply);
  //QTimer::singleShot(0, reply, SLOT(deleteLater()));
  reply->deleteLater();
  QString url = reply->url().toString();
  if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
    int retry = retries_[url];
    if (retry <= MaxRetries) {
      emit errorMessage(
        tr("network error, retry")
        + QString(" (%1/%2):").arg(QString::number(retry)).arg(QString::number(MaxRetries))
        + url
      );

      QString originalUrl = reply->attribute(RequestUrlAttribute).toString();
      fetch(url, originalUrl);
    } else
      emit errorMessage(tr("network error, failed to resolve media URL") + ": " + url);
    DOUT("exit: network error:" << reply->errorString());
    return;
  }
  retries_.remove(url);
  QByteArray data = reply->readAll();
  AnnotationList l = parseDocument(data, Json);
  if (l.isEmpty())
    emit errorMessage(tr("failed to resolve annotations from URL") + ": " + reply->url().toString());
  else {
    QString url =reply->url().toString(),
            originalUrl = reply->attribute(RequestUrlAttribute).toString();
#ifdef WITH_LIB_ANNOTCACHE
    AnnotationCacheManager::globalInstance()->saveData(data, originalUrl);
#endif // WITH_LIB_ANNOTCACHE
    emit fetched(l, url, originalUrl);
  }
  DOUT("exit: annots.size =" << l.size());
}

// - Decode -

AnnotationList
AcfunCodec::parseXmlDocument(const QByteArray &data)
{
  DOUT("enter: data.size =" << data.size());
  AnnotationList ret;
  if (data.isEmpty()) {
    DOUT("exit: empty data");
    return ret;
  }
  QDomDocument doc;
  doc.setContent(skipXmlLeadingComment(
    QChar(data[0]).isSpace() ? data.trimmed() : data
  ));
  if (doc.isNull()) {
    DOUT("exit: invalid document root");
    return ret;
  }
  QDomElement root = doc.firstChildElement("c");
  if (root.isNull()) {
    DOUT("exit: invalid root element");
    return ret;
  }

  QDomElement e = root.firstChildElement("l");
  while (!e.isNull()) {
    QString text = e.text().trimmed();
    QString attr = e.attribute("i");
    if (!text.isEmpty() && !attr.isEmpty()) {
      Annotation a = parseComment(attr, text);
      if (a.hasText()) {
        qint64 id = ret.size() + qint64(INT_MAX);
        a.setId(-id);
        ret.append(a);
      }
    }
    e = e.nextSiblingElement("l");
  }
  DOUT("exit: size =" << ret.size());
  return ret;
}

// Example: http://comment.acfun.tv/70590459.json?clientID=0.9264421034604311
AnnotationList
AcfunCodec::parseJsonDocument(const QByteArray &data)
{
  DOUT("enter: data.size =" << data.size());
  AnnotationList ret;
  if (data.isEmpty()) {
    DOUT("exit: empty data");
    return ret;
  }

  // Comments and spaces will be skipped by QScriptEngine.
  //data = skipJsonLeadingComment(QChar(data[0]).isSpace() ? data.trimmed() : data);

  // json parser: http://qtwiki.org/Parsing_JSON_with_QT_using_standard_QT_library
  QScriptEngine engine;
  QString json(data);

  QScriptValue root = engine.evaluate("(" + json + ")");
  if (!root.isValid()) {
    DOUT("exit: invalid JSON document");
    return ret;
  }

  QScriptValueIterator it(root);
  it.next();
  while (it.hasNext()) {
    QScriptValue v = it.value();

    QScriptValue c = v.property("c");
    QScriptValue m = v.property("m");

    QString text = m.toString().trimmed();
    QString attr = c.toString();
    if (!text.isEmpty() && !attr.isEmpty()) {
      Annotation a = parseComment(attr, text);
      if (a.hasText()) {
        qint64 id = ret.size() + qint64(INT_MAX);
        a.setId(-id);
        ret.append(a);
      }
    }
    it.next();
  }
  DOUT("exit: size =" << ret.size());
  return ret;
}

QString
AcfunCodec::parseText(const QString &text)
{
  DOUT("text =" << text);
  if (text.isEmpty())
    return QString();
  QString ret = ::html_escape(text.trimmed());
  if (ret == "/fly")
    return CORE_CMD_VIEW_FLY " " + QString::fromLocal8Bit("飞");
  ret.replace("/fly ", CORE_CMD_VIEW_FLY " ", Qt::CaseInsensitive);
  if (ret.startsWith("飛") ||
#ifdef _MSC_VER
      ret.startsWith("\xe9\xa3\x9e")
#else
      ret.startsWith("飞")
#endif // _MSC_VER
      )
    ret.prepend(CORE_CMD_VIEW_FLY " ");
  return ret.trimmed();
}

// Example: <l i="104.5,25,16776960,1,smdhk79eda608,1328249735"><![CDATA[......]]></l>
Annotation
AcfunCodec::parseAttribute(const QString &attr)
{
  DOUT("attr =" << attr);
  enum { AttrPos = 0, AttrFontColor, AttrFloatStyle, AttrFontSize, AttrUserId, AttrCreateTime, AttrCount };

  if (attr.isEmpty())
    return Annotation();
  QStringList l = attr.split(',');
  if (l.size() != AttrCount)
    return Annotation();

  double pos = l[AttrPos].toDouble();
  int fontSize = l[AttrFontSize].toInt();
  int fontColor = l[AttrFontColor].toInt();
  int floatStyle = l[AttrFloatStyle].toInt();
  QString userAlias = l[AttrUserId];
  qint64 createTime = l[AttrCreateTime].toLongLong();

  Annotation ret;
  QString t;
  // 1
  ret.setPos(pos * 1000);

  // 2
  //fontSize
#ifdef Q_OS_WIN
  fontSize *= 0.9;
#endif // Q_OS_WIN
  if (fontSize > 0)
    t += CORE_CMD_SIZE  "[" + QString::number(fontSize) + "]";

  // 3
  //fontColor
  if (fontColor > 0)
    t += CORE_CMD_COLOR "[" "#" + QString().sprintf("%06X", fontColor) + "]";

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
  ret.setUserAlias(userAlias);
  qint64 uid = qHash(userAlias);
  ret.setUserId(uid + LLONG_MIN);

  // 6
  ret.setCreateTime(createTime);

  // Others
  //if (t.isEmpty())
  //  t = CORE_CMD_VERBATIM;
  ret.setText(t);

  ret.setLanguage(Traits::SimplifiedChinese);

  return ret;
}

// EOF
