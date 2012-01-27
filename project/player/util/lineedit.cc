// lineedit.cc
// 10/10/2011

#include "lineedit.h"
#include "uistyle.h"
#include "stylesheet.h"
#include <QtGui>

// - Constructions -

LineEdit::LineEdit(QWidget *parent)
  : Base(parent)
{
  contextMenu_ = new QMenu(this);
  UiStyle::globalInstance()->setWindowStyle(contextMenu_);
  setStyleSheet(SS_LINEEDIT);
}

// - Events -

void
LineEdit::contextMenuEvent(QContextMenuEvent *event)
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
