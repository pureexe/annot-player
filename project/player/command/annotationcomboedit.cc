// annotationcomboedit.cc
// 7/16/2011

#include "annotationcomboedit.h"
#include "annotationeditor.h"
#include "rc.h"
#include "tr.h"
#include "ac/acui.h"
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
  Q_ASSERT(event);

  QMenu m;
  AcUi::globalInstance()->setContextMenuStyle(&m, false); // persistent = false

  if (contextMenuFlags() & PasteAndGoAction)
    m.addAction(pasteAndGoAct);
  if (count() && (contextMenuFlags() & PopupAction))
    m.addAction(popupAct);
  if (contextMenuFlags() & EditAction)
    m.addAction(editAct);
  if (contextMenuFlags() & ClearAction)
    m.addAction(clearAct);

  m.addSeparator();

  QMenu *scm = lineEdit()->createStandardContextMenu();
  m.addActions(scm->actions());

  m.exec(event->globalPos());
  delete scm;
  event->accept();
}

// EOF
