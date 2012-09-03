// ocntranslator.cc
// 8/20/2012
// See: http://yassblog.blog.so-net.ne.jp/2010-06-24
// See: http://www.emacswiki.org/emacs/trans-ej.el

#include "module/translator/ocntranslator.h"
#include "module/translator/ocntranslator_p.h"
#include <QtCore/QString>
//#include <QtCore/QTimer>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <ctime>

//#define DEBUG "ocntranslator"
#include "module/debug/debug.h"

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

QByteArray
OcnTranslator::postData(const QString &text, const QString &to, const QString &from, const QByteArray &key)
{
  const char *lang_to =
    to == "en" ? "en" :
    to == "ko" ? "ko" :
    to == "zh-CHS" ? "zh" :
    to == "zh-CHT" ? "tw" :
    "en";

  const char *lang_from =
    from.isEmpty() ? "ja" :
    from == "en" ? "en" :
    from == "ko" ? "ko" :
    from == "zh-CHS" ? "zh" :
    from == "zh-CHT" ? "tw" :
    "ja";

  QByteArray ret = QUrl::toPercentEncoding(text);
  return ret.prepend("&" OCN_KEY_TEXT "=")
     .prepend(key).prepend("&" OCN_KEY_AUTH "=")
     .prepend(lang_to).prepend(lang_from).prepend(OCN_KEY_LANG "=");
}

QNetworkReply*
OcnTranslator::createReply(const QString &text, const QString &to, const QString &from)
{
  QNetworkRequest req(QUrl(OCN_API));
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  QByteArray data = postData(text, to, from, currentAuthKey());
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
