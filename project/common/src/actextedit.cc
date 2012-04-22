// ac/actextedit.cc
// 10/11/2011

#include "ac/actextedit.h"
#include "ac/acui.h"
#include "ac/acss.h"
#include <QtGui/QMenu>
#include <QtGui/QContextMenuEvent>
#include <QtCore/QTimer>

// - Constructions -

AcTextEdit::AcTextEdit(QWidget *parent)
  : Base(parent)
{
  setStyleSheet(SS_TEXTEDIT);
}

// - Events -

void
AcTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
#ifdef WITH_WIN_DWM
  Q_ASSERT(event);
  QMenu *m = new QMenu(this);
  AcUi::globalInstance()->setWindowStyle(m, false); // resistent = false

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

bool
AcTextEdit::containsPlainText(const QString &text) const
{ return toPlainText().contains(text); }

bool
AcTextEdit::containsHtml(const QString &text) const
{ return toHtml().contains(text); }

// EOF
