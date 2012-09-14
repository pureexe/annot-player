// dynamicocntranslator.cc
// 8/20/2012
// See: http://yassblog.blog.so-net.ne.jp/2010-06-24
// See: http://www.emacswiki.org/emacs/trans-ej.el

#include "module/translator/dynamicocntranslator.h"
#include "module/translator/ocntranslator_p.h"
#include "module/qtext/network.h"
#include <QtCore/QString>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

//#define DEBUG "ocntranslator"
#include "module/debug/debug.h"

#define RequestTextAttribute QNetworkRequest::UserMax
#define RequestToAttribute   QNetworkRequest::Attribute(QNetworkRequest::UserMax -1)
#define RequestFromAttribute QNetworkRequest::Attribute(QNetworkRequest::UserMax -2)

// - Authentification -

QByteArray
DynamicOcnTranslator::parseAuthKey(const QByteArray &data)
{
  DOUT("enter: data =" << data);
  QByteArray ret;
  if (!data.isEmpty()) {
    int begin = data.indexOf(OCN_AUTH_BEGIN);
    if (begin >= 0) {
      begin += sizeof(OCN_AUTH_BEGIN) -1;
      int end = data.indexOf(OCN_AUTH_END, begin);
      if (end > 0)
        ret = data.mid(begin, end - begin);
    }
  }
  DOUT("exit: ret =" << ret);
  return ret;
}

// - Translate -

QByteArray
DynamicOcnTranslator::postData(const QString &text, const QString &to, const QString &from, const QByteArray &key)
{
  const char *lang_to =
    to == "en" ? "en" :
    to == "ko" ? "ko" :
    to == "zh-CHS" ? "zh" :
    to == "zh-CHT" ? "tw" :
    "en";

  const char *lang_from =
    from.isEmpty() ? "ja" :
    from == "en" ? "en" :
    from == "ko" ? "ko" :
    from == "zh-CHS" ? "zh" :
    from == "zh-CHT" ? "tw" :
    "ja";

  QByteArray ret = QUrl::toPercentEncoding(text);
  return ret.prepend("&" OCN_KEY_TEXT "=")
     .prepend(key).prepend("&" OCN_KEY_AUTH "=")
     .prepend(lang_to).prepend(lang_from).prepend(OCN_KEY_LANG "=");
}

void
DynamicOcnTranslator::translate(const QString &text, const QString &to, const QString &from)
{
  if (!isEnabled())
    return;
  DOUT("enter");
  if (reply_ && isSynchronized()) {
    //reply_->abort();
    reply_->deleteLater();
    DOUT("abort previous reply");
  }

  DOUT("update auth key");
  reply_ = networkAccessManager()->get(QNetworkRequest(QUrl(OCN_AUTH_API)));

  auto p = QtExt::PublicNetworkReply::fromReply(reply_);
  p->setAttribute(RequestTextAttribute, text);
  p->setAttribute(RequestToAttribute, to);
  if (!from.isEmpty())
    p->setAttribute(RequestFromAttribute, from);
  DOUT("exit");
}

void
DynamicOcnTranslator::processReply(QNetworkReply *reply)
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
    emit errorMessage(tr("network error from OCN Honyaku") + ": " + reply->errorString());
    DOUT("exit: error =" << reply->error() << ", reason =" << reply->errorString());
    return;
  }

  if (reply->url().path().endsWith("index.cgi")) {
    processData(reply->readAll());
    DOUT("exit: process data");
    return;
  }

  QByteArray key = parseAuthKey(reply->readAll());
  if (key.isEmpty()) {
    emit errorMessage(tr("network error from OCN Honyaku"));
    DOUT("exit: empty authentification key");
  }

  QString text = reply->attribute(RequestTextAttribute).toString(),
          to = reply->attribute(RequestToAttribute).toString(),
          from = reply->attribute(RequestFromAttribute).toString();

  DOUT("enter: to =" << to << ", text =" << text);
  QByteArray data = postData(text, to, from, key);
  QNetworkRequest req(QUrl(OCN_API));
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  reply_ = networkAccessManager()->post(req, data);
  DOUT("exit: repost request");
}

void
DynamicOcnTranslator::processData(const QByteArray &data)
{
  if (!data.isEmpty()) {
    int begin = data.indexOf(OCN_AREA_BEGIN);
    if (begin >= 0) {
      begin += sizeof(OCN_AREA_BEGIN) -1;
      begin = data.indexOf(OCN_AREA_BEGIN2, begin);
      if (begin >= 0) {
        begin += sizeof(OCN_AREA_BEGIN2) -1;
        int end = data.indexOf(OCN_AREA_END, begin);
        if (end > 0) {
          QByteArray text = data.mid(begin, end - begin);
          DOUT("text =" << text);
          if (!text.isEmpty() && text != OCN_FORBIDDEN_TEXT) {
            emit translated(text);
            DOUT("exit: ok");
            return;
          }
        }
      }
    }
  }

  emit errorMessage(tr("network error from OCN Honyaku"));
  DOUT("exit: error");
}

// EOF
