// qtext/radiobutton.cc
// 5/30/2011

#include "lib/qtext/radiobutton.h"

// - Events -

void
QtExt::
RadioButton::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QtExt::
RadioButton::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

// EOF
