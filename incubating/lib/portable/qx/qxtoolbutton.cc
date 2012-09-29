// qxtoolbutton.cc
// 8/6/2011

#include "qx/qxtoolbutton.h"

QX_BEGIN_NAMESPACE

// - Events -

void
QxToolButton::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QxToolButton::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

QX_END_NAMESPACE
