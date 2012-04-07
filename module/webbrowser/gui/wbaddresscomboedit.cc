// wbaddresscomboedit.cc
// 3/31/2012

#include "wbaddresscomboedit.h"
#include "wbss.h"
#include <QtGui>

// - Construction -

void
WbAddressComboEdit::createActions()
{
  pasteAndGoAct = new QAction(this); {
    pasteAndGoAct->setText(tr("Paste and go"));
    pasteAndGoAct->setToolTip(tr("Paste and go"));
    connect(pasteAndGoAct, SIGNAL(triggered()), SLOT(pasteAndGo()));
  }
}

// - Actions -

void
WbAddressComboEdit::pasteAndGo()
{
  QClipboard *c = QApplication::clipboard();
  if (c) {
    QString url = c->text().trimmed();
    if (!url.isEmpty())
      emit visitAddressRequested(url);
  }
}

bool
WbAddressComboEdit::isClipboardEmpty()
{
  QClipboard *c = QApplication::clipboard();
  return !c || c->text().trimmed().isEmpty();
}

// - Events -

void
WbAddressComboEdit::contextMenuEvent(QContextMenuEvent *event)
{
  if (!event)
    return;

  QMenu m;

  m.addAction(pasteAndGoAct);
  m.addAction(popupAct);
  m.addAction(clearAct);
  m.addSeparator();

  pasteAndGoAct->setEnabled(!isClipboardEmpty());
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
WbAddressComboEdit::keyPressEvent(QKeyEvent *event)
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
