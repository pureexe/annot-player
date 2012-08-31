// niftytranslator.cc
// 8/21/2012
// See: https://github.com/alice0775/userChrome.js/blob/master/popupTranslate.uc.xul

#include "module/translator/niftytranslator.h"
#include "module/translator/niftytranslator_p.h"
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

//#define DEBUG "niftytranslator"
#include "module/debug/debug.h"

// - Translate -

QString
NiftyTranslator::translateUrl(const QString &text)
{ return NIFTY_API + text; }

void
NiftyTranslator::translate(const QString &text)
{
  if (!isEnabled())
    return;
  DOUT("enter: text =" << text);
  if (reply_ && isSynchronized()) {
    //reply_->abort();
    reply_->deleteLater();
    DOUT("abort previous reply");
  }
  reply_ = networkAccessManager()->get(QNetworkRequest(translateUrl(text)));
  DOUT("exit");
}

void
NiftyTranslator::processReply(QNetworkReply *reply)
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
    emit errorMessage(tr("network error from @nifty honyaku") + ": " + reply->errorString());
    DOUT("exit: error =" << reply->error() << ", reason =" << reply->errorString());
    return;
  }

  QByteArray data = reply->readAll();
  if (!data.isEmpty()) {
    int begin = data.indexOf(NIFTY_AREA_BEGIN);
    if (begin >= 0) {
      begin += sizeof(NIFTY_AREA_BEGIN) -1;
      int end = data.indexOf(NIFTY_AREA_END, begin);
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

  emit errorMessage(tr("network error from @nifty honyaku"));
  DOUT("exit: error");
}

// EOF
