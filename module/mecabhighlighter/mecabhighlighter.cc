// mecabhighlighter.cc
// 9/3/2012
// See: mecab/sdk

#include "module/mecabhighlighter/mecabhighlighter.h"
#include <mecab.h>
#include <QtGui/QTextCharFormat>

//#define DEBUG "mecabhighlighter"
#include "module/debug/debug.h"
//#include <QtCore/QDebug>

enum { u8sz = 3 }; // bytes of single utf8 char

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
    enum { CT_Subject = 2, CT_Modifier = 6 };
    switch (node->char_type) {
    case CT_Subject: sz = 14; alpha = 40; color = ++i%2 ? Qt::green : Qt::magenta; break; // noun or verb
    case CT_Modifier: sz = 10; alpha = 20; color = ++j%2 ? Qt::red : Qt::cyan; break; // adj or adv
    default: continue;
    }
    color.setAlpha(alpha);
    fmt.setBackground(color);
    //fmt.setFontWeight(weight);
    fmt.setFontPointSize(sz);
    setFormat((node->surface - input) / u8sz, node->length / u8sz, fmt);
  }
  DOUT("exit");
}

// EOF
