// yahootranslator.cc
// 8/21/2012
// See: http://chiitrans2.googlecode.com/git/Translation.cs

#include "lib/translator/yahootranslator.h"
#include "lib/translator/yahootranslator_p.h"
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

//#define DEBUG "yahootranslator"
#include "lib/debug/debug.h"

// - Properties -

QString
YahooTranslator::name() const
{ return tr("Yahoo! Honyaku"); }

// - Translate -

// See: http://jane.s89.xrea.com/test/read.cgi/kakolog/1180559017/306-307n
// See: http://honyaku.yahoo.co.jp/lib/js/translation.js?2188
const char*
YahooTranslator::lcode(int lang)
{
  switch (lang) {
  case English: return "E";
  case Japanese: return "J";
  case TraditionalChinese: // FIXME
  case SimplifiedChinese:
    return "C-CN";
  case Korean:
    return "K";
    // FIXME:
  //else if (lcode == "fr")
  //  return "F"; // FIXME
  //else if (lcode == "de")
  //  return "G"; // FIXME
  //else if (lcode == "it")
  //  return "I"; // FIXME
  //else if (lcode == "es")
  //  return "S"; // FIXME
  //else if (lcode == "pt")
  //  return "P"; // FIXME
  default:
    return 0;
  }
}

QString
YahooTranslator::translateUrl(const QString &text, const char *to, const char *from)
{
  return YAHOO_API
     "&" YAHOO_QUERY_LANG "=CR-" + QString(from).append(to) +
     "&" YAHOO_QUERY_TEXT "=" + text;
}

QNetworkReply*
YahooTranslator::createReply(const QString &text, int to, int from)
{
  const char *f = lcode(from),
             *t = lcode(to);
  QString url = translateUrl(text, t ? t : "E", f ? f : "J");
  return networkAccessManager()->get(QNetworkRequest(url));
}

QString
YahooTranslator::parseReply(const QByteArray &data)
{
  QByteArray ret;
  if (!data.isEmpty()) {
    int begin = data.indexOf(YAHOO_AREA_BEGIN);
    if (begin >= 0) {
      begin += sizeof(YAHOO_AREA_BEGIN) -1;
      begin = data.indexOf(YAHOO_AREA_BEGIN2, begin);
      if (begin >= 0) {
        begin += sizeof(YAHOO_AREA_BEGIN2) -1;
        int end = data.indexOf(YAHOO_AREA_END, begin);
        if (end > 0)
          ret = data.mid(begin, end - begin);
      }
    }
  }
  return ret;
}

// EOF
