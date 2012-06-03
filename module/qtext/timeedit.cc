// qtext/timeedit.cc
// 5/1/2012

#include "module/qtext/timeedit.h"
#include <QtGui/QKeyEvent>

// - Events -

void
QtExt::
TimeEdit::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QtExt::
TimeEdit::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

void
QtExt::
TimeEdit::keyPressEvent(QKeyEvent *event)
{
  Base::keyPressEvent(event);
  if (event->key() == Qt::Key_Return)
    emit returnPressed();
}

// EOF
