// comboedit.cc
// 7/16/2011

#include "comboedit.h"
#include "comboeditprivate.h"
#include "uistyle.h"
#include "stylesheet.h"
#include "rc.h"
#include "tr.h"
#include <QtGui>

#define DEBUG "comboedit"
#include "module/debug/debug.h"

// - Constructions -

ComboEdit::ComboEdit(QWidget *parent)
  : Base(parent)
{ init(); }

ComboEdit::ComboEdit(const QStringList &items, QWidget *parent)
  : Base(parent), defaultItems(items)
{ init(); }

void
ComboEdit::init()
{
  setLineEdit(new ComboBoxLineEdit_);

  createActions();

  setStyleSheet(SS_COMBOBOX);

  if (!defaultItems.isEmpty())
    reset();

  setEditable(true);
}

void
ComboEdit::createActions()
{
#define MAKE_ACTION(_action, _styleid, _slot) \
  _action = new QAction(QIcon(RC_IMAGE_##_styleid), TR(T_MENUTEXT_##_styleid), this); \
  _action->setToolTip(TR(T_TOOLTIP_##_styleid)); \
  connect(_action, SIGNAL(triggered()), _slot);

  MAKE_ACTION(popupAct, HISTORY, SLOT(popup()))
  MAKE_ACTION(clearAct, CLEAR, SLOT(reset()))

#undef MAKE_ACTION

  // Create menus
  contextMenu = new QMenu(TR(T_TITLE_ANNOTATIONBROWSER), this);
  UiStyle::globalInstance()->setContextMenuStyle(contextMenu, true); // persistent = true
}

// - Actions -

void
ComboEdit::reset()
{
  clear();
  addItems(defaultItems);
  clearEditText();
}

// - Events -

void
ComboEdit::contextMenuEvent(QContextMenuEvent *event)
{
  if (!event)
    return;

  contextMenu->clear();

  if (count())
    contextMenu->addAction(popupAct);
  contextMenu->addAction(clearAct);
  contextMenu->addSeparator();

  QMenu *scm = lineEdit()->createStandardContextMenu();
  contextMenu->addActions(scm->actions());

  contextMenu->exec(event->globalPos());
  delete scm;
  event->accept();
}

// EOF

/*
void
ComboEdit::keyPressEvent(QKeyEvent *event)
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

