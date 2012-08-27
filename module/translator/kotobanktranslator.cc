// kotobanktranslator.cc
// 8/20/2012

#include "module/translator/kotobanktranslator.h"
#include "module/translator/kotobanktranslator_p.h"
#include "module/translator/translatorsettings.h"
#include <QtCore/QUrl>
#include <QtCore/QRegExp>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

//#define DEBUG "kotobanktranslator"
#include "module/debug/debug.h"

// - Translate -

QString
KotobankTranslator::translateUrl(const QString &text)
{ return KOTOBANK_SEARCH_API + text; }

void
KotobankTranslator::translate(const QString &text)
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

// Sample:
//
// <div style="float:left;width:20px;padding-top:5px;">
// <INPUT TYPE="radio" NAME="jukugosel" VALUE="1350726_1370780_X"  CHECKED ID="LAB1350726">
// </div>
// ...
//
void
KotobankTranslator::processReply(QNetworkReply *reply)
{
  DOUT("enter");
  if (TranslatorSettings::globalSettings()->isSynchronized() &&
      reply_ != reply) {
    DOUT("exit: reply changed");
    return;
  }
  reply_ = 0;

  Q_ASSERT(reply);
  reply->deleteLater();
  QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
  if (!redirectUrl.isEmpty()) {
    reply_ = networkAccessManager()->get(QNetworkRequest(redirectUrl));
    return;
  }
  if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
    emit errorMessage(tr("network error from Kotobank Translator") + ": " + reply->errorString());
    DOUT("exit: error =" << reply->error() << ", reason =" << reply->errorString());
    return;
  }

  QUrl url = reply->url();
  QString path = url.path();
  if (path.startsWith("/search/", Qt::CaseInsensitive)) {
    DOUT("search");
    processSearchData(reply->readAll());
  } else if (path.startsWith("/word/", Qt::CaseInsensitive)) {
    DOUT("word");
    processWordData(reply->readAll());
  } else {
    DOUT("error: unknown url:" << url);
    emit errorMessage(tr("network error from Kotobank Translator"));
  }
  DOUT("exit");
}

void
KotobankTranslator::processSearchData(const QByteArray &data)
{
  DOUT("enter");
  if (!data.isEmpty()) {
    int begin = data.indexOf(KOTOBANK_SEARCH_BEGIN);
    if (begin > 0) {
      int end = data.indexOf(KOTOBANK_SEARCH_END, begin);
      if (end > 0) {
        QString text = data.mid(begin, end - begin);
        text.remove(QRegExp(KOTOBANK_SEARCH_REMOVE_SPONSOR));

        DOUT("text =" << text);
        if (!text.isEmpty()) {
          emit translated(text);
          DOUT("exit: ok");
          return;
        }
      }
    }
  }

  emit errorMessage(tr("network error from KOTOBANK Translator"));
  DOUT("exit: error");
}

void
KotobankTranslator::processWordData(const QByteArray &data)
{
  DOUT("enter");
  if (!data.isEmpty()) {
    int begin = data.indexOf(KOTOBANK_WORD_BEGIN);
    if (begin > 0) {
      int end = data.indexOf(KOTOBANK_WORD_END, begin);
      if (end > 0) {
        QString text = data.mid(begin, end - begin);
        text.remove(QRegExp(KOTOBANK_WORD_REMOVE_HEADER))
            .remove(QRegExp(KOTOBANK_WORD_REMOVE_FOOTER))
            .remove(QRegExp(KOTOBANK_WORD_REMOVE_SEARCH));

        DOUT("text =" << text);
        if (!text.isEmpty()) {
          emit translated(text);
          DOUT("exit: ok");
          return;
        }
      }
    }
  }

  emit errorMessage(tr("network error from KOTOBANK Translator"));
  DOUT("exit: error");
}

// EOF
