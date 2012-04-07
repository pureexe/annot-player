// ac/actextedit.cc
// 10/11/2011

#include "ac/actextedit.h"
#include "ac/acui.h"
#include "ac/acss.h"
#include <QMenu>
#include <QContextMenuEvent>

// - Constructions -

AcTextEdit::AcTextEdit(QWidget *parent)
  : Base(parent)
{
  contextMenu_ = new QMenu(this);
  AcUi::globalInstance()->setWindowStyle(contextMenu_);
  setStyleSheet(SS_TEXTEDIT);
}

// - Events -

void
AcTextEdit::contextMenuEvent(QContextMenuEvent *event)
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
AcTextEdit::containsPlainText(const QString &text) const
{ return toPlainText().contains(text); }

bool
AcTextEdit::containsHtml(const QString &text) const
{ return toHtml().contains(text); }

// EOF
