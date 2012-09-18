// kotobanktranslator.cc
// 8/20/2012

#include "lib/translator/kotobanktranslator.h"
#include "lib/translator/kotobanktranslator_p.h"
#include <QtCore/QUrl>
#include <QtCore/QRegExp>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

//#define DEBUG "kotobanktranslator"
#include "qtx/qxdebug.h"

// - Properties -

QString
KotobankTranslator::name() const
{ return tr("Kotobank"); }

// - Translate -

QString
KotobankTranslator::translateUrl(const QString &text)
{ return KOTOBANK_SEARCH_API + text; }

QNetworkReply*
KotobankTranslator::createReply(const QString &text)
{ return networkAccessManager()->get(QNetworkRequest(translateUrl(text))); }

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
  if (isSynchronized() && networkReply() != reply) {
    DOUT("exit: reply changed");
    return;
  }
  setNetworkReply(nullptr);

  Q_ASSERT(reply);
  reply->deleteLater();
  QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
  if (!redirectUrl.isEmpty()) {
    setNetworkReply(networkAccessManager()->get(QNetworkRequest(redirectUrl)));
    return;
  }
  if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
    emit errorMessage(tr("network error from %1").arg(name()) + ": " + reply->errorString());
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
    emit errorMessage(tr("network error from %1").arg(name()));
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

  emit errorMessage(tr("network error from %1").arg(name()));
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

  emit errorMessage(tr("network error from %1").arg(name()));
  DOUT("exit: error");
}

// EOF
