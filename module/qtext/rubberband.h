#ifndef _QTEXT_RUBBERBOUD_H
#define _QTEXT_RUBBERBOUD_H

// qtext/rubberbound.h
// 2012/3/28

#include <QRubberBand>
#include <QGraphicsColorizeEffect>

namespace QtExt {

class MouseRubberBand : public QRubberBand
{
  Q_OBJECT
  typedef MouseRubberBand Self;
  typedef QRubberBand Base;

  QPoint pressed_;

public:
  explicit MouseRubberBand(Shape s, QWidget *parent = 0)
    : Base(s, parent) { }

  bool isPressed() const
  { return !pressed_.isNull(); }

signals:
  void selected(QRect geometry);

public slots:
  void press(const QPoint &pos);
  void drag(const QPoint &pos);
  void release();

  //void pressGlobal(const QPoint &globalPos)
  //{ press(mapFromGlobal(globalPos) - mapFromGlobal(QPoint())); }
  //void dragGlobal(const QPoint &globalPos)
  //{ drag(mapFromGlobal(globalPos) - mapFromGlobal(QPoint())); }

public slots:
  void setColor(const QColor &c, qreal strength = 1.0)
  {
    QGraphicsColorizeEffect *e = 0;
    if (c.isValid()) {
      e = new QGraphicsColorizeEffect(this);
      e->setColor(c);
      e->setStrength(strength);
    }
    setGraphicsEffect(e);
  }
};

} // namespace QtExt

#endif // _QTEXT_RUBBERBOUD_H
