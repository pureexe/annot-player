// qxtexthighlighter.cc
// 8/14/2012

#include "qtx/qxtexthighlighter.h"
#include <QtGui/QColor>

#define HIGHLIGHT_COLOR QColor(255,255,0,224)  // yellow: 255,255,0

QTX_BEGIN_NAMESPACE

// - Construction -

void
QxTextHighlighter::init()
{
  highlightFormat_.setBackground(HIGHLIGHT_COLOR);
}

// - Properties -

void
QxTextHighlighter::setHighlightText(const QString &t)
{
  if (highlightText_ != t) {
    highlightText_ = t;
    rehighlight();
  }
}

// - Actions -

void
QxTextHighlighter::highlightBlock(const QString &text)
{
  if (highlightText_.isEmpty())
    return;

  int index = text.indexOf(highlightText_);
  int length = highlightText_.length();
  while (index >= 0) {
    setFormat(index, length, highlightFormat_);
    index = text.indexOf(highlightText_, index + length);
  }
}

QTX_END_NAMESPACE
