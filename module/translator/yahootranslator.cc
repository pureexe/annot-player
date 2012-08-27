// yahootranslator.cc
// 8/21/2012
// See: http://chiitrans2.googlecode.com/git/Translation.cs

#include "module/translator/yahootranslator.h"
#include "module/translator/yahootranslator_p.h"
#include "module/translator/translatorsettings.h"
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

//#define DEBUG "yahootranslator"
#include "module/debug/debug.h"

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

void
YahooTranslator::translate(const QString &text, const QString &to, const QString &from)
{
  if (!isEnabled())
    return;
  DOUT("enter: text =" << text);
  if (reply_ && TranslatorSettings::globalSettings()->isSynchronized()) {
    //reply_->abort();
    reply_->deleteLater();
    DOUT("abort previous reply");
  }
  const char *f = translateLanguage(from),
             *t = translateLanguage(to);
  QString url = translateUrl(text, t ? t : "E", f ? f : "J");
  DOUT("url =" << url);
  reply_ = networkAccessManager()->get(QNetworkRequest(url));
  DOUT("exit");
}

void
YahooTranslator::processReply(QNetworkReply *reply)
{
  DOUT("enter");
  Q_ASSERT(reply);
  reply->deleteLater();

  if (TranslatorSettings::globalSettings()->isSynchronized() &&
      reply_ != reply) {
    DOUT("exit: reply changed");
    return;
  }
  reply_ = 0;

  if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
    emit errorMessage(tr("network error from Yahoo! Honyaku") + ": " + reply->errorString());
    DOUT("exit: error =" << reply->error() << ", reason =" << reply->errorString());
    return;
  }

  QByteArray data = reply->readAll();
  if (!data.isEmpty()) {
    int begin = data.indexOf(YAHOO_AREA_BEGIN);
    if (begin >= 0) {
      begin += sizeof(YAHOO_AREA_BEGIN) -1;
      begin = data.indexOf(YAHOO_AREA_BEGIN2, begin);
      if (begin >= 0) {
        begin += sizeof(YAHOO_AREA_BEGIN2) -1;
        int end = data.indexOf(YAHOO_AREA_END, begin);
        if (end > 0) {
          QString text = data.mid(begin, end - begin);
          DOUT("text =" << text);
          if (!text.isEmpty()) {
            emit translated(text);
            DOUT("exit: ok");
            return;
          }
        }
      }
    }
  }

  emit errorMessage(tr("network error from Yahoo! Honyaku"));
  DOUT("exit: error");
}

// EOF
