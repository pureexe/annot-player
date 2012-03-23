// textedit.cc
// 10/11/2011

#include "textedit.h"
#include "uistyle.h"
#include "stylesheet.h"
#include <QtGui>

// - Constructions -

TextEdit::TextEdit(QWidget *parent)
  : Base(parent)
{
  contextMenu_ = new QMenu(this);
  UiStyle::globalInstance()->setWindowStyle(contextMenu_);
  setStyleSheet(SS_TEXTEDIT);
}

// - Events -

void
TextEdit::contextMenuEvent(QContextMenuEvent *event)
{
  if (event) {
    contextMenu_->clear();

    QMenu *scm = createStandardContextMenu();
    contextMenu_->addActions(scm->actions());

    contextMenu_->exec(event->globalPos());
    delete scm;
  }
}

bool
TextEdit::containsPlainText(const QString &text) const
{ return toPlainText().contains(text); }

bool
TextEdit::containsHtml(const QString &text) const
{ return toHtml().contains(text); }

// EOF
