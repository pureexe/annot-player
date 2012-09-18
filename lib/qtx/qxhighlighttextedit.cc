// qxhighlighttextedit.cc
// 8/14/2012

#include "qtx/qxhighlighttextedit.h"
#include "qtx/qxtexthighlighter.h"

enum { HIGHLIGHT_SIZE = 2 };

QTX_BEGIN_NAMESPACE

// - Construction -

void
QxHighlightTextEdit::init()
{
  highlighter_ = new QxTextHighlighter(this);
  connect(this, SIGNAL(selectionChanged()), SLOT(updateSelectedText()));
}

// - Actions -

void
QxHighlightTextEdit::highlightText(const QString &t)
{ highlighter_->setHighlightText(t); }

void
QxHighlightTextEdit::updateSelectedText()
{
  QString t = textCursor().selectedText().trimmed();
  if (selectedText_ != t) {
    highlightText(selectedText_.size() >= HIGHLIGHT_SIZE ? selectedText_ : QString());
    emit selectedTextChanged(selectedText_ = t);
  }
}

QTX_END_NAMESPACE
