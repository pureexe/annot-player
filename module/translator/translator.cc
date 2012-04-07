// translator.cc
// 11/2/2011

#include "translator.h"
#include "translator_config.h"
#include <QtCore>
#include <QtNetwork>
#include <QtXml>

//#define DEBUG "translator"
#include "module/debug/debug.h"

// - Construction -

// See: http://www.emreakkas.com/internationalization/microsoft-translator-api-languages-list-language-codes-and-names
const char *Translator::English = "en";
const char *Translator::Japanese = "ja";
const char *Translator::Korean = "ko";
const char *Translator::SimplifiedChinese = "zh-CHS";
const char *Translator::TraditionalChinese = "zh-CHT";

Translator::Translator(QObject *parent)
  : Base(parent)
{
  qnam_ = new QNetworkAccessManager(this);
  connect(qnam_, SIGNAL(finished(QNetworkReply*)), SLOT(processNetworkReply(QNetworkReply*)));
}

// - Translate -

QUrl
Translator::translationQuery(const QString &text, const QString &to, const QString &from)
{
  QUrl ret(BING_API);
  if (!from.isEmpty())
    ret.addQueryItem(BING_APIKEY_FROM, from);
  ret.addQueryItem(BING_APIKEY_TO, to);
  ret.addQueryItem(BING_APIKEY_APPID, BING_APPID);
  ret.addEncodedQueryItem(BING_APIKEY_TEXT, QUrl::toPercentEncoding(text));
  return ret;
}

void
Translator::translate(const QString &text, const QString &to, const QString &from) const
{
  DOUT("enter");
  QUrl query = translationQuery(text, to, from);
  DOUT("query =" << query);
  qnam_->get(QNetworkRequest(query));
  DOUT("exit");
}

// Sample request: http://api.microsofttranslator.com/v2/Http.svc/Translate?appId=FCB48AFBE3CB7B0E7AA146C950762FC87EA13FBB&text=hello&from=en&to=ja
// Sample reply: <string xmlns="http://schemas.microsoft.com/2003/10/Serialization/">こんにちは</string>
void
Translator::processNetworkReply(QNetworkReply *reply)
{
  DOUT("enter");
  Q_ASSERT(reply);
  reply->deleteLater();

  if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
    emit networkError(reply->errorString());
    DOUT("exit: error =" << reply->error());
    return;
  }

  QDomDocument doc;
  doc.setContent(reply->readAll());
  if (doc.isNull()) {
    emit networkError(tr("invalid server reply with null document root"));
    DOUT("exit: error, invalid document root");
    return;
  }

  QDomElement e = doc.documentElement();

  if (e.tagName() != "string") {
    emit networkError(tr("server reply with root tag") + QString(" (tag = %1)").arg(e.tagName()));
    DOUT("exit: error, invalid root tag =" << e.tagName());
    return;
  }

  QString received = e.text();
  DOUT("received =" << received);
  emit translated(received);
  DOUT("exit");
}

// EOF
