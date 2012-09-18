// qxradiobutton.cc
// 5/30/2011

#include "qtx/qxradiobutton.h"

QTX_BEGIN_NAMESPACE

// - Events -

void
QxRadioButton::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QxRadioButton::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

QTX_END_NAMESPACE
