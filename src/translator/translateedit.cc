// translateedit.cc
// 8/13/2012

#include "translateedit.h"
#include "src/common/acss.h"
#include "lib/qtext/texthighlighter.h"
#include <QtGui/QAction>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>

//#define DEBUG "translateedit"
#include "lib/debug/debug.h"

// - Constructions -

TranslateEdit::TranslateEdit(QWidget *parent)
  : Base(parent)
{
  //setWindowTitle(tr("Translations"));
  //setToolTip(tr("Translations"));

  setStyleSheet(ACSS_TEXTEDIT);

  highlighter_ = new QtExt::TextHighlighter(this);

  translateAct_ = new QAction(tr("Translate"), this);
  connect(translateAct_, SIGNAL(triggered()), SLOT(invalidateSelectedText()));

  clearAct_ = new QAction(tr("Clear All"), this);
  connect(clearAct_, SIGNAL(triggered()), SLOT(clear()));

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

void
TranslateEdit::contextMenuEvent(QContextMenuEvent *e)
{
  QMenu *m = createStandardContextMenu();
  m->addSeparator();
  m->addAction(clearAct_);
  m->addAction(translateAct_);
  m->exec(e->globalPos());
  e->accept();
}


// EOF
