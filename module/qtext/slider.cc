// qtext/slider.cc
// 8/6/2011

#include "module/qtext/slider.h"
#include <QtGui/QMouseEvent>

// - Events -

void
QtExt::
Slider::enterEvent(QEvent *event)
{
  lastCursor_ = cursor();
  setCursor(hoverCursor());
  hovered_ = true;
  emit hovered();
  Base::enterEvent(event);
}

void
QtExt::
Slider::leaveEvent(QEvent *event)
{
  setCursor(lastCursor_);
  hovered_ = false;
  emit leaved();
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
