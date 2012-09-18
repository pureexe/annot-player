#ifndef ACSLIDER_H
#define ACSLIDER_H

// acslider.h
// 5/19/2012

#include "qtx/qxslider.h"

typedef QxSlider AcSliderBase;
class AcSlider : public AcSliderBase
{
  Q_OBJECT
  Q_DISABLE_COPY(AcSlider)
  typedef AcSlider Self;
  typedef AcSliderBase Base;

  bool hovered_;

public:
  explicit AcSlider(QWidget *parent = nullptr)
    : Base(parent), hovered_(false) { }
  explicit AcSlider(Qt::Orientation orientation, QWidget *parent = nullptr)
    : Base(orientation, parent), hovered_(false) { }

public:
  bool isHovered() const { return hovered_ || underMouse(); }

  // - Events -
protected:
  void enterEvent(QEvent *event) override;
  void leaveEvent(QEvent *event) override;
};

#endif // ACSLIDER_H
