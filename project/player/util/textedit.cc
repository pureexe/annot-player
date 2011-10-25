// textedit.cc
// 10/11/2011

#include "textedit.h"
#include "uistyle.h"
#include <QtGui>

// - Constructions -

TextEdit::TextEdit(QWidget *parent)
  : Base(parent)
{
  contextMenu_ = new QMenu(this);
  UiStyle::globalInstance()->setWindowStyle(contextMenu_);
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

// EOF
