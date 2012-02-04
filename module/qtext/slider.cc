// qtext/slider.cc
// 8/6/2011

#include "qtext/slider.h"
#include <QtGui>

// - Constructions -

#define SLIDER_INIT \
  hover_(false), hoverCursor_(Qt::PointingHandCursor)

QtExt::
Slider::Slider(QWidget *parent)
  : Base(parent), SLIDER_INIT
{
  setGraphicsEffect(new QGraphicsBlurEffect(this));
}

QtExt::
Slider::Slider(Qt::Orientation orientation, QWidget *parent)
  : Base(orientation, parent), SLIDER_INIT
{ }

#undef SLIDER_INIT

bool
QtExt::
Slider::isHovered() const
{ return hover_; }

QCursor
QtExt::
Slider::hoverCursor() const
{ return hoverCursor_; }

void
QtExt::
Slider::setHoverCursor(const QCursor &cursor)
{ hoverCursor_ = cursor; }

// - Events -

void
QtExt::
Slider::enterEvent(QEvent *event)
{
  Q_UNUSED(event);
  lastCursor_ = cursor();
  setCursor(hoverCursor());
  hover_ = true;
  //emit hovered();
}

void
QtExt::
Slider::leaveEvent(QEvent *event)
{
  Q_UNUSED(event);
  setCursor(lastCursor_);
  hover_ = false;
  //emit leaved();
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
