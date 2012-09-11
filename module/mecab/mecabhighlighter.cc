// mecabhighlighter.cc
// 9/3/2012
// See: mecab/sdk

#include "module/mecab/mecabhighlighter.h"
#include <QtGui/QTextCharFormat>

#ifdef Q_OS_WIN
# include <qt_windows.h> // override <windows.h> in mecab.h
#endif // Q_OS_WIN
#include <mecab.h>

//#define DEBUG "mecabhighlighter"
#include "module/debug/debug.h"
//#include <QtCore/QDebug>

//enum { u8sz = 3 }; // bytes of single utf8 char
enum { CT_Verb = 2, CT_Noun = 8, CT_Modifier = 6, CT_Punct = 3, CT_Katagana = 7, CT_Latin = 5 };

// - Properties -

MeCabHighlighter::~MeCabHighlighter()
{
  if (tagger_)
    delete tagger_;
}

MeCab::Tagger*
MeCabHighlighter::tagger() const
{
  if (!tagger_)
    tagger_ = MeCab::createTagger("");

  Q_ASSERT(tagger_);
  return tagger_;
}

// - Action -

//QByteArray
//MeCabHighlighter::toShiftJIS(const QString &text) const
//{
//  if (!encoder_)
//    encoder_ = QTextCodec::codecForName("SHIFT-JIS")->makeEncoder();
//  return encoder_->fromUnicode(text);
//}

void
MeCabHighlighter::highlightBlock(const QString &text)
{
  if (text.isEmpty())
    return;
  DOUT("enter: text =" << text);

  MeCab::Tagger *t = tagger();

  //QByteArray ba = toShiftJIS(text);
  QByteArray ba = text.toUtf8();
  const char *input = ba;

  QTextCharFormat fmt;
  QColor color;
  int alpha;
  int sz;

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
    switch (node->char_type) {
    case CT_Noun:
    case CT_Verb:
    case CT_Katagana:
      sz = 14;
      alpha = 40;
      color = ++i%2 ? Qt::red : Qt::cyan;
      break;
    case CT_Modifier:
      sz = 10;
      alpha = 20;
      color = ++j%2 ? Qt::green : Qt::magenta;
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
  DOUT("exit");
}

// EOF
