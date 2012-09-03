// yahootranslator.cc
// 8/21/2012
// See: http://chiitrans2.googlecode.com/git/Translation.cs

#include "module/translator/yahootranslator.h"
#include "module/translator/yahootranslator_p.h"
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

//#define DEBUG "yahootranslator"
#include "module/debug/debug.h"

// - Properties -

QString
YahooTranslator::name() const
{ return tr("Yahoo! Honyaku"); }

// - Translate -

// See: http://jane.s89.xrea.com/test/read.cgi/kakolog/1180559017/306-307n
// See: http://honyaku.yahoo.co.jp/lib/js/translation.js?2188
const char*
YahooTranslator::translateLanguage(const QString &lcode)
{
  if (lcode.isEmpty())
    return 0;
  else if (lcode == "en")
    return "E";
  else if (lcode == "ja")
    return "J";
  else if (lcode == "zh-CHS")
    return "C-CN";
  else if (lcode == "zh-CHT")
    return "C-CN"; // FIXME
  else if (lcode == "ko")
    return "K";
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
  else
    return 0;
}

QString
YahooTranslator::translateUrl(const QString &text, const char *to, const char *from)
{
  return YAHOO_API
     "&" YAHOO_KEY_LANG "=CR-" + QString(from).append(to) +
     "&" YAHOO_KEY_TEXT "=" + text;
}

QNetworkReply*
YahooTranslator::createReply(const QString &text, const QString &to, const QString &from)
{
  const char *f = translateLanguage(from),
             *t = translateLanguage(to);
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
