// qtext/checkbox.cc
// 5/30/2011

#include "lib/qtext/checkbox.h"

// - Events -

void
QtExt::
CheckBox::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QtExt::
CheckBox::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

// EOF
