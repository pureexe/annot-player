// ocntranslator.cc
// 8/20/2012
// See: http://yassblog.blog.so-net.ne.jp/2010-06-24
// See: http://www.emacswiki.org/emacs/trans-ej.el

#include "lib/translator/ocntranslator.h"
#include "lib/translator/ocntranslator_p.h"
#include <QtCore/QString>
//#include <QtCore/QTimer>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <ctime>

//#define DEBUG "ocntranslator"
#include "lib/debug/debug.h"

// - Properties -

QString
OcnTranslator::name() const
{ return tr("OCN Honyaku"); }

// - Authentification -

QByteArray
OcnTranslator::currentAuthKey()
{
  qint64 utc = ::time(nullptr);
  QByteArray hex = QByteArray::number(utc, 16);
  return OCN_AUTH_PREFIX + hex + OCN_AUTH_SUFFIX;
}

// - Translate -

const char*
OcnTranslator::lcode(int lang)
{
  switch (lang) {
  case English: return "en";
  case Korean: return "ko";
  case TraditionalChinese: return "tw";
  case SimplifiedChinese: return "zh";
  default: return 0;
  }
}

QByteArray
OcnTranslator::postData(const QString &text, const char *to, const char *from, const QByteArray &key)
{
  QByteArray ret = QUrl::toPercentEncoding(text);
  return ret.prepend("&" OCN_QUERY_TEXT "=")
     .prepend(key).prepend("&" OCN_QUERY_AUTH "=")
     .prepend(to).prepend(from).prepend(OCN_QUERY_LANG "=");
}

QNetworkReply*
OcnTranslator::createReply(const QString &text, int to, int from)
{
  QNetworkRequest req(QUrl(OCN_API));
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  const char *t = lcode(to),
             *f = lcode(from);
  QByteArray data = postData(text, t ? t : "en", f ? f : "ja", currentAuthKey());
  return networkAccessManager()->post(req, data);
}

QString
OcnTranslator::parseReply(const QByteArray &data)
{
  QByteArray ret;
  if (!data.isEmpty()) {
    int begin = data.indexOf(OCN_AREA_BEGIN);
    if (begin >= 0) {
      begin += sizeof(OCN_AREA_BEGIN) -1;
      begin = data.indexOf(OCN_AREA_BEGIN2, begin);
      if (begin >= 0) {
        begin += sizeof(OCN_AREA_BEGIN2) -1;
        int end = data.indexOf(OCN_AREA_END, begin);
        if (end > 0) {
          ret = data.mid(begin, end - begin);
          if (ret == OCN_FORBIDDEN_TEXT)
            ret.clear();
        }
      }
    }
  }
  return ret;
}

// EOF
