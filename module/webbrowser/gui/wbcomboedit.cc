// wbcomboedit.cc
// 3/31/2012

#include "wbcomboedit.h"
#include "wblineedit.h"
#include "wbss.h"
#include <QtGui>

// - Construction -

void
WbComboEdit::init()
{
  setLineEdit(new WbLineEdit);

  createActions();

  setStyleSheet(SS_COMBOBOX);

  if (!defaultItems_.isEmpty())
    reset();

  if (completer())
    completer()->setCompletionMode(QCompleter::PopupCompletion);

  setEditable(true);
}

void
WbComboEdit::createActions()
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
}

// - Actions -

void
WbComboEdit::reset()
{
  clear();
  addItems(defaultItems_);
  clearEditText();
}

// - Events -

void
WbComboEdit::contextMenuEvent(QContextMenuEvent *event)
{
  Q_ASSERT(event);
  QMenu m;

  m.addAction(popupAct);
  m.addAction(clearAct);
  m.addSeparator();

  popupAct->setEnabled(count());

  QMenu *scm = lineEdit()->createStandardContextMenu();
  m.addActions(scm->actions());

  m.exec(event->globalPos());
  delete scm;
  event->accept();
}

// EOF

/*
void
WbComboEdit::keyPressEvent(QKeyEvent *event)
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
