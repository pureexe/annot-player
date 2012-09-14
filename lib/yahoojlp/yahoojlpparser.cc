// yahoojlpparser.cc
// 9/13/2012

#include "lib/yahoojlp/yahoojlpparser.h"
#include "lib/yahoojlp/yahoojlpapis_p.h"
#include "lib/qtext/network.h"
#include "lib/annotcloud/annottag.h"
#include "lib/qtext/htmltag.h"
#include <QtCore/QHash>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtGui/QColor>
#include <QtGui/QTextCharFormat>

#define DEBUG "yahoojlpparser"
#include "lib/debug/debug.h"
//#include <QtCore/QDebug>

#define RenderHintsAttribute    QNetworkRequest::UserMax

// - Properties -

QNetworkAccessManager*
YahooJlpParser::networkAccessManager() const
{
  if (!nam_) {
    Self *self = const_cast<Self *>(this);
    self->nam_ = new QNetworkAccessManager(self);
    connect(nam_, SIGNAL(finished(QNetworkReply*)), SLOT(processReply(QNetworkReply*)));
  }

  return nam_;
}

// - Action -

void
YahooJlpParser::renderText(const QString &text, ulong hints)
{
  if (text.isEmpty())
    return;
  DOUT("enter: text =" << text);
  if (reply_ && isSynchronized()) {
    //reply_->abort();
    reply_->deleteLater();
    DOUT("abort previous reply");
  }

  reply_ = createReply(text);
  if (hints)
    QtExt::PublicNetworkReply::fromReply(reply_)
      ->setAttribute(RenderHintsAttribute, uint(hints));
  DOUT("exit");
}

QNetworkReply*
YahooJlpParser::createReply(const QString &text)
{
  QUrl url(YAHOO_MA_API);
  url.addQueryItem(YAHOO_QUERY_ID, YAHOO_APPID);
  url.addEncodedQueryItem(YAHOO_QUERY_TEXT, QUrl::toPercentEncoding(text));
  return networkAccessManager()->get(QNetworkRequest(url));
}

void
YahooJlpParser::processReply(QNetworkReply *reply)
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
    emit errorMessage(tr("network error from Yahoo!JAPAN") + ": " + reply->errorString());
    DOUT("exit: error =" << reply->error() << ", reason =" << reply->errorString());
    return;
  }

  QByteArray data = reply->readAll();
  DOUT("reply =" << data);

  QDomDocument doc;
  doc.setContent(data);
  if (doc.isNull()) {
    emit errorMessage(tr("network error from Yahoo!JAPAN"));
    DOUT("exit: invalid or empty document");
    return;
  }

  QDomElement result = doc.documentElement() // supposed to be "ResultSet"
                          .firstChildElement("ma_result");
  if (!result.isNull()) {
    DOUT("contains ma_result");
    result = result.firstChildElement("word_list");
  }
  if (result.isNull()) {
    DOUT("exit: invalid or empty result");
    return;
  }

  ulong hints = reply->attribute(RenderHintsAttribute).toUInt();
  QString ret;
  QString yomigana;
  int i = 0, j = 0;

  QDomElement word = result.firstChildElement("word");
  while (!word.isNull()) {
    QString surface = word.firstChildElement("surface").text(),
            pos = word.firstChildElement("pos").text();

    DOUT("pos =" << pos << ", surface =" << surface);

    QString fmt;
    QColor color;
    bool showFurigana = false;

    switch (qHash(pos)) {
    case H_Noun:
    case H_Verb:
      showFurigana = true;
    //case CT_Katagana:
      if (hints & ResizeHint)
        fmt.append(CORE_CMD_LATEX_BIG);
      if (hints & ColorHint) {
        color = ++i%2 ? Qt::red : Qt::cyan;
        color.setAlpha(80);
      }
      break;

    case H_Special:
      if (!surface.isEmpty()) {
        yomigana.append(HTML_TAG(td));
        ret.append(
          HTML_TD_OPEN(align=center)
            CORE_CMD_COLOR_BACKGROUND "[rgba(0,0,0,20)]" "{" + surface + "}"
          HTML_TD_CLOSE()
        );
      } continue;
      Q_ASSERT(0);

    default: // modifier
      if (hints & ColorHint) {
        color = ++j%2 ? Qt::green : Qt::magenta;
        color.setAlpha(60);
      }
      //showFurigana = true;
    }

    QString furigana = word.firstChildElement("reading").text();
    if (!furigana.size() == 1)
      switch (furigana[0].unicode()) {
      case '0': furigana = "‚ê‚¢"; break;
      case '1': furigana = "‚¢‚¿"; break;
      case '2': furigana = "‚É"; break;
      case '3': furigana = "‚³‚ñ"; break;
      case '4': furigana = "‚æ‚ñ"; break;
      case '5': furigana = "‚²"; break;
      case '6': furigana = "‚ë‚­"; break;
      case '7': furigana = "‚µ‚¿"; break;
      case '8': furigana = "‚Í‚¿"; break;
      case '9': furigana = "‚«‚ã‚¤"; break;
      }

    yomigana.append(furigana.isEmpty() || furigana == surface ? HTML_TAG(td) :
      HTML_TD_OPEN(align=center)
        "{"
          + QString(hints & ResizeHint ? CORE_CMD_LATEX_SMALL : CORE_CMD_LATEX_TINY) +
          CORE_CMD_COLOR_BACKGROUND "[rgba(0,0,0,20)]"
          + furigana +
        "}"
      HTML_TD_CLOSE()
    );

    if (color.isValid())
      fmt.append(
        CORE_CMD_COLOR_BACKGROUND "["
        +
        QString("rgba(%1,%2,%3,%4)")
          .arg(QString::number(color.red()))
          .arg(QString::number(color.green()))
          .arg(QString::number(color.blue()))
          .arg(QString::number(color.alpha()))
        + "]"
      );
    else
      fmt.append(CORE_CMD_COLOR_BACKGROUND "[rgba(0,0,0,20)]");

    ret.append(
      HTML_TD_OPEN(align=center)
        "{" + fmt + surface + "}"
      HTML_TD_CLOSE()
    );

    word = word.nextSiblingElement("word");
  }

  ret =
    HTML_CENTER_OPEN()
    HTML_TABLE_OPEN()
      HTML_TR_OPEN(valign=bottom) + yomigana + HTML_TR_CLOSE()
      HTML_TR_OPEN(valign=bottom) + ret + HTML_TR_CLOSE()
    HTML_TABLE_CLOSE()
    HTML_CENTER_CLOSE();

  emit textRendered(ret);
  DOUT("exit: ret =" << ret);
}

// EOF
