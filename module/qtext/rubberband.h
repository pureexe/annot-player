#ifndef _QTEXT_RUBBERBOUD_H
#define _QTEXT_RUBBERBOUD_H

// qtext/rubberbound.h
// 2012/3/28

#include <QtCore/QPoint>
#include <QtGui/QGraphicsColorizeEffect>
#include <QtGui/QRubberBand>

namespace QtExt {

class ColorizedRubberBand : public QRubberBand
{
  Q_OBJECT
  Q_DISABLE_COPY(ColorizedRubberBand)
  typedef ColorizedRubberBand Self;
  typedef QRubberBand Base;

  QColor color_;
public:
  explicit ColorizedRubberBand(Shape s, QWidget *parent = nullptr)
    : Base(s, parent) { }

signals:
  void colorChanged(QColor color);
public:
  QColor color() const { return color_; }
public slots:
  void setColor(const QColor &c)
  { if (color_ != c) emit colorChanged(color_ = c); }
};

class SquareRubberBand : public ColorizedRubberBand
{
  Q_OBJECT
  Q_DISABLE_COPY(SquareRubberBand)
  typedef SquareRubberBand Self;
  typedef ColorizedRubberBand Base;

public:
  explicit SquareRubberBand(Shape s, QWidget *parent = nullptr);

protected slots:
  void invalidateColor();
protected:
  void paintEvent(QPaintEvent *e) override;
};

class CircularRubberBand : public ColorizedRubberBand
{
  Q_OBJECT
  Q_DISABLE_COPY(CircularRubberBand)
  typedef CircularRubberBand Self;
  typedef ColorizedRubberBand Base;

  QPoint center_;
  int radius_;

public:
  explicit CircularRubberBand(Shape s, QWidget *parent = nullptr)
    : Base(s, parent), radius_(0) { }

  int radius() const { return radius_; }
  QPoint center() const { return center_; }
public slots:
  void setRadius(int value)
  { if (radius_ != value) { radius_ = value; updateGeometry(); } }

  void setCenter(const QPoint &pos)
  { if (center_ != pos) { center_ = pos; updateGeometry(); } }

  void updateGeometry();
protected:
  void paintEvent(QPaintEvent *e) override;
};

class MouseRubberBand : public SquareRubberBand
{
  Q_OBJECT
  Q_DISABLE_COPY(MouseRubberBand)
  typedef MouseRubberBand Self;
  typedef SquareRubberBand Base;

  QPoint pressed_;

public:
  explicit MouseRubberBand(Shape s, QWidget *parent = nullptr)
    : Base(s, parent) { }

  bool isPressed() const
  { return !pressed_.isNull(); }

  bool isEmpty() const { return size().isEmpty(); }

signals:
  void selected(QRect geometry);

public slots:
  void press(const QPoint &pos);
  void drag(const QPoint &pos);
  void release();
  void cancel();

  //void pressGlobal(const QPoint &globalPos)
  //{ press(mapFromGlobal(globalPos) - mapFromGlobal(QPoint())); }
  //void dragGlobal(const QPoint &globalPos)
  //{ drag(mapFromGlobal(globalPos) - mapFromGlobal(QPoint())); }
};

} // namespace QtExt

#endif // _QTEXT_RUBBERBOUD_H
