// qtext/rubberbound.cc
// 2012/3/28

#include "module/qtext/rubberband.h"
#include <QtGui>
#include <boost/algorithm/minmax.hpp>
#include <boost/tuple/tuple.hpp>

// - RubberBandWithColor -

void
QtExt::
RubberBandWithColor::setColor(const QColor &color)
{
  color_ = color;
#ifndef Q_WS_MAC
  enum { length = 1 };
  QGraphicsColorizeEffect *e = 0;
  if (color_.isValid()) {
    e = new QGraphicsColorizeEffect(this);
    e->setColor(color_);
    e->setStrength(1);
  }
  setGraphicsEffect(e);
#endif // Q_WS_MAC
}

void
QtExt::
RubberBandWithColor::paintEvent(QPaintEvent *e)
{
#ifdef Q_WS_MAC
  if (color_.isValid()) {
    enum { width = 2, alpha = 32 };

    QPen pen(color_, width);
    pen.setStyle(Qt::SolidLine);
    QPainter painter;
    painter.begin(this);
    painter.setPen(pen);
    if (shape() == Rectangle) {
      QColor c = color_;
      c.setAlpha(alpha);
      QBrush brush(c);
      brush.setStyle(Qt::Dense1Pattern);
      painter.setBrush(brush);
    }
    painter.drawRect(e->rect());
    painter.end();
  } else
#endif // Q_WS_MAC
  Base::paintEvent(e);
}

// - MouseRubberBound -

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

  QApplication::setOverrideCursor(Qt::CrossCursor);
  show();
}

void
QtExt::
MouseRubberBand::release()
{
  cancel();
  QRect r = geometry();
  if (!r.isEmpty())
    emit selected(r);
}

void
QtExt::
MouseRubberBand::cancel()
{
  hide();
  if (QApplication::overrideCursor())
    QApplication::restoreOverrideCursor();
  pressed_ = QPoint();
}


// EOF
