// sdltranslator.cc
// 8/21/2012
// See: https://github.com/alice0775/userChrome.js/blob/master/popupTranslate.uc.xul

#include "module/translator/sdltranslator.h"
#include "module/translator/sdltranslator_p.h"
#include "module/translator/translatorsettings.h"
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

//#define DEBUG "sdltranslator"
#include "module/debug/debug.h"

// - Translate -

QString
SdlTranslator::translateUrl(const QString &text)
{ return SDL_API + text; }

void
SdlTranslator::translate(const QString &text)
{
  if (!isEnabled())
    return;
  DOUT("enter: text =" << text);
  if (reply_ && TranslatorSettings::globalSettings()->isSynchronized()) {
    //reply_->abort();
    reply_->deleteLater();
    DOUT("abort previous reply");
  }
  reply_ = networkAccessManager()->get(QNetworkRequest(translateUrl(text)));
  DOUT("exit");
}

void
SdlTranslator::processReply(QNetworkReply *reply)
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
    emit errorMessage(tr("network error from SDL Translator") + ": " + reply->errorString());
    DOUT("exit: error =" << reply->error() << ", reason =" << reply->errorString());
    return;
  }

  QString text = reply->readAll();
  if (text.isEmpty()) {
    emit errorMessage(tr("network error from SDL Translator"));
    DOUT("exit: error");
  }

  emit translated(text);
  DOUT("exit: ok");
}

// EOF
