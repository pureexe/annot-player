// qtext/slider.cc
// 8/6/2011

#include "module/qtext/slider.h"
#include <QtGui/QMouseEvent>

// - Events -

void
QtExt::
Slider::enterEvent(QEvent *event)
{
  hovered_ = true;
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QtExt::
Slider::leaveEvent(QEvent *event)
{
  hovered_ = false;
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

// Click'n'Go instead of move steps
// See: http://lists.trolltech.com/qt-interest/2006-11/msg00363.html
void
QtExt::
Slider::mousePressEvent(QMouseEvent *event)
{
  if (event && event->button() == Qt::LeftButton) {
    if (orientation() == Qt::Vertical)
      setValue(minimum() + ((maximum()-minimum()) * (height()-event->y())) / height() ) ;
    else
      setValue(minimum() + ((maximum()-minimum()) * event->x()) / width() ) ;
  }
  Base::mousePressEvent(event);
}

// EOF
