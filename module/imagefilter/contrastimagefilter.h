#ifndef CONTRASTIMAGEFILTER_H
#define CONTRASTIMAGEFILTER_H

// contrastimagefilter.h
// 5/17/2012
// See: http://stackoverflow.com/questions/4981474/brightness-contrast-gamma-adjustments-with-c-qt

#include "module/imagefilter/pixelimagefilter.h"
#include <QtCore/QObject>
#include <cmath>

class ContrastImageFilter : public QObject, public PixelImageFilter
{
  Q_OBJECT
  Q_PROPERTY(int value READ value WRITE setValue)
  Q_DISABLE_COPY(ContrastImageFilter)
  typedef ContrastImageFilter Self;
  typedef QObject Base;

  int contrast_;

public:
  enum { DefaultValue = 100 };

  explicit ContrastImageFilter(QObject *parent = 0)
    : Base(parent), contrast_(DefaultValue) { }
  explicit ContrastImageFilter(int contrast, QObject *parent = 0)
    : Base(parent), contrast_(contrast) { }

  int value() const { return contrast_; }
public slots:
  void setValue(int contrast) { contrast_ = contrast; }
  void clear() { setValue(DefaultValue); }

public:
  static quint8 filterColor(quint8 value, int contrast)
  { return qBound<int>(0, (value-127)*contrast/100 + 127, 255 ); }

  static bool needsDisplay(int contrast)
  { return contrast != DefaultValue; }

  virtual quint8 filterColor(quint8 value) const ///< \reimp
  { return filterColor(value, contrast_); }

  virtual bool needsDisplay() const ///< \reimp
  { return needsDisplay(contrast_); }
};

#endif // CONTRASTIMAGEFILTER_H
