// qtext/toolbutton.cc
// 8/6/2011

#include "lib/qtext/toolbutton.h"

// - Events -

void
QtExt::
ToolButton::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QtExt::
ToolButton::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

// EOF
