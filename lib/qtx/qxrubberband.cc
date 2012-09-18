// qxrubberbound.cc
// 2012/3/28

#include "qtx/qxrubberband.h"
#include <QtGui>
#include <boost/algorithm/minmax.hpp>
#include <boost/tuple/tuple.hpp>

QTX_BEGIN_NAMESPACE

// - SquareRubberBand -

QxSquareRubberBand::QxSquareRubberBand(Shape s, QWidget *parent)
  : Base(s, parent)
{
#ifndef Q_OS_MAC
  connect(this, SIGNAL(colorChanged(QColor)), SLOT(invalidateColor()));
#endif // Q_OS_MAC
}

void
QxSquareRubberBand::invalidateColor()
{
  enum { strength = 1 };
  QGraphicsColorizeEffect *e = nullptr;
  QColor c = color();
  if (c.isValid()) {
    e = qobject_cast<QGraphicsColorizeEffect *>(graphicsEffect());
    if (!e)
      e = new QGraphicsColorizeEffect(this);
    e->setColor(c);
    e->setStrength(strength);
  }
  setGraphicsEffect(e);
}

void
QxSquareRubberBand::paintEvent(QPaintEvent *e)
{
#ifdef Q_OS_MAC
  QColor c = color();
  if (c.isValid()) {
    enum { width = 2, alpha = 32 };

    QPen pen(c, width);
    pen.setStyle(Qt::SolidLine);
    QPainter painter;
    painter.begin(this);
    painter.setPen(pen);
    if (shape() == Rectangle) {
      c.setAlpha(alpha);
      QBrush brush(c);
      brush.setStyle(Qt::Dense1Pattern);
      painter.setBrush(brush);
    }
    painter.drawRect(e->rect());
    painter.end();
  } else
#endif // Q_OS_MAC
  Base::paintEvent(e);
}

// - CircularRubberBand -

void
QxCircularRubberBand::updateGeometry()
{
  QRect r(0, 0, radius_*2, radius_*2);
  r.moveCenter(center_);
  setGeometry(r);
}

void
QxCircularRubberBand::paintEvent(QPaintEvent *e)
{
  QColor c = color();
  if (c.isValid()) {
    enum { width = 1, alpha = 32 };

    QPen pen(c, width);
    pen.setStyle(Qt::SolidLine);
    QPainter painter;
    painter.begin(this);
    painter.setPen(pen);
    if (shape() == Rectangle) {
      c.setAlpha(alpha);
      QBrush brush(c);
      brush.setStyle(Qt::Dense1Pattern);
      painter.setBrush(brush);
    }
    //painter.drawEllipse(e->rect());
    QRect r = rect();
    r.moveCenter(center_);
    painter.drawEllipse(r);
    painter.end();
  } else
  Base::paintEvent(e);
}

// - MouseRubberBound -

void
QxMouseRubberBand::drag(const QPoint &pos)
{
  if (isPressed()) {
    int x1, y1, x2, y2;
    boost::tie(x1, x2) = boost::minmax(pos.x(), pressed_.x());
    boost::tie(y1, y2) = boost::minmax(pos.y(), pressed_.y());
    setGeometry(x1, y1, x2 - x1, y2 - y1);
  }
}

void
QxMouseRubberBand::press(const QPoint &pos)
{
  //resize(QSize());
  //move(pos);
  setGeometry(pos.x(), pos.y(), 0, 0);

  pressed_ = pos;

  QApplication::setOverrideCursor(Qt::CrossCursor);
  show();
}

void
QxMouseRubberBand::release()
{
  cancel();
  QRect r = geometry();
  if (!r.isEmpty())
    emit selected(r);
}

void
QxMouseRubberBand::cancel()
{
  hide();
  if (QApplication::overrideCursor())
    QApplication::restoreOverrideCursor();
  pressed_ = QPoint();
}


QTX_END_NAMESPACE
