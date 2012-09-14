#ifndef FOGIMAGEFILTER_H
#define FOGIMAGEFILTER_H

// fogimagefilter.h
// 5/17/2012

#include "lib/imagefilter/simpleimagefilter.h"
#include <QtCore/QObject>

class FogImageFilter : public SimpleImageFilter
{
  Q_OBJECT
  Q_DISABLE_COPY(FogImageFilter)
  typedef FogImageFilter Self;
  typedef SimpleImageFilter Base;

public:
  enum { Brightness = 0, Contrast = 100, Gamma = 300 };

  explicit FogImageFilter(QObject *parent = nullptr)
    : Base(parent) { reset(); }

public slots:
  void reset()
  {
    setBrightness(Brightness);
    setContrast(Contrast);
    setGamma(Gamma);
  }
};

#endif // FOGIMAGEFILTER_H
