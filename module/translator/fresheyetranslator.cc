// fresheyetranslator.cc
// 8/21/2012
// See: http://ready.to/search/list/cs_trans.htm

#include "module/translator/fresheyetranslator.h"
#include "module/translator/fresheyetranslator_p.h"
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>

//#define DEBUG "fresheyetranslator"
#include "module/debug/debug.h"

// - Properties -

QString
FresheyeTranslator::name() const
{ return tr("freshEYE Honyaku"); }

// - Translate -

QByteArray
FresheyeTranslator::postData(const QString &text, int to, int from)
{
  return (
    !from || from == Japanese ? (
      to == TraditionalChinese ? FRESHEYE_POST_JA_TW :
      to == SimplifiedChinese ? FRESHEYE_POST_JA_CN :
      FRESHEYE_POST_JA_EN
  ) : from == English ? FRESHEYE_POST_EN_JA :
      from == TraditionalChinese ? FRESHEYE_POST_TW_JA :
      from == SimplifiedChinese ? FRESHEYE_POST_CN_JA :
      FRESHEYE_POST_JA_EN
  ) + QUrl::toPercentEncoding(text);
}

QNetworkReply*
FresheyeTranslator::createReply(const QString &text, int to, int from)
{
  DOUT("enter: text =" << text);
  QString url = isChineseLanguage(to) ? FRESHEYE_API_ZH : FRESHEYE_API_EN;
  QNetworkRequest req(url);
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  DOUT("exit");
  return networkAccessManager()->post(req, postData(text, to, from));
}

QString
FresheyeTranslator::parseReply(const QByteArray &data)
{
  QByteArray ret;
  if (!data.isEmpty()) {
    int begin = data.indexOf(FRESHEYE_AREA_BEGIN);
    if (begin >= 0) {
      begin += sizeof(FRESHEYE_AREA_BEGIN) -1;
      begin = data.indexOf(FRESHEYE_AREA_BEGIN2, begin);
      if (begin >= 0) {
        begin += sizeof(FRESHEYE_AREA_BEGIN2) -1;
        int end = data.indexOf(FRESHEYE_AREA_END, begin);
        if (end > 0)
          ret = data.mid(begin, end - begin);
      }
    }
  }
  return ret;
}

// EOF
