#ifndef _QTEXT_RUBBERBOUD_H
#define _QTEXT_RUBBERBOUD_H

// qtext/rubberbound.h
// 2012/3/28

#include <QRubberBand>
#include <QGraphicsColorizeEffect>

namespace QtExt {

class RubberBandWithColor : public QRubberBand
{
  Q_OBJECT
  typedef RubberBandWithColor Self;
  typedef QRubberBand Base;

  QColor color_;
public:
  explicit RubberBandWithColor(Shape s, QWidget *parent = 0)
    : Base(s, parent) { }

  QColor color() const { return color_; }
public slots:
  void setColor(const QColor &c);

protected:
  virtual void paintEvent(QPaintEvent *e); ///< \override
};

class MouseRubberBand : public RubberBandWithColor
{
  Q_OBJECT
  typedef MouseRubberBand Self;
  typedef RubberBandWithColor Base;

  QPoint pressed_;

public:
  explicit MouseRubberBand(Shape s, QWidget *parent = 0)
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
