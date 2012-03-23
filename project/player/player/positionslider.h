#ifndef POSITIONSLIDER_H
#define POSITIONSLIDER_H

// positionslider.h
// 3/20/2012

#include "module/qtext/slider.h"

typedef QtExt::Slider PositionSliderBase;
class PositionSlider : public PositionSliderBase
{
  Q_OBJECT
  typedef PositionSlider Self;
  typedef PositionSliderBase Base;

  int availablePosition_;

public:
  explicit PositionSlider(QWidget *parent = 0);

  int availablePosition() const { return availablePosition_; }
public slots:
  void clearAvailablePosition() { setAvailablePosition(0); }

  void setAvailablePosition(int value);

protected slots:
  void invalidateStyleSheet(int value = 0);
};

#endif // POSITIONSLIDER_H
