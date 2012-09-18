// microsofttranslator.cc
// 11/2/2011

#include "lib/translator/microsofttranslator.h"
#include "lib/translator/microsofttranslator_p.h"
#ifdef WITH_LIB_QTX
# include "qtx/qxnetwork.h"
#endif // WITH_LIB_QTX
#include <QtNetwork/QAuthenticator>
#include <QtNetwork/QNetworkAccessManager>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

//#define DEBUG "microsofttranslator"
#include "qtx/qxdebug.h"

// - Construction -

MicrosoftTranslator::MicrosoftTranslator(QObject *parent)
  : Base(parent)
{ connect(networkAccessManager(), SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), SLOT(authenticate(QNetworkReply*,QAuthenticator*))); }

QString
MicrosoftTranslator::name() const
{ return tr("Microsoft Translator"); }

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

// See: http://msdn.microsoft.com/en-us/library/hh456380.aspx
const char*
MicrosoftTranslator::lcode(int lang)
{
  switch (lang) {
  case English:  return "en";
  case Japanese: return "ja";
  case TraditionalChinese:  return "zh-CHT";
  case SimplifiedChinese:   return "zh-CHS";
  case Korean:   return "ko";
  case French:   return "fr";
  case German:   return "de";
  case Italian:  return "it";
  case Spanish:  return "es";
  case Portuguese: return "pt";
  case Russian:  return "ru";
  default: return 0;
  }
}

// See: http://social.msdn.microsoft.com/Forums/nl-NL/DataMarket/thread/8b8e203d-b2d7-4c81-a3ff-2e1e4e65d18c
QUrl
MicrosoftTranslator::translateUrl(const QString &text, int to, int from)
{
#define QUOTE(_str) QString("'%1'").arg(_str)
  QUrl ret(AZURE_API);
  if (from)
    ret.addQueryItem(AZURE_QUERY_FROM, QUOTE(lcode(from)));
  ret.addQueryItem(AZURE_QUERY_TO, QUOTE(lcode(to)));
  ret.addEncodedQueryItem(AZURE_QUERY_TEXT, QUrl::toPercentEncoding(QUOTE(text)));
#undef QUOTE
  return ret;
}

QNetworkRequest
MicrosoftTranslator::translateRequest(const QUrl &url)
{
  QNetworkRequest ret(url);
#ifdef WITH_LIB_QTX
  QxNetwork::setNetworkRequestAuthentificationHeader(&ret, AZURE_AUTH_USERNAME, AZURE_AUTH_PASSWORD);
#endif // WITH_LIB_QTX
  return ret;
}

QNetworkReply*
MicrosoftTranslator::createReply(const QString &text, int to, int from)
{
  QUrl query = translateUrl(text, to, from);
  return networkAccessManager()->get(translateRequest(query));
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
QString
MicrosoftTranslator::parseReply(const QByteArray &data)
{
  QString ret;
  QDomDocument doc;
  doc.setContent(data);
  if (!doc.isNull()) {
    QDomElement e = doc.documentElement();
    if (!e.isNull() && // "feed"
        !(e=e.firstChildElement("entry")).isNull() &&
        !(e=e.firstChildElement("content")).isNull()) {
      QDomNode n = e.firstChild(); // <m:properties>
      if (!n.isNull() &&
          !(e=n.firstChildElement()).isNull()) { // <d:Text>
        ret = e.text();
      }
    }
  }
  return ret;
}

// EOF
