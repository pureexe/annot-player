#ifndef DARKNESSIMAGEFILTER_H
#define DARKNESSIMAGEFILTER_H

// darknessimagefilter.h
// 5/17/2012

#include "module/imagefilter/simpleimagefilter.h"
#include <QtCore/QObject>

class DarknessImageFilter : public SimpleImageFilter
{
  Q_OBJECT
  Q_DISABLE_COPY(DarknessImageFilter)
  typedef DarknessImageFilter Self;
  typedef SimpleImageFilter Base;

public:
  enum { Brightness = 0, Contrast = 200, Gamma = 200 };

  explicit DarknessImageFilter(QObject *parent = nullptr)
    : Base(parent) { reset(); }

public slots:
  void reset()
  {
    setBrightness(Brightness);
    setContrast(Contrast);
    setGamma(Gamma);
  }
};

#endif // DARKNESSIMAGEFILTER_H
