// translatebrowser.cc
// 8/13/2012

#include "translatebrowser.h"
#include "project/common/acss.h"
#include "module/qtext/texthighlighter.h"

//#define DEBUG "translatebrowser"
#include "module/debug/debug.h"

// - Constructions -

TranslateBrowser::TranslateBrowser(QWidget *parent)
  : Base(parent)
{
  setStyleSheet(ACSS_TEXTBROWSER);
  setReadOnly(true);
  setOpenLinks(false);

  highlighter_ = new QtExt::TextHighlighter(this);

  connect(this, SIGNAL(selectionChanged()), SLOT(updateSelectedText()));
}

// - Actions -

void
TranslateBrowser::highlightText(const QString &t)
{ highlighter_->setHighlightText(t); }

void
TranslateBrowser::updateSelectedText()
{
  QString t = textCursor().selectedText().trimmed();
  if (selectedText_ != t)
    emit selectedTextChanged(selectedText_ = t);
}

// EOF
