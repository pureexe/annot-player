// translateedit.cc
// 8/13/2012

#include "translateedit.h"
#include "project/common/acss.h"
#include "module/qtext/texthighlighter.h"

//#define DEBUG "translateedit"
#include "module/debug/debug.h"

// - Constructions -

TranslateEdit::TranslateEdit(QWidget *parent)
  : Base(parent)
{
  //setWindowTitle(tr("Translations"));
  //setToolTip(tr("Translations"));

  setStyleSheet(ACSS_TEXTEDIT);

  highlighter_ = new QtExt::TextHighlighter(this);

  connect(this, SIGNAL(selectionChanged()), SLOT(updateSelectedText()));
}

// - Actions -

void
TranslateEdit::highlightText(const QString &t)
{ highlighter_->setHighlightText(t); }

void
TranslateEdit::updateSelectedText()
{
  QString t = textCursor().selectedText().trimmed();
  if (selectedText_ != t)
    emit selectedTextChanged(selectedText_ = t);
}

// - Events -

//void
//TranslateEdit::contextMenuEvent(QContextMenuEvent *e)
//{
//}

// EOF
