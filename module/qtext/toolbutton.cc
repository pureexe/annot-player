// qtext/toolbutton.cc
// 8/6/2011

#include "module/qtext/toolbutton.h"

// - Events -

void
QtExt::
ToolButton::enterEvent(QEvent *event)
{
  lastCursor_ = cursor();
  setCursor(hoverCursor());
  hovered_ = true;
  emit hovered();
  Base::enterEvent(event);
}

void
QtExt::
ToolButton::leaveEvent(QEvent *event)
{
  setCursor(lastCursor_);
  hovered_ = false;
  emit leaved();
  Base::leaveEvent(event);
}

// EOF
