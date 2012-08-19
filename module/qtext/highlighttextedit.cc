// highlighttextedit.cc
// 8/14/2012

#include "module/qtext/highlighttextedit.h"
#include "module/qtext/texthighlighter.h"

enum { HIGHLIGHT_SIZE = 2 };

// - Construction -

void
QtExt::
HighlightTextEdit::init()
{
  highlighter_ = new TextHighlighter(this);
  connect(this, SIGNAL(selectionChanged()), SLOT(updateSelectedText()));
}

// - Actions -

void
QtExt::
HighlightTextEdit::highlightText(const QString &t)
{ highlighter_->setHighlightText(t); }

void
QtExt::
HighlightTextEdit::updateSelectedText()
{
  QString t = textCursor().selectedText().trimmed();
  if (selectedText_ != t) {
    highlightText(selectedText_.size() >= HIGHLIGHT_SIZE ? selectedText_ : QString());
    emit selectedTextChanged(selectedText_ = t);
  }
}

// EOF
