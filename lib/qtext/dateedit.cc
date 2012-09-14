// qtext/dateedit.cc
// 5/1/2012

#include "lib/qtext/dateedit.h"
#include <QtGui/QKeyEvent>

// - Events -

void
QtExt::
DateEdit::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QtExt::
DateEdit::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

void
QtExt::
DateEdit::keyPressEvent(QKeyEvent *event)
{
  Base::keyPressEvent(event);
  if (event->key() == Qt::Key_Return)
    emit returnPressed();
}

// EOF
