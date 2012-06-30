// accomboedit.cc
// 7/16/2011

#include "project/common/accomboedit.h"
#include "project/common/accomboedit_p.h"
#include "project/common/acui.h"
#include "project/common/acss.h"
#include <QtGui>

// - Construction -

void
AcComboEdit::init()
{
  contextMenuFlags_ = PopupAction | ClearAction;

  setLineEdit(new AcComboBoxLineEdit);
  connect(lineEdit(), SIGNAL(returnPressed()), SLOT(hidePopup()));

  createActions();

  setStyleSheet(ACSS_COMBOBOX);

  if (!defaultItems.isEmpty())
    reset();

  if (completer())
    completer()->setCompletionMode(QCompleter::PopupCompletion);

  setEditable(true);
}

void
AcComboEdit::createActions()
{
  connect(popupAct = new QAction(tr("History"), this),
          SIGNAL(triggered()), SLOT(showPopup()));
  connect(clearAct = new QAction(tr("Clear All"), this),
          SIGNAL(triggered()), SLOT(reset()));
  connect(pasteAndGoAct = new QAction(tr("Paste and Go"), this),
          SIGNAL(triggered()), SLOT(pasteAndGo()));
}

// - Actions -

void
AcComboEdit::pasteAndGo()
{
  QClipboard *c = QApplication::clipboard();
  if (c) {
    QString text = c->text().trimmed();
    if (!text.isEmpty()) {
      insertItem(0, text);
      qobject_cast<AcComboBoxLineEdit *>(lineEdit())->enterText(text);
      setEditText(text);
    }
  }
}

bool
AcComboEdit::isClipboardEmpty()
{
  QClipboard *c = QApplication::clipboard();
  return !c || c->text().trimmed().isEmpty();
}

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
  Q_ASSERT(event);
  if (!contextMenuFlags_) {
    Base::contextMenuEvent(event);
    return;
  }

  // Create menus
  QMenu *m = new QMenu(this);
  AcUi::globalInstance()->setContextMenuStyle(m, false); // persistent = false

  if (contextMenuFlags_ & PasteAndGoAction) {
    pasteAndGoAct->setEnabled(!isClipboardEmpty());
    m->addAction(pasteAndGoAct);
  }

  if ((contextMenuFlags_ & PopupAction) && count())
    m->addAction(popupAct);

  if (contextMenuFlags_ & ClearAction)
    m->addAction(clearAct);

  m->addSeparator();

  QMenu *scm = lineEdit()->createStandardContextMenu();
  m->addActions(scm->actions());

  m->exec(event->globalPos());
  delete scm;
  delete m;
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

