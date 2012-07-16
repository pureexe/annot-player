// gui/wbcomboedit.cc
// 3/31/2012

#include "gui/wbcomboedit.h"
#include "gui/wblineedit.h"
#include "global/wbss.h"
#include <QtGui>

// - Construction -

void
WbComboEdit::init()
{
  setLineEdit(new WbLineEdit);
  connect(lineEdit(), SIGNAL(returnPressed()), SLOT(hidePopup()));
  connect(lineEdit(), SIGNAL(returnPressed()), SLOT(submitText()));

  createActions();
  clearIcon();

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
    popupAct->setStatusTip(tr("History"));
  } connect(popupAct, SIGNAL(triggered()), SLOT(showPopup()));
  clearAct = new QAction(this); {
    clearAct->setText(tr("Clear History"));
    clearAct->setStatusTip(tr("Clear History"));
  } connect(clearAct, SIGNAL(triggered()), SLOT(reset()));
  pasteAndGoAct = new QAction(this); {
    pasteAndGoAct->setText(tr("Paste and Go"));
    pasteAndGoAct->setStatusTip(tr("Paste and Go"));
  } connect(pasteAndGoAct, SIGNAL(triggered()), SLOT(pasteAndGo()));

  submitAct = new QAction(this); {
    submitAct->setText(tr("Submit"));
    submitAct->setStatusTip(tr("Submit"));
  } connect(submitAct, SIGNAL(triggered()), SLOT(submitText()));
}

void
WbComboEdit::setIcon(const QString &url)
{
  QString ss = url.isEmpty() ? SS_COMBOBOX_NOICON
                             : SS_COMBOBOX_ICON(+url+);
  setStyleSheet(ss);
  emit styleSheetChanged();
}

// - Actions -

void
WbComboEdit::reset()
{
  clear();
  if (defaultIcons_.isEmpty())
    addItems(defaultItems_);
  else
    for (int i = 0; i < defaultItems_.size(); i++)
      if (i < defaultItems_.size())
        addItem(QIcon(defaultIcons_[i]), defaultItems_[i]);
      else
        addItem(defaultItems_[i]);
  clearEditText();
}

// - Events -

void
WbComboEdit::contextMenuEvent(QContextMenuEvent *event)
{
  Q_ASSERT(event);
  QMenu *m = new QMenu(this);

  m->addAction(submitAct);
  m->addAction(pasteAndGoAct);
  m->addAction(popupAct);
  m->addAction(clearAct);
  m->addSeparator();

  submitAct->setEnabled(!currentText().trimmed().isEmpty());
  pasteAndGoAct->setEnabled(!isClipboardEmpty());
  popupAct->setEnabled(count());

  QMenu *scm = lineEdit()->createStandardContextMenu();
  m->addActions(scm->actions());

  m->exec(event->globalPos());
  delete scm;
  delete m;
  event->accept();
}

bool
WbComboEdit::isClipboardEmpty()
{
  QClipboard *c = QApplication::clipboard();
  return !c || c->text().trimmed().isEmpty();
}

// - Actions -

void
WbComboEdit::pasteAndGo()
{
  QClipboard *c = QApplication::clipboard();
  if (c) {
    QString t = c->text().trimmed();
    if (!t.isEmpty())
      emit textEntered(t);
  }
}

void
WbComboEdit::submitText()
{
  QString t = currentText().trimmed();
  if (!t.isEmpty())
    emit textEntered(t);
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
