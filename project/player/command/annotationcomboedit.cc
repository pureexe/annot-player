// annotationcomboedit.cc
// 7/16/2011

#include "annotationcomboedit.h"
#include "annotationeditor.h"
#include "stylesheet.h"
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
#define MAKE_ACTION(_action, _styleid, _slot) \
  _action = new QAction(QIcon(RC_IMAGE_##_styleid), TR(T_MENUTEXT_##_styleid), this); \
  _action->setToolTip(TR(T_TOOLTIP_##_styleid)); \
  connect(_action, SIGNAL(triggered()), _slot);

  MAKE_ACTION(editAct, EDIT, SLOT(edit()))

#undef MAKE_ACTION
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
