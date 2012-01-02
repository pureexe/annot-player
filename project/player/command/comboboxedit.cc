// comboboxedit.cc
// 7/16/2011

#include "comboboxedit.h"
#include "comboboxeditprivate.h"
#include "annotationeditor.h"
#include "uistyle.h"
#include "rc.h"
#include "tr.h"
#include "annotationeditor.h"
#include <QtGui>

#define DEBUG "comboboxedit"
#include "module/debug/debug.h"

// - Constructions -

ComboBoxEdit::ComboBoxEdit(QWidget *parent)
  : Base(parent), editor_(0)
{
  setLineEdit(new ComboBoxLineEdit_);

  createActions();

  setEditable(true);
}

void
ComboBoxEdit::createActions()
{
#define MAKE_ACTION(_action, _styleid, _slot) \
  _action = new QAction(QIcon(RC_IMAGE_##_styleid), TR(T_MENUTEXT_##_styleid), this); \
  _action->setToolTip(TR(T_TOOLTIP_##_styleid)); \
  connect(_action, SIGNAL(triggered()), _slot);

  MAKE_ACTION(editAct_, EDIT, SLOT(edit()))
  MAKE_ACTION(clearAct_, CLEAR, SLOT(reset()))

#undef MAKE_ACTION

  // Create menus
  contextMenu_ = new QMenu(TR(T_TITLE_ANNOTATIONBROWSER), this);
  UiStyle::globalInstance()->setContextMenuStyle(contextMenu_, true); // persistent = true
}


// - Properties -

void
ComboBoxEdit::edit()
{
  if (!editor_) {
    editor_ = new AnnotationEditor(this);
    connect(editor_, SIGNAL(textSaved(QString)), SLOT(setEditText(QString)));
  }
  editor_->setText(currentText());
  editor_->show();
}

// - Actions -

void
ComboBoxEdit::reset()
{
  clear();
  addItems(defaultItems_);
  clearEditText();
}

// - Events -

void
ComboBoxEdit::contextMenuEvent(QContextMenuEvent *event)
{
  if (!event)
    return;

  contextMenu_->clear();

  contextMenu_->addAction(editAct_);
  contextMenu_->addAction(clearAct_);
  contextMenu_->addSeparator();

  QMenu *scm = lineEdit()->createStandardContextMenu();
  contextMenu_->addActions(scm->actions());

  contextMenu_->exec(event->globalPos());
  delete scm;
  event->accept();
}

// EOF

/*
void
ComboBoxEdit::keyPressEvent(QKeyEvent *event)
{
  Q_ASSERT(event);
  // Do not pass escape key to parent.
  switch (event->key()) {
  case Qt::Key_Escape:
    DOUT("Key_Escape");
    clearFocus();    // FIXME: after clear focus, which window get new focus?
    event->accept();
    return;

  case Qt::Key_Return:
    DOUT("Key_Return");
    event->accept();
    //emit returnPressed();
    return;

  case Qt::Key_Up:
    DOUT("Key_Up");
    //previous();
    event->accept();
    return;

  case Qt::Key_Down:
    DOUT("Key_Down");
    //next();
    event->accept();
    return;
  }
  Base::keyPressEvent(event);
}
*/

