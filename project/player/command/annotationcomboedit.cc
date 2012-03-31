// annotationcomboedit.cc
// 7/16/2011

#include "annotationcomboedit.h"
#include "annotationeditor.h"
#include "rc.h"
#include "tr.h"
#include <QtGui>

#define DEBUG "annotationcomboedit"
#include "module/debug/debug.h"

// - Constructions -

AnnotationComboEdit::AnnotationComboEdit(QWidget *parent)
  : Base(parent), editor_(0)
{ createActions(); }

void
AnnotationComboEdit::createActions()
{
  editAct = new QAction(this); {
    editAct->setText(TR(T_MENUTEXT_EDIT));
    editAct->setToolTip(TR(T_TOOLTIP_EDIT));
    connect(editAct, SIGNAL(triggered()), SLOT(edit()));
  }
}

// - Properties -

void
AnnotationComboEdit::edit()
{
  if (!editor_) {
    editor_ = new AnnotationEditor(this);
    connect(editor_, SIGNAL(textSaved(QString)), SLOT(setEditText(QString)));
  }
  editor_->setText(currentText());
  editor_->show();
}

// - Events -

void
AnnotationComboEdit::contextMenuEvent(QContextMenuEvent *event)
{
  if (!event)
    return;

  contextMenu->clear();

  if (!defaultItems.isEmpty())
    contextMenu->addAction(popupAct);
  contextMenu->addAction(editAct);
  contextMenu->addAction(clearAct);
  contextMenu->addSeparator();

  QMenu *scm = lineEdit()->createStandardContextMenu();
  contextMenu->addActions(scm->actions());

  contextMenu->exec(event->globalPos());
  delete scm;
  event->accept();
}

// EOF
