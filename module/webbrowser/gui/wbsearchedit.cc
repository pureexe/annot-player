// wbsearchedit.cc
// 3/31/2012

#include "wbsearchedit.h"
#include "wbss.h"
#include <QtGui>

// - Construction -

void
WbSearchEdit::createActions()
{
  pasteAndGoAct = new QAction(this); {
    pasteAndGoAct->setText(tr("Paste and go"));
    pasteAndGoAct->setStatusTip(tr("Paste and go"));
    connect(pasteAndGoAct, SIGNAL(triggered()), SLOT(pasteAndGo()));
  }
}

// - Actions -

void
WbSearchEdit::pasteAndGo()
{
  QClipboard *c = QApplication::clipboard();
  if (c) {
    QString url = c->text().trimmed();
    if (!url.isEmpty())
      emit textEntered(url);
  }
}

bool
WbSearchEdit::isClipboardEmpty()
{
  QClipboard *c = QApplication::clipboard();
  return !c || c->text().trimmed().isEmpty();
}

// - Events -

void
WbSearchEdit::contextMenuEvent(QContextMenuEvent *event)
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
