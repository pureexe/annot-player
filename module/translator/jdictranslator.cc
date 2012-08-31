// jdictranslator.cc
// 8/11/2012

#include "module/translator/jdictranslator.h"
#include "module/translator/jdictranslator_p.h"
#include "module/qtext/bytearray.h"
#include <QtCore/QByteArray>
#include <QtCore/QRegExp>
#include <QtCore/QTextCodec>
#include <QtCore/QTextDecoder>
#include <QtCore/QTextEncoder>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

//#define DEBUG "jdictranslator"
#include "module/debug/debug.h"

// - Construction -

JdicTranslator::JdicTranslator(QObject *parent)
  : Base(parent), reply_(nullptr)
{
  QTextCodec *codec = QTextCodec::codecForName(JDIC_ENCODING);
  Q_ASSERT(codec);
  decoder_ = codec->makeDecoder();
  Q_ASSERT(decoder_);
  encoder_ = codec->makeEncoder();
  Q_ASSERT(encoder_);
}

const char*
JdicTranslator::dictionaryForLanguage(const QString &lang)
{
  // FIXME: only enlish dictionary is working, others would incur network error
  Q_UNUSED(lang)
  return JDIC_DICT_EN;
  //if (lang.isEmpty())
  //  return JDIC_DICT_EN;
  //else if (lang == "es")
  //  return JDIC_DICT_ES;
  //else if (lang == "it")
  //  return JDIC_DICT_IT;
  //else if (lang == "de")
  //  return JDIC_DICT_DE;
  //else if (lang == "fr")
  //  return JDIC_DICT_FR;
  //else if (lang == "ru")
  //  return JDIC_DICT_RU;
  //else
  //  return JDIC_DICT_EN;
}

// - Encoding -

QString
JdicTranslator::decodeText(const QByteArray &data) const
{ return decoder_->toUnicode(data); }

QByteArray
JdicTranslator::encodeText(const QString &data) const
{ return encoder_->fromUnicode(data); }

// - Translate -

QByteArray
JdicTranslator::postData(const QString &text, const char *dict) const
{
  QByteArray ret = encodeText(text);
  q_toPercentEncoding(&ret);
  ret.prepend("&" JDIC_POST_TEXT "=");
  ret.prepend(dict);
  ret.prepend(JDIC_POST_LANG "=");
  return ret;
}

void
JdicTranslator::translate(const QString &text, const char *dict)
{
  if (!isEnabled())
    return;
  DOUT("enter: text =" << text);
  if (reply_ && isSynchronized()) {
    //reply_->abort();
    reply_->deleteLater();
    DOUT("abort previous reply");
  }
  QByteArray data = postData(text, dict);
  QNetworkRequest req(QUrl(JDIC_API));
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  reply_ = networkAccessManager()->post(req, data);
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
JdicTranslator::processReply(QNetworkReply *reply)
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
    emit errorMessage(tr("network error from WWWJDIC Translator") + ": " + reply->errorString());
    DOUT("exit: error =" << reply->error() << ", reason =" << reply->errorString());
    return;
  }

  QByteArray data = reply->readAll();
  if (!data.isEmpty()) {
    int begin = data.indexOf(JDIC_DOC_BEGIN);
    if (begin > 0) {
      int end = data.indexOf(JDIC_DOC_END, begin);
      if (end < 0)
        end = data.indexOf(JDIC_DOC_END2, begin);
      if (end > 0) {
        data = data.mid(begin, end - begin);
        QString text = decodeText(data);
        text.remove(JDIC_REMOVE_RE);

        DOUT("text =" << text);
        if (!text.isEmpty()) {
          emit translated(text);
          DOUT("exit: ok");
          return;
        }
      }
    }
  }

  emit errorMessage(tr("network error from WWWJDIC Translator"));
  DOUT("exit: error");
}

// EOF
