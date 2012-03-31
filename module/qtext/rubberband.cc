// qtext/rubberbound.cc
// 2012/3/28

#include "module/qtext/rubberband.h"
#include <QtGui>
#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/minmax.hpp>

void
QtExt::
MouseRubberBand::drag(const QPoint &pos)
{
  if (isPressed()) {
    int x1, y1, x2, y2;
    boost::tie(x1, x2) = boost::minmax(pos.x(), pressed_.x());
    boost::tie(y1, y2) = boost::minmax(pos.y(), pressed_.y());
    setGeometry(x1, y1, x2 - x1, y2 - y1);
  }
}

void
QtExt::
MouseRubberBand::press(const QPoint &pos)
{
  //resize(QSize());
  //move(pos);
  setGeometry(pos.x(), pos.y(), 0, 0);

  pressed_ = pos;
  show();
}

void
QtExt::
MouseRubberBand::release()
{
  hide();
  pressed_ = QPoint();
  QRect r = geometry();
  if (!r.isEmpty())
    emit selected(r);
}

// EOF
