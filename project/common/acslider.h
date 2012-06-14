#ifndef ACSLIDER_H
#define ACSLIDER_H

// acslider.h
// 5/19/2012

#include "module/qtext/slider.h"

class AcSlider : public QtExt::Slider
{
  Q_OBJECT
  Q_DISABLE_COPY(AcSlider)
  typedef Slider Self;
  typedef QtExt::Slider Base;

  bool hovered_;

public:
  explicit AcSlider(QWidget *parent = 0)
    : Base(parent), hovered_(false) { }
  explicit AcSlider(Qt::Orientation orientation, QWidget *parent = 0)
    : Base(orientation, parent), hovered_(false) { }

public:
  bool isHovered() const { return hovered_ || underMouse(); }

  // - Events -
protected:
  virtual void enterEvent(QEvent *event); ///< \reimp
  virtual void leaveEvent(QEvent *event); ///< \reimp
};

#endif // ACSLIDER_H
