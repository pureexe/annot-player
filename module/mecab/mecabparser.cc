// mecabparser.cc
// 9/3/2012
//
// See: mecab/sdk
// See: http://qt-project.org/doc/qt-4.8/richtext-html-subset.html

#include "module/mecab/mecabparser.h"
#include "module/annotcloud/annottag.h"
#include "module/qtext/htmltag.h"
#ifdef WITH_MODULE_TEXTCODEC
# include "module/textcodec/textcodec.h"
#else
# error "require module textcodec"
#endif // WITH_MODULE_TEXTCODEC
#include <QtGui/QColor>

#ifdef Q_OS_WIN
# include <qt_windows.h> // override <windows.h> in mecab.h
#endif // Q_OS_WIN
#include <mecab.h>

//#define DEBUG "mecabparser"
#include "module/debug/debug.h"
#include <QtCore/QDebug>

//enum { u8sz = 3 }; // bytes of single utf8 char
enum { CT_Verb = 2, CT_Noun = 8, CT_Modifier = 6, CT_Punct = 3, CT_Katagana = 7, CT_Latin = 5 };

// - Properties -

MeCabParser::~MeCabParser()
{
  if (tagger_)
    delete tagger_;
}

MeCab::Tagger*
MeCabParser::tagger() const
{
  if (!tagger_)
    tagger_ = MeCab::createTagger("");

  Q_ASSERT(tagger_);
  return tagger_;
}

// - Action -

QString
MeCabParser::renderText(const QString &text, ulong hints)
{
  QString ret;
  if (text.isEmpty())
    return ret;
  DOUT("enter: hints =" << hints << ", text =" << text);

  MeCab::Tagger *t = tagger();

  //QByteArray ba = toShiftJIS(text);
  QByteArray ba = text.toUtf8();
  const char *input = ba;

  QString yomigana;

  // Gets Node object.
  int i = 0, j = 0;
  for (const MeCab::Node *node = t->parseToNode(input);
       node; node = node->next) {
    switch (node->stat) {
    case MECAB_BOS_NODE: // begin of sentence
    case MECAB_EOS_NODE: // end of sentence
      continue;
    }
    DOUT(node->char_type << node->lcAttr << node->rcAttr << QByteArray(node->surface, node->length) << node->feature);

    QString t = QString::fromUtf8(node->surface, node->length);
    //QString t = text.mid((node->surface - input) / u8sz, node->length / u8sz);

    QString fmt;
    QColor color;
    bool showFurigana = false;
    switch (node->char_type) {
    case CT_Verb:
    case CT_Noun:
      showFurigana = true;
    case CT_Katagana:
      if (hints & ResizeHint)
        fmt.append(CORE_CMD_LATEX_BIG);
      if (hints & ColorHint) {
        color = ++i%2 ? Qt::red : Qt::cyan;
        color.setAlpha(80);
      }
      break;
    case CT_Modifier:  // adj or adv
      if (hints & ColorHint) {
        color = ++j%2 ? Qt::green : Qt::magenta;
        color.setAlpha(60);
      }
      //showFurigana = true;
      break;
    case CT_Punct:
    case CT_Latin:
    default:
      if (!t.isEmpty()) {
        yomigana.append(HTML_TAG(td));
        ret.append(
          HTML_TD_OPEN(align=center)
            CORE_CMD_COLOR_BACKGROUND "[rgba(0,0,0,20)]" "{" + t + "}"
          HTML_TD_CLOSE()
        );
        if (node->char_type != CT_Punct)
          qDebug() << "mecabparser:renderTextWithFurigana: unknown char_type =" << node->char_type << ", text =" << t;
      } continue;
    }

    QString furigana;
    if (showFurigana) {
      QString feature = QString::fromUtf8(node->feature); {
        int index = feature.lastIndexOf(',');
        if (index > 0) {
          QString katagana = feature.mid(index +1);
          QString hiragana = TextCodec::kata2hira(katagana);
          if (hiragana != t)
            furigana = hiragana;
        }
      }
    }

    yomigana.append(furigana.isEmpty() ? HTML_TAG(td) :
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
        "{" + fmt + t + "}"
      HTML_TD_CLOSE()
    );
  }

  ret =
    HTML_CENTER_OPEN()
    HTML_TABLE_OPEN()
      HTML_TR_OPEN(valign=bottom) + yomigana + HTML_TR_CLOSE()
      HTML_TR_OPEN(valign=bottom) + ret + HTML_TR_CLOSE()
    HTML_TABLE_CLOSE()
    HTML_CENTER_CLOSE();
  DOUT("exit:ret =" << ret);
  return ret;
}

// EOF

/*
QString
MeCabParser::renderText(const QString &text)
{
  QString ret;
  if (text.isEmpty())
    return ret;
  DOUT("enter: text =" << text);

  MeCab::Tagger *t = tagger();

  //QByteArray ba = toShiftJIS(text);
  QByteArray ba = text.toUtf8();
  const char *input = ba;


  // Gets Node object.
  int i = 0, j = 0;
  for (const MeCab::Node *node = t->parseToNode(input);
       node; node = node->next) {
    switch (node->stat) {
    case MECAB_BOS_NODE: // begin of sentence
    case MECAB_EOS_NODE: // end of sentence
      continue;
    }
    DOUT(node->char_type << node->lcAttr << node->rcAttr << QByteArray(node->surface, node->length) << node->feature);

    QString fmt;
    QColor color;
    int alpha;
    switch (node->char_type) {
    case CT_Verb:
    case CT_Noun:
      fmt.append(CORE_CMD_LATEX_BIG);
      alpha = 80; color = ++i%2 ? Qt::green : Qt::magenta;
      break;
    case CT_Modifier:  // adj or adv
      alpha = 60; color = ++j%2 ? Qt::red : Qt::cyan;
      break;
    case CT_Punct:
    default: continue;
    }

    QString t = QString::fromUtf8(node->surface, node->length);
    //QString t = text.mid((node->surface - input) / u8sz, node->length / u8sz);

    //fmt.setBackground(color);
    //fmt.setFontPointSize(sz);
    fmt.append(
        "\\background["
        +
        QString("rgba(%1,%2,%3,%4)")
        .arg(QString::number(color.red()))
        .arg(QString::number(color.green()))
        .arg(QString::number(color.blue()))
        .arg(QString::number(alpha))
        + "]")
    ;
    ret.append('{').append(fmt).append(t).append('}');
  }
  DOUT("exit:ret =" << ret);
  return ret;
}
*/
