// microsofttranslator.cc
// 11/2/2011

#include "module/translator/microsofttranslator.h"
#include "module/translator/microsofttranslator_p.h"
#ifdef WITH_MODULE_QTEXT
# include "module/qtext/network.h"
#endif // WITH_MODULE_QTEXT
#include <QtNetwork/QAuthenticator>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

//#define DEBUG "microsofttranslator"
#include "module/debug/debug.h"

// - Construction -

MicrosoftTranslator::MicrosoftTranslator(QObject *parent)
  : Base(parent)
{
  qnam_ = new QNetworkAccessManager(this);
  connect(qnam_, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), SLOT(authenticate(QNetworkReply*,QAuthenticator*)));
  connect(qnam_, SIGNAL(finished(QNetworkReply*)), SLOT(processNetworkReply(QNetworkReply*)));
}

// See: http://www.developer.nokia.com/Community/Discussion/showthread.php?211356-QNetworkRequest-Authentication
void
MicrosoftTranslator::authenticate(QAuthenticator *auth)
{
  Q_ASSERT(auth);
  DOUT("enter");
  auth->setUser(AZURE_AUTH_USERNAME);
  auth->setPassword(AZURE_AUTH_PASSWORD);
  DOUT("exit");
}

// - Translate -

// See: http://social.msdn.microsoft.com/Forums/nl-NL/DataMarket/thread/8b8e203d-b2d7-4c81-a3ff-2e1e4e65d18c
QUrl
MicrosoftTranslator::translateUrl(const QString &text, const QString &to, const QString &from)
{
#define QUOTE(_str) QString("'%1'").arg(_str)
  QUrl ret(AZURE_API);
  if (!from.isEmpty())
    ret.addQueryItem(AZURE_API_FROM, QUOTE(from));
  ret.addQueryItem(AZURE_API_TO, QUOTE(to));
  ret.addEncodedQueryItem(AZURE_API_TEXT, QUrl::toPercentEncoding(QUOTE(text)));
#undef QUOTE
  return ret;
}

QNetworkRequest
MicrosoftTranslator::translateRequest(const QUrl &url)
{
  QNetworkRequest ret(url);
#ifdef WITH_MODULE_QTEXT
  QtExt::setNetworkRequestAuthentificationHeader(&ret, AZURE_AUTH_USERNAME, AZURE_AUTH_PASSWORD);
#endif // WITH_MODULE_QTEXT
  return ret;
}

void
MicrosoftTranslator::translate(const QString &text, const QString &to, const QString &from)
{
  DOUT("enter");
  QUrl query = translateUrl(text, to, from);
  DOUT("query =" << query);
  qnam_->get(translateRequest(query));
  DOUT("exit");
}

// Sample request: http://api.microsofttranslator.com/v2/Http.svc/Translate?appId=FCB48AFBE3CB7B0E7AA146C950762FC87EA13FBB&text=hello&from=en&to=ja
// Sample reply:
// <feed xmlns:base="https://api.datamarket.azure.com/Data.ashx/Bing/MicrosoftTranslator/Translate" xmlns:d="http://schemas.microsoft.com/ado/2007/08/dataservices" xmlns:m="http://schemas.microsoft.com/ado/2007/08/dataservices/metadata" xmlns="http://www.w3.org/2005/Atom">
//   <title type="text" />
//   <subtitle type="text">Microsoft Translator</subtitle>
//   <id>https://api.datamarket.azure.com/Data.ashx/Bing/MicrosoftTranslator/Translate?Text='hello'&amp;To='ja'&amp;$top=100</id>
//   <rights type="text" />
//   <updated>2012-07-02T03:54:54Z</updated>
//   <link rel="self" href="https://api.datamarket.azure.com/Data.ashx/Bing/MicrosoftTranslator/Translate?Text='hello'&amp;To='ja'&amp;$top=100" />
//   <entry>
//     <id>https://api.datamarket.azure.com/Data.ashx/Bing/MicrosoftTranslator/Translate?Text='hello'&amp;To='ja'&amp;$skip=0&amp;$top=1</id>
//     <title type="text">Translation</title>
//     <updated>2012-07-02T03:54:54Z</updated>
//     <link rel="self" href="https://api.datamarket.azure.com/Data.ashx/Bing/MicrosoftTranslator/Translate?Text='hello'&amp;To='ja'&amp;$skip=0&amp;$top=1" />
//     <content type="application/xml">
//       <m:properties>
//         <d:Text m:type="Edm.String">‚±‚ñ‚É‚¿‚Í</d:Text>
//       </m:properties>
//     </content>
//   </entry>
// </feed>
void
MicrosoftTranslator::processNetworkReply(QNetworkReply *reply)
{
  DOUT("enter");
  Q_ASSERT(reply);
  reply->deleteLater();

  if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
    emit error(tr("network error from Microsoft Translator") + ": " + reply->errorString());
    DOUT("exit: error =" << reply->error() << ", reason =" << reply->errorString());
    return;
  }

  QDomDocument doc;
  doc.setContent(reply->readAll());
  if (!doc.isNull()) {
    QDomElement e = doc.documentElement();
    if (!e.isNull() && // "feed"
        !(e=e.firstChildElement("entry")).isNull() &&
        !(e=e.firstChildElement("content")).isNull()) {
      QDomNode n = e.firstChild(); // <m:properties>
      if (!n.isNull() &&
          !(e=n.firstChildElement()).isNull()) { // <d:Text>
        QString t = e.text();
        if (!t.isEmpty()) {
          DOUT("exit: ok:" << t);
          emit translated(t);
          return;
        }
      }
    }
  }

  emit error(tr("network error from Microsoft Translator"));
  DOUT("exit: error");
}

// EOF
