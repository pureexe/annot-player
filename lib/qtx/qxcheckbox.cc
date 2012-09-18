// qxcheckbox.cc
// 5/30/2011

#include "qtx/qxcheckbox.h"

QTX_BEGIN_NAMESPACE

// - Events -

void
QxCheckBox::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QxCheckBox::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

QTX_END_NAMESPACE
