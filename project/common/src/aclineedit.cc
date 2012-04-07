// aclineedit.cc
// 10/10/2011

#include "ac/aclineedit.h"
#include "ac/acui.h"
#include "ac/acss.h"
#include <QMenu>
#include <QContextMenuEvent>

// - Constructions -

AcLineEdit::AcLineEdit(QWidget *parent)
  : Base(parent)
{
  contextMenu_ = new QMenu(this);
  AcUi::globalInstance()->setWindowStyle(contextMenu_);
  setStyleSheet(SS_LINEEDIT);
}

// - Events -

void
AcLineEdit::contextMenuEvent(QContextMenuEvent *event)
{
  if (event) {
    contextMenu_->clear();

    QMenu *scm = createStandardContextMenu();
    contextMenu_->addActions(scm->actions());

    contextMenu_->exec(event->globalPos());
    delete scm;
  }
}

// EOF
