// qtext/slider.cc
// 8/6/2011

#include "module/qtext/slider.h"
#include <QtGui/QMouseEvent>
#include <QtCore/QDebug>

// - Events -

void
QtExt::
Slider::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QtExt::
Slider::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

// Click'n'Go instead of move steps
// See: http://lists.trolltech.com/qt-interest/2006-11/msg00363.html
void
QtExt::
Slider::mousePressEvent(QMouseEvent *event)
{
  Q_ASSERT(!size().isEmpty());
  if (event->button() == Qt::LeftButton)
    setValue(orientation() == Qt::Vertical ?
      minimum() + ((maximum()-minimum()) * (height()-event->y())) / height() :
      minimum() + ((maximum()-minimum()) * event->x()) / width()
    );
  Base::mousePressEvent(event);
}

//void
//QtExt::
//Slider::mouseMoveEvent(QMouseEvent *event)
//{
//  Q_ASSERT(!size().isEmpty());
//  setHoverValue(orientation() == Qt::Vertical ?
//    minimum() + ((maximum()-minimum()) * (height()-event->y())) / height() :
//    minimum() + ((maximum()-minimum()) * event->x()) / width()
//  );
//  Base::mouseMoveEvent(event);
//}

// EOF
