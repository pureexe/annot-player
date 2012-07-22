// annotationcomboedit.cc
// 7/16/2011

#include "annotationcomboedit.h"
#include "annotationeditor.h"
#include "rc.h"
#include "tr.h"
#include "project/common/acui.h"
#include <QtGui>

#define DEBUG "annotationcomboedit"
#include "module/debug/debug.h"

// - Constructions -

AnnotationComboEdit::AnnotationComboEdit(QWidget *parent)
  : Base(parent), editor_(nullptr)
{ createActions(); }

void
AnnotationComboEdit::createActions()
{
  connect(editAct = new QAction(TR(T_MENUTEXT_EDIT), this), SIGNAL(triggered()), SLOT(edit()));
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
AnnotationComboEdit::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Space && currentText().isEmpty()) {
    if (QObject *receiver = parent())
      QCoreApplication::sendEvent(receiver, event);
    return;
  }
  Base::keyPressEvent(event);
}

void
AnnotationComboEdit::contextMenuEvent(QContextMenuEvent *event)
{
  Q_ASSERT(event);

  QMenu *m = new QMenu;//(this); otherwise, rendered incorrect in OsdWindow
  AcUi::globalInstance()->setContextMenuStyle(m, false); // persistent = false

  if (contextMenuFlags() & PasteAndGoAction)
    m->addAction(pasteAndGoAct);
  if (count() && (contextMenuFlags() & PopupAction))
    m->addAction(popupAct);
  if (contextMenuFlags() & EditAction)
    m->addAction(editAct);
  if (contextMenuFlags() & ClearAction)
    m->addAction(clearAct);

  m->addSeparator();

  QMenu *scm = lineEdit()->createStandardContextMenu();
  m->addActions(scm->actions());

  m->exec(event->globalPos());
  event->accept();
  delete scm;
  delete m;
}

// EOF
