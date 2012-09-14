// yahoojlphighlighter.cc
// 9/8/2012

#include "lib/yahoojlp/yahoojlphighlighter.h"
#include "lib/yahoojlp/yahoojlpapis_p.h"
#include <QtCore/QHash>
#include <QtCore/QUrl>
#include <QtGui/QTextCharFormat>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#define DEBUG "yahoojlphighlighter"
#include "lib/debug/debug.h"
//#include <QtCore/QDebug>

// - Properties -

QNetworkAccessManager*
YahooJlpHighlighter::networkAccessManager() const
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
YahooJlpHighlighter::highlightBlock(const QString &text)
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
  DOUT("exit");
}

QNetworkReply*
YahooJlpHighlighter::createReply(const QString &text)
{
  QUrl url(YAHOO_MA_API);
  url.addQueryItem(YAHOO_QUERY_ID, YAHOO_APPID);
  url.addEncodedQueryItem(YAHOO_QUERY_TEXT, QUrl::toPercentEncoding(text));
  return networkAccessManager()->get(QNetworkRequest(url));
}

void
YahooJlpHighlighter::processReply(QNetworkReply *reply)
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

  int start = 0;
  int i = 0, j = 0;
  QTextCharFormat fmt;
  QColor color;
  int alpha;
  int sz;
  QDomElement word = result.firstChildElement("word");
  while (!word.isNull()) {
    QString surface = word.firstChildElement("surface").text(),
            pos = word.firstChildElement("pos").text();

    DOUT("pos =" << pos << ", surface =" << surface);

    bool skip = false;
    switch (qHash(pos)) {
    case H_Noun:
    case H_Verb:
      sz = 14;
      alpha = 40;
      color = ++i%2 ? Qt::green : Qt::magenta;
      break;
    case H_Special:
      skip = true;
      break;
    default: // modifier
      sz = 10;
      alpha = 20;
      color = ++j%2 ? Qt::red : Qt::cyan;
    }

    if (!skip) {
      color.setAlpha(alpha);
      fmt.setBackground(color);
      //fmt.setFontWeight(weight);
      fmt.setFontPointSize(sz);

      DOUT("start =" << start << ", len =" << surface.size());
      setFormat(start, surface.size(), fmt);
    }
    start += surface.size();
    word = word.nextSiblingElement("word");
  }
  DOUT("exit");
}

// EOF

  /*
  YahooJlp::Tagger *t = tagger();

  //QByteArray ba = toShiftJIS(text);
  QByteArray ba = text.toUtf8();
  const char *input = ba;

  QTextCharFormat fmt;
  QColor color;
  int alpha;
  int sz;

  // Gets Node object.
  int i = 0, j = 0;
  for (const YahooJlp::Node *node = t->parseToNode(input);
       node; node = node->next) {
    switch (node->stat) {
    case MECAB_BOS_NODE: // begin of sentence
    case MECAB_EOS_NODE: // end of sentence
      continue;
    }
    DOUT(node->char_type << node->lcAttr << node->rcAttr << QByteArray(node->surface, node->length) << node->feature);
    switch (node->char_type) {
    case CT_Noun:
    case CT_Verb:
    case CT_Katagana:
      sz = 14;
      alpha = 40;
      color = ++i%2 ? Qt::green : Qt::magenta;
      break;
    case CT_Modifier:
      sz = 10;
      alpha = 20;
      color = ++j%2 ? Qt::red : Qt::cyan;
      break;
    case CT_Punct:
    case CT_Latin:
    default: continue;
    }
    color.setAlpha(alpha);
    fmt.setBackground(color);
    //fmt.setFontWeight(weight);
    fmt.setFontPointSize(sz);

    //int start = (node->surface - input) / u8sz,
    //    len = node->length / u8sz;
    int start = QString::fromUtf8(input, node->surface - input).size(),
        len = QString::fromUtf8(node->surface, node->length).size();
    setFormat(start, len, fmt);
  }
  */
