// infoseektranslator.cc
// 8/21/2012
// See: http://transer.com/sdk/rest_api_function.html
// See: http://translation.infoseek.ne.jp/js/translation-text.js

#include "module/translator/infoseektranslator.h"
#include "module/translator/infoseektranslator_p.h"
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

//#define DEBUG "infoseektranslator"
#include "module/debug/debug.h"

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

void
InfoseekTranslator::translate(const QString &text, const QString &to, const QString &from)
{
  if (!isEnabled())
    return;
  DOUT("enter: text =" << text);
  if (reply_ && isSynchronized()) {
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
InfoseekTranslator::processReply(QNetworkReply *reply)
{
  DOUT("enter");
  Q_ASSERT(reply);
  reply->deleteLater();

  if (isSynchronized() && reply_ != reply) {
    DOUT("exit: reply changed");
    return;
  }
  reply_ = nullptr;

  if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
    emit errorMessage(tr("network error from Infoseek Honyaku") + ": " + reply->errorString());
    DOUT("exit: error =" << reply->error() << ", reason =" << reply->errorString());
    return;
  }

  QByteArray data = reply->readAll();
  DOUT("data =" << data);
  if (!data.isEmpty()) {
    int begin = data.indexOf(INFOSEEK_AREA_BEGIN);
    if (begin >= 0) {
      begin += sizeof(INFOSEEK_AREA_BEGIN) -1;
      int end = data.indexOf(INFOSEEK_AREA_END, begin);
      if (end > 0) {
        QString text = data.mid(begin, end - begin);
        if (text.contains("\\u"))
          text.replace("\\u0022", "\"");
        DOUT("text =" << text);
        if (!text.isEmpty()) {
          emit translated(text);
          DOUT("exit: ok");
          return;
        }
      }
    }
  }

  emit errorMessage(tr("network error from Infoseek Honyaku"));
  DOUT("exit: error");
}

// EOF
