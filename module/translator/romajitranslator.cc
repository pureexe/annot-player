// romajitranslator.cc
// 7/15/2012

#include "module/translator/romajitranslator.h"
#include "module/translator/romajitranslator_p.h"
#include "module/qtext/bytearray.h"
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QByteArray>
#include <QtCore/QTextCodec>
#include <QtCore/QTextDecoder>
#include <QtCore/QTextEncoder>

//#define DEBUG "romajitranslator"
#include "module/debug/debug.h"

// - Construction -

RomajiTranslator::RomajiTranslator(QObject *parent)
  : Base(parent)
{
  qnam_ = new QNetworkAccessManager(this);
  connect(qnam_, SIGNAL(finished(QNetworkReply*)), SLOT(processNetworkReply(QNetworkReply*)));

  QTextCodec *codec = QTextCodec::codecForName(ROMAJI_ENCODING);
  Q_ASSERT(codec);
  decoder_ = codec->makeDecoder();
  Q_ASSERT(decoder_);
  encoder_ = codec->makeEncoder();
  Q_ASSERT(encoder_);
}

// - Encoding -

QString
RomajiTranslator::decodeText(const QByteArray &data) const
{ return decoder_->toUnicode(data); }

QByteArray
RomajiTranslator::encodeText(const QString &data) const
{ return encoder_->fromUnicode(data); }

// - Translate -

QString
RomajiTranslator::translateUrl(const QString &text) const
{
  QByteArray data = encodeText(text);
  q_toPercentEncoding(&data);
  data.prepend(ROMAJI_API "?" ROMAJI_API_TEXT "=");
  QString url = QString::fromAscii(data);
  DOUT("url =" << url);
  return url;
}

void
RomajiTranslator::translate(const QString &text)
{
  DOUT("enter: text =" << text);
  QString query = translateUrl(text);
  DOUT("query =" << query);
  qnam_->get(QNetworkRequest(query));
  DOUT("exit");
}

// SIJS to %-encoding: http://www.url-encode-decode.com/
// Input: 何故だろう\nナニ><
// Sample request: http://www.romaji.org/?STB&text=%89%BD%8C%CC%82%BE%82%EB%82%A4%0A%83%69%83%6A%3E%3C
// Sample reply:
//
// <!-- ROMAJI.ORG DISPLAY RESULT AREA -->
// <table width="459" align="center" bgcolor="#9292D5" class="def" cellpadding="10">
// <tr rowspan="6"><td rowspan="6" bgcolor="#D9C8E2">
// <p>
// 何故だろう<br />
//  <font color="red">naze darou<br />
// </font>ナニ><<br />
//  <font color="red">nani ><<br />
// </font></p>
// </td></tr>
// </table>
// <!-- ROMAJI.ORG DISPLAY RESULT AREA -->
void
RomajiTranslator::processNetworkReply(QNetworkReply *reply)
{
  DOUT("enter");
  Q_ASSERT(reply);
  reply->deleteLater();

  if (!reply->isFinished() || reply->error() != QNetworkReply::NoError) {
    emit errorMessage(tr("network error from Romaji Translator") + ": " + reply->errorString());
    DOUT("exit: error =" << reply->error() << ", reason =" << reply->errorString());
    return;
  }

  enum {
    area_begin_len = sizeof(ROMAJI_AREA_BEGIN)  -1,
    line_begin_len = sizeof(ROMAJI_LINE_BEGIN) -1,
    line_end_len = sizeof(ROMAJI_LINE_BEGIN) -1
  };
  QByteArray data = reply->readAll();
  if (!data.isEmpty()) {
    int begin = data.indexOf(ROMAJI_AREA_BEGIN);
    if (begin >= 0) {
      begin += area_begin_len;
      int end = data.indexOf(ROMAJI_AREA_END, begin);
      if (end > 0) {
        data = data.mid(begin, end - begin);

        QString text;
        end = 0;
        forever {
          begin = data.indexOf(ROMAJI_LINE_BEGIN, end);
          if (begin < 0)
            break;
          begin += line_begin_len;
          end = data.indexOf(ROMAJI_LINE_END, begin);
          if (end < 0)
            break;
          QByteArray line = data.mid(begin, end - begin);
          end += line_end_len;

          QString t = decodeText(line);
          if (!t.isEmpty()) {
            if (text.isEmpty())
              text = t;
            else
              text.append('\n')
                  .append(t);
          }
        }

        DOUT("text =" << text);
        if (!text.isEmpty()) {
          emit translated(text);
          return;
        }
      }
    }
  }

  emit errorMessage(tr("network error from Romaji Translator"));
  DOUT("exit: error");
}

// EOF
