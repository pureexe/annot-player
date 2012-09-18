// networktranslator.cc
// 8/26/2011

#include "lib/translator/networktranslator.h"
#include "lib/translator/translatorsettings.h"
#include "qtx/qxnetwork.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>

//#define DEBUG "networktranslator"
#include "qtx/qxdebug.h"

// - Properties -

#define REQ_TEXT_ATTR   QNetworkRequest::UserMax

QNetworkAccessManager*
NetworkTranslator::networkAccessManager() const
{
  if (!nam_) {
    Self *self = const_cast<Self *>(this);
    self->nam_ = new QNetworkAccessManager(self);
    connect(nam_, SIGNAL(finished(QNetworkReply*)), SLOT(processReply(QNetworkReply*)));
  }
  return nam_;
}

// - Actions -

void
NetworkTranslator::translate(const QString &text, int to, int from)
{
  if (!isEnabled() || text.isEmpty())
    return;
  if (TranslatorSettings::globalSettings()->isCacheEnabled() &&
      text.size() < TranslatorSettings::globalSettings()->cacheSize()) {
    auto p = cachedTranslation().find(text);
    if (p != cachedTranslation().end()) {
      if (reply_ && isSynchronized()) {
        //reply_->abort();
        reply_->deleteLater();
        reply_ = nullptr;
      }
      emit translated(p.value());
      return;
    }
  }
  doTranslate(text, to, from);
}

void
NetworkTranslator::doTranslate(const QString &text, int to, int from)
{
  if (!isEnabled())
    return;
  DOUT("enter: text =" << text);
  if (reply_ && isSynchronized()) {
    //reply_->abort();
    reply_->deleteLater();
    DOUT("abort previous reply");
  }

  reply_ = createReply(text, to, from);

  if (TranslatorSettings::globalSettings()->isCacheEnabled() &&
      text.size() <= TranslatorSettings::globalSettings()->cacheSize())
    QxPublicNetworkReply::fromReply(reply_)
        ->setAttribute(REQ_TEXT_ATTR, text);
  DOUT("exit");
}

void
NetworkTranslator::processReply(QNetworkReply *reply)
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
    emit errorMessage(tr("network error from %1").arg(name()) + ": " + reply->errorString());
    DOUT("exit: error =" << reply->error() << ", reason =" << reply->errorString());
    return;
  }

  QString text = parseReply(reply->readAll());
  if (text.isEmpty())  {
    emit errorMessage(tr("network error from %1").arg(name()));
    DOUT("exit: parse error");
    return;
  }

  QVariant v = reply->attribute(REQ_TEXT_ATTR);
  if (!v.isNull()) {
    QString t = v.toString();
    if (!t.isEmpty())
      cacheTranslation(t, text);
  }
  emit translated(text);
  DOUT("exit: translated text =" << text);
}

// EOF
