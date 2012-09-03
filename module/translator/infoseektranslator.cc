// infoseektranslator.cc
// 8/21/2012
// See: http://transer.com/sdk/rest_api_function.html
// See: http://translation.infoseek.ne.jp/js/translation-text.js

#include "module/translator/infoseektranslator.h"
#include "module/translator/infoseektranslator_p.h"
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>

//#define DEBUG "infoseektranslator"
#include "module/debug/debug.h"

// - Properties -

QString
InfoseekTranslator::name() const
{ return tr("Infoseek Honyaku"); }

// - Translate -

// Note: according to userinfo.js:
// ja-zhs => JC
// zhs-ja => CJ
// ja-zht => JCT
// zht-ja => CJT, not CTJ!
const char*
InfoseekTranslator::translateLanguage(const QString &lcode)
{
  if (lcode.isEmpty())
    return 0;
  else if (lcode == "en")
    return "E";
  else if (lcode == "ja")
    return "J";
  else if (lcode == "zh-CHS")
    return "C";
  else if (lcode == "zh-CHT")
    return "CT";
  else if (lcode == "ko")
    return "K";
  else if (lcode == "fr")
    return "F";
  else if (lcode == "de")
    return "G";
  else if (lcode == "it")
    return "I";
  else if (lcode == "es")
    return "S";
  else if (lcode == "pt")
    return "P";
  else
    return 0;
}

QString
InfoseekTranslator::translateUrl(const QString &text, const char *to, const char *from)
{
  return INFOSEEK_API
     "&" INFOSEEK_KEY_AUTH "=" INFOSEEK_AUTH_USER "+" INFOSEEK_AUTH_KEY
     "&" INFOSEEK_KEY_LANG "=" + QString(from).append(to) +
     "&" INFOSEEK_KEY_TEXT "=" + text;
}

QNetworkReply*
InfoseekTranslator::createReply(const QString &text, const QString &to, const QString &from)
{
  const char *f = translateLanguage(from),
             *t = translateLanguage(to);
  QString url = translateUrl(text, t ? t : "E", f ? f : "J");
  return networkAccessManager()->get(QNetworkRequest(url));
}

QString
InfoseekTranslator::parseReply(const QByteArray &data)
{
  QByteArray ret;
  if (!data.isEmpty()) {
    int begin = data.indexOf(INFOSEEK_AREA_BEGIN);
    if (begin >= 0) {
      begin += sizeof(INFOSEEK_AREA_BEGIN) -1;
      int end = data.indexOf(INFOSEEK_AREA_END, begin);
      if (end > 0) {
        ret = data.mid(begin, end - begin);
        if (ret.contains("\\u"))
          ret.replace("\\u0022", "\"");
      }
    }
  }
  return ret;
}

// EOF
