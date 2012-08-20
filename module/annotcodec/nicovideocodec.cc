// nicovideocodec.cc
// 2/6/2012
// See: http://dic.nicovideo.jp/a/ニコニコ動画api
// See: http://hai3.net/blog/?p=122
// See:  http://blog.smartnetwork.co.jp/staff/node/22

#include "module/annotcodec/nicovideocodec.h"
#ifdef WITH_MODULE_ANNOTCACHE
# include "module/annotcache/annotationcachemanager.h"
#endif // WITH_MODULE_ANNOTCACHE
#include "module/annotcloud/annottag.h"
#include "module/annotcloud/traits.h"
#include "module/qtext/htmltag.h"
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <climits>

//#define DEBUG "nicovideocodec"
#include "module/debug/debug.h"

using namespace AnnotCloud;

enum NicoHash { // qHash(QString($mail))
  NH_184 = 13492,        // "184"
  NH_Docomo = 112567871, // "docomo"
  NH_Medium = 121352381, // "medium"
  NH_Naka = 477201,      // "naka"
  NH_Shita = 7991457,    // "shita"
  NH_Ue = 1973           // "ue"
};

// - Fetch -

bool
NicovideoCodec::match(const QString &url) const
{ return url.startsWith("file:///", Qt::CaseInsensitive); }

void
NicovideoCodec::fetch(const QString &url, const QString &originalUrl)
{
  DOUT("enter: url =" << url);
  QString path = QUrl(url).toLocalFile();
  if (!path.isEmpty())
    fetchLocalFile(path, originalUrl);
  DOUT("exit");
}

void
NicovideoCodec::fetchLocalFile(const QString &path, const QString &originalUrl)
{
  DOUT("enter: path =" << path);
  QFile file(path);
  if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
    emit errorMessage(tr("network error, failed to resolve nicovideo comments"));
    return;
  }
  QByteArray data = file.readAll();
  //DOUT("data =" << QString(data));
  AnnotationList l = parseDocument(data);
  if (l.isEmpty())
    emit errorMessage(tr("failed to resolve nicovideo comments"));
  else {
#ifdef WITH_MODULE_ANNOTCACHE
    AnnotationCacheManager::globalInstance()->saveData(data, originalUrl);
#endif // WITH_MODULE_ANNOTCACHE
    emit fetched(l, path, originalUrl);
  }
  DOUT("exit: annots.size =" << l.size());
}

// - Decode -

// See: http://blog.smartnetwork.co.jp/staff/node/22
// Example:  <chat thread="1319870055" no="54344" vpos="13103" date="1328534388" mail="shita medium yellow 184" user_id="wiQ-lBgI1JIAAU7NjAMgTk837Eg" anonymity="1">オオオオオ</chat>
// attr = thread, no, vpos, date, mail, user_id, anonymity
AnnotationList
NicovideoCodec::parseDocument(const QByteArray &data)
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
  QDomElement root = doc.firstChildElement("packet");
  if (root.isNull()) {
    DOUT("exit: invalid root element");
    return ret;
  }

  QDomElement e = root.firstChildElement("chat");
  while (!e.isNull()) {
    QString text = e.text().trimmed();
    if (!text.isEmpty()) {
      Annotation a;

      qint64 id = e.attribute("no").toLongLong();
      long vpos = e.attribute("vpos").toLong();
      qint64 date = e.attribute("date").toLongLong();
      QString mail = e.attribute("mail");
      QString userAlias = e.attribute("user_id");
      QString anonymity = e.attribute("anonymity");

      a.setId(-id);
      a.setCreateTime(date);

      if (anonymity == "1")
        a.setUserAnonymous(true);
      a.setUserAlias(userAlias);
      qint64 uid = qHash(userAlias);
      a.setUserId(uid + LLONG_MIN);

      qint64 pos = vpos * 10; // 1/100 second
      a.setPos(pos);

      QString prefix = parsePrefix(mail);
      if (!prefix.isEmpty())
        text = prefix + " " + text;
      a.setText(text);

      a.setLanguage(Traits::Japanese);

      ret.append(a);
    }
    e = e.nextSiblingElement("chat");
  }
  DOUT("exit: size =" << ret.size());
  return ret;
}

QString
NicovideoCodec::parseText(const QString &text)
{ return text.isEmpty() ? text : ::html_escape(text.trimmed()); }

// See: http://nicowiki.com/elsecom.html
QString
NicovideoCodec::parsePrefix(const QString &text)
{
  if (text.isEmpty() || text == "184")
    return QString();

  QString ret;
  foreach (const QString &attr, text.split(' ',  QString::SkipEmptyParts))
    switch (qHash(attr)) {
    case NH_184:
    case NH_Docomo:
    case NH_Naka:
    case NH_Medium:
      continue;
    case NH_Ue: ret.append(CORE_CMD_VIEW_TOP); break;
    case NH_Shita: ret.append(CORE_CMD_VIEW_BOTTOM); break;
    default:
      if (!attr.isEmpty())
        ret.append(CORE_CMDSTR).append(attr);
    }
  return ret;
}

// EOF
