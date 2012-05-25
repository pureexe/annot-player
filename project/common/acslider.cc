// acslider.cc
// 5/16/2012

#include "project/common/acslider.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QVariant>

// - Events -

void
AcSlider::enterEvent(QEvent *event)
{
  hovered_ = true;
  qApp->setProperty("cursor", Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
AcSlider::leaveEvent(QEvent *event)
{
  hovered_ = false;
  qApp->setProperty("cursor", Qt::ArrowCursor);
  Base::leaveEvent(event);
}

// EOF
