// excitetranslator.cc
// 8/20/2012
// See: https://github.com/alice0775/userChrome.js/blob/master/popupTranslate.uc.xul

#include "module/translator/excitetranslator.h"
#include "module/translator/excitetranslator_p.h"
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

//#define DEBUG "excitetranslator"
#include "module/debug/debug.h"

// - Translate -

QString
ExciteTranslator::translateUrl(const QString &text)
{ return EXCITE_API + text; }

void
ExciteTranslator::translate(const QString &text)
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
ExciteTranslator::processReply(QNetworkReply *reply)
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
    emit errorMessage(tr("network error from Excite Honyaku") + ": " + reply->errorString());
    DOUT("exit: error =" << reply->error() << ", reason =" << reply->errorString());
    return;
  }

  QByteArray data = reply->readAll();
  if (!data.isEmpty()) {
    int begin = data.indexOf(EXCITE_AREA_BEGIN);
    if (begin >= 0) {
      begin += sizeof(EXCITE_AREA_BEGIN) -1;
      begin = data.indexOf(EXCITE_AREA_BEGIN2, begin);
      if (begin >= 0) {
        begin += sizeof(EXCITE_AREA_BEGIN2) -1;
        int end = data.indexOf(EXCITE_AREA_END, begin);
        if (end > 0) {
          data = data.mid(begin, end - begin);
          data.replace("&#010;", "\n");
          QString text = data;
          if (text[text.size() - 1] == '\n')
            text.chop(1);
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

  emit errorMessage(tr("network error from Excite Honyaku"));
  DOUT("exit: error");
}

// EOF
