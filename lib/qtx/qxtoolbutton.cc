// qxtoolbutton.cc
// 8/6/2011

#include "qtx/qxtoolbutton.h"

QTX_BEGIN_NAMESPACE

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

QTX_END_NAMESPACE
