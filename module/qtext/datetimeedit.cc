// qtext/datetimeedit.cc
// 5/1/2012

#include "module/qtext/datetimeedit.h"
#include <QtGui/QKeyEvent>

// - Events -

void
QtExt::
DateTimeEdit::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QtExt::
DateTimeEdit::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

void
QtExt::
DateTimeEdit::keyPressEvent(QKeyEvent *event)
{
  Base::keyPressEvent(event);
  if (event->key() == Qt::Key_Return)
    emit returnPressed();
}

// EOF
