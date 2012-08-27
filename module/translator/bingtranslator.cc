// bingtranslator.cc
// 11/2/2011

#include "module/translator/bingtranslator.h"
#include "module/translator/bingtranslator_p.h"
#include "module/translator/translatorsettings.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

//#define DEBUG "bingtranslator"
#include "module/debug/debug.h"

// - Translate -

QUrl
BingTranslator::translateUrl(const QString &text, const QString &to, const QString &from)
{
  QUrl ret(BING_API);
  if (!from.isEmpty())
    ret.addQueryItem(BING_API_FROM, from);
  ret.addQueryItem(BING_API_TO, to);
  ret.addQueryItem(BING_API_APPID, BING_APPID);
  ret.addEncodedQueryItem(BING_API_TEXT, QUrl::toPercentEncoding(text));
  return ret;
}

void
BingTranslator::translate(const QString &text, const QString &to, const QString &from)
{
  if (!isEnabled())
    return;
  DOUT("enter");
  if (reply_ && TranslatorSettings::globalSettings()->isSynchronized()) {
    //reply_->abort();
    reply_->deleteLater();
    DOUT("abort previous reply");
  }
  QUrl query = translateUrl(text, to, from);
  DOUT("query =" << query);
  reply_ = networkAccessManager()->get(QNetworkRequest(query));
  DOUT("exit");
}

// Sample request: http://api.microsofttranslator.com/v2/Http.svc/Translate?appId=FCB48AFBE3CB7B0E7AA146C950762FC87EA13FBB&text=hello&from=en&to=ja
// Sample reply: <string xmlns="http://schemas.microsoft.com/2003/10/Serialization/">こんにちは</string>
void
BingTranslator::processReply(QNetworkReply *reply)
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
    emit errorMessage(tr("network error from Bing Translator") + ": " + reply->errorString());
    DOUT("exit: error =" << reply->error());
    return;
  }

  QDomDocument doc;
  doc.setContent(reply->readAll());
  if (!doc.isNull()) {
    QDomElement e = doc.documentElement();
    if (e.tagName() == "string") {
      QString t = e.text();
      if (!t.isEmpty()) {
        emit translated(t);
        DOUT("exit: ok:" << t);
        return;
      }
    }
  }

  emit errorMessage(tr("network error from Bing Translator"));
  DOUT("exit");
}

// EOF
