// aclineedit.cc
// 10/10/2011

#include "src/common/aclineedit.h"
#include "src/common/acss.h"

// - Constructions -

AcLineEdit::AcLineEdit(QWidget *parent)
  : Base(parent)
{ setStyleSheet(ACSS_LINEEDIT); }

void
AcLineEdit::enterText(const QString &text)
{
  setText(text);
  emit returnPressed();
}


// EOF

// - Events -
/*
void
AcLineEdit::contextMenuEvent(QContextMenuEvent *event)
{
#ifdef WITH_WIN_DWM
  Q_ASSERT(event);

  QMenu *m = new QMenu(this);
  AcUi::globalInstance()->setContextMenuStyle(m, false); // persistent = false

  QMenu *scm = createStandardContextMenu();
  m->addActions(scm->actions());

  m->exec(event->globalPos());
  delete scm;
  delete m;
  event->accept();
#else
  Base::contextMenuEvent(event);
#endif // WITH_WIN_DWM
}
*/
