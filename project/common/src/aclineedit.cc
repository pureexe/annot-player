// aclineedit.cc
// 10/10/2011

#include "ac/aclineedit.h"
#include "ac/acui.h"
#include "ac/acss.h"
#include <QtGui/QMenu>
#include <QtGui/QContextMenuEvent>
#include <QtCore/QTimer>

// - Constructions -

AcLineEdit::AcLineEdit(QWidget *parent)
  : Base(parent)
{ setStyleSheet(SS_LINEEDIT); }

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
#ifdef WITH_WIN_DWM
  Q_ASSERT(event);

  QMenu *m = new QMenu(this);
  AcUi::globalInstance()->setContextMenuStyle(m, false); // persistent = false

  QMenu *scm = createStandardContextMenu();
  m->addActions(scm->actions());

  m->exec(event->globalPos());
  delete scm;
  QTimer::singleShot(0, m, SLOT(deleteLater()));
  event->accept();
#else
  Base::contextMenuEvent(event);
#endif // WITH_WIN_DWM
}

// EOF
