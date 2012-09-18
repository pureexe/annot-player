#ifndef QXRUBBERBOUD_H
#define QXRUBBERBOUD_H

// qtext/rubberbound.h
// 2012/3/28

#include "qtx/qtxglobal.h"
#include <QtCore/QPoint>
#include <QtGui/QGraphicsColorizeEffect>
#include <QtGui/QRubberBand>

QTX_BEGIN_NAMESPACE

class QxColorizedRubberBand : public QRubberBand
{
  Q_OBJECT
  Q_DISABLE_COPY(QxColorizedRubberBand)
  typedef QxColorizedRubberBand Self;
  typedef QRubberBand Base;

  QColor color_;
public:
  explicit QxColorizedRubberBand(Shape s, QWidget *parent = nullptr)
    : Base(s, parent) { }

signals:
  void colorChanged(QColor color);
public:
  QColor color() const { return color_; }
public slots:
  void setColor(const QColor &c)
  { if (color_ != c) emit colorChanged(color_ = c); }
};

class QxSquareRubberBand : public QxColorizedRubberBand
{
  Q_OBJECT
  Q_DISABLE_COPY(QxSquareRubberBand)
  typedef QxSquareRubberBand Self;
  typedef QxColorizedRubberBand Base;

public:
  explicit QxSquareRubberBand(Shape s, QWidget *parent = nullptr);

protected slots:
  void invalidateColor();
protected:
  void paintEvent(QPaintEvent *e) override;
};

class QxCircularRubberBand : public QxColorizedRubberBand
{
  Q_OBJECT
  Q_DISABLE_COPY(QxCircularRubberBand)
  typedef QxCircularRubberBand Self;
  typedef QxColorizedRubberBand Base;

  QPoint center_;
  int radius_;

public:
  explicit QxCircularRubberBand(Shape s, QWidget *parent = nullptr)
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

class QxMouseRubberBand : public QxSquareRubberBand
{
  Q_OBJECT
  Q_DISABLE_COPY(QxMouseRubberBand)
  typedef QxMouseRubberBand Self;
  typedef QxSquareRubberBand Base;

  QPoint pressed_;

public:
  explicit QxMouseRubberBand(Shape s, QWidget *parent = nullptr)
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

QTX_END_NAMESPACE

#endif // QXRUBBERBOUD_H
