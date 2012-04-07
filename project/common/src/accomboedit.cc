// accomboedit.cc
// 7/16/2011

#include "ac/accomboedit.h"
#include "ac/acui.h"
#include "ac/acss.h"
#include "accomboeditprivate.h"
#include <QtGui>

// - Construction -

void
AcComboEdit::init()
{
  setLineEdit(new AcComboBoxLineEdit);

  createActions();

  setStyleSheet(SS_COMBOBOX);

  if (!defaultItems.isEmpty())
    reset();

  if (completer())
    completer()->setCompletionMode(QCompleter::PopupCompletion);

  setEditable(true);
}

void
AcComboEdit::createActions()
{
  popupAct = new QAction(this); {
    popupAct->setText(tr("History"));
    popupAct->setToolTip(tr("History"));
    connect(popupAct, SIGNAL(triggered()), SLOT(popup()));
  }
  clearAct = new QAction(this); {
    clearAct->setText(tr("Clear"));
    clearAct->setToolTip(tr("Clear"));
    connect(clearAct, SIGNAL(triggered()), SLOT(reset()));
  }

  // Create menus
  contextMenu = new QMenu(this);
  AcUi::globalInstance()->setContextMenuStyle(contextMenu, true); // persistent = true
}

// - Actions -

void
AcComboEdit::reset()
{
  clear();
  addItems(defaultItems);
  clearEditText();
}

// - Events -

void
AcComboEdit::contextMenuEvent(QContextMenuEvent *event)
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
AcComboEdit::keyPressEvent(QKeyEvent *event)
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

