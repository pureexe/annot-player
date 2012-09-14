// infoseektranslator.cc
// 8/21/2012
// See: http://transer.com/sdk/rest_api_function.html
// See: http://translation.infoseek.ne.jp/js/translation-text.js

#include "lib/translator/infoseektranslator.h"
#include "lib/translator/infoseektranslator_p.h"
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>

//#define DEBUG "infoseektranslator"
#include "lib/debug/debug.h"

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
InfoseekTranslator::lcode(int lang)
{
  switch (lang) {
  case English:  return "E";
  case Japanese: return "J";
  case TraditionalChinese: return "CT";
  case SimplifiedChinese: return "C";
  case Korean:  return "K";
  case French:  return "F";
  case German:  return "G";
  case Italian: return "I";
  case Spanish: return "S";
  case Portuguese: return "P";
  default:      return 0;
  }
}

QString
InfoseekTranslator::translateUrl(const QString &text, const char *to, const char *from)
{
  return INFOSEEK_API
     "&" INFOSEEK_QUERY_AUTH "=" INFOSEEK_AUTH_USER "+" INFOSEEK_AUTH_KEY
     "&" INFOSEEK_QUERY_LANG "=" + QString(from).append(to) +
     "&" INFOSEEK_QUERY_TEXT "=" + text;
}

QNetworkReply*
InfoseekTranslator::createReply(const QString &text, int to, int from)
{
  const char *f = lcode(from),
             *t = lcode(to);
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
