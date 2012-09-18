// jdictranslator.cc
// 8/11/2012

#include "lib/translator/jdictranslator.h"
#include "lib/translator/jdictranslator_p.h"
#include "qtx/qxbytearray.h"
#include <QtCore/QByteArray>
#include <QtCore/QRegExp>
#include <QtCore/QTextCodec>
#include <QtCore/QTextDecoder>
#include <QtCore/QTextEncoder>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>

//#define DEBUG "jdictranslator"
#include "qtx/qxdebug.h"

// - Construction -

JdicTranslator::JdicTranslator(QObject *parent)
  : Base(parent)
{
  QTextCodec *codec = QTextCodec::codecForName(JDIC_ENCODING);
  Q_ASSERT(codec);
  decoder_ = codec->makeDecoder();
  Q_ASSERT(decoder_);
  encoder_ = codec->makeEncoder();
  Q_ASSERT(encoder_);
}

QString
JdicTranslator::name() const
{ return "WWWJDIC"; }

const char*
JdicTranslator::dictionary(int lang)
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

QNetworkReply*
JdicTranslator::createReply(const QString &text, const char *dict)
{
  QByteArray data = postData(text, dict);
  QNetworkRequest req(QUrl(JDIC_API));
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  return networkAccessManager()->post(req, data);
}

// Sample:
//
// <div style="float:left;width:20px;padding-top:5px;">
// <INPUT TYPE="radio" NAME="jukugosel" VALUE="1350726_1370780_X"  CHECKED ID="LAB1350726">
// </div>
// ...
//
QString
JdicTranslator::parseReply(const QByteArray &data)
{
  QString ret;
  if (!data.isEmpty()) {
    int begin = data.indexOf(JDIC_DOC_BEGIN);
    if (begin > 0) {
      int end = data.indexOf(JDIC_DOC_END, begin);
      if (end < 0)
        end = data.indexOf(JDIC_DOC_END2, begin);
      if (end > 0) {
        QByteArray t = data.mid(begin, end - begin);
        ret = decodeText(t);
        ret.remove(JDIC_REMOVE_RE);
      }
    }
  }
  return ret;
}

// EOF
