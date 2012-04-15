// aclineedit.cc
// 10/10/2011

#include "ac/aclineedit.h"
#include "ac/acui.h"
#include "ac/acss.h"
#include <QMenu>
#include <QContextMenuEvent>

// - Constructions -

AcLineEdit::AcLineEdit(QWidget *parent)
  : Base(parent)
{
  setStyleSheet(SS_LINEEDIT);
}

void
AcLineEdit::enterText(const QString &text)
{
  setText(text);
  emit returnPressed();
}

// - Events -

void
AcLineEdit::contextMenuEvent(QContextMenuEvent *event)
{
  Q_ASSERT(event);

  QMenu m;
  AcUi::globalInstance()->setContextMenuStyle(&m, false); // persistent = false

  QMenu *scm = createStandardContextMenu();
  m.addActions(scm->actions());

  m.exec(event->globalPos());
  delete scm;
}

// EOF
