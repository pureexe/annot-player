// fresheyetranslator.cc
// 8/21/2012
// See: http://ready.to/search/list/cs_trans.htm

#include "module/translator/fresheyetranslator.h"
#include "module/translator/fresheyetranslator_p.h"
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

//#define DEBUG "fresheyetranslator"
#include "module/debug/debug.h"

// - Translate -

QByteArray
FresheyeTranslator::postData(const QString &text, const QString &to, const QString &from)
{
  return (
    from.isEmpty() || from == "ja" ? (
      to == "zh-CHS" ? FRESHEYE_POST_JA_CN :
      to == "zh-CHT" ? FRESHEYE_POST_JA_TW :
      FRESHEYE_POST_JA_EN
  ) : from == "en" ? FRESHEYE_POST_EN_JA :
      from == "zh-CHS" ? FRESHEYE_POST_CN_JA :
      from == "zh-CHT" ? FRESHEYE_POST_TW_JA :
      FRESHEYE_POST_JA_EN
  ) + QUrl::toPercentEncoding(text);
}

void
FresheyeTranslator::translate(const QString &text, const QString &to, const QString &from)
{
  if (!isEnabled())
    return;
  DOUT("enter: text =" << text);
  if (reply_ && isSynchronized()) {
    //reply_->abort();
    reply_->deleteLater();
    DOUT("abort previous reply");
  }

  QString url = to.startsWith("zh-") ? FRESHEYE_API_ZH : FRESHEYE_API_EN;
  QNetworkRequest req(url);
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  reply_ = networkAccessManager()->post(req, postData(text, to, from));

  DOUT("exit");
}

void
FresheyeTranslator::processReply(QNetworkReply *reply)
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
    emit errorMessage(tr("network error from freshEYE Honyaku") + ": " + reply->errorString());
    DOUT("exit: error =" << reply->error() << ", reason =" << reply->errorString());
    return;
  }

  QByteArray data = reply->readAll();
  if (!data.isEmpty()) {
    int begin = data.indexOf(FRESHEYE_AREA_BEGIN);
    if (begin >= 0) {
      begin += sizeof(FRESHEYE_AREA_BEGIN) -1;
      begin = data.indexOf(FRESHEYE_AREA_BEGIN2, begin);
      if (begin >= 0) {
        begin += sizeof(FRESHEYE_AREA_BEGIN2) -1;
        int end = data.indexOf(FRESHEYE_AREA_END, begin);
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

  emit errorMessage(tr("network error from freshEYE Honyaku"));
  DOUT("exit: error");
}

// EOF
