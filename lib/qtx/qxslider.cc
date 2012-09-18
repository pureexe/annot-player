// qxslider.cc
// 8/6/2011

#include "qtx/qxslider.h"
#include <QtGui/QMouseEvent>

QTX_BEGIN_NAMESPACE

// - Events -

void
QxSlider::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QxSlider::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

// Click'n'Go instead of move steps
// See: http://lists.trolltech.com/qt-interest/2006-11/msg00363.html
void
QxSlider::mousePressEvent(QMouseEvent *event)
{
  Q_ASSERT(!size().isEmpty());
  if (event->button() == Qt::LeftButton)
    setValue(orientation() == Qt::Vertical ?
      minimum() + ((maximum()-minimum()) * (height()-event->y())) / height() :
      minimum() + ((maximum()-minimum()) * event->x()) / width()
    );
  Base::mousePressEvent(event);
}

QTX_END_NAMESPACE

//void
//QxSlider::mouseMoveEvent(QMouseEvent *event)
//{
//  Q_ASSERT(!size().isEmpty());
//  setHoverValue(orientation() == Qt::Vertical ?
//    minimum() + ((maximum()-minimum()) * (height()-event->y())) / height() :
//    minimum() + ((maximum()-minimum()) * event->x()) / width()
//  );
//  Base::mouseMoveEvent(event);
//}

