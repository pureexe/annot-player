#ifndef PIXELIMAGEFILTER_H
#define PIXELIMAGEFILTER_H

// pixelimagefilter.h
// 5/17/2012

#include "module/imagefilter/imagefilter.h"
#include <QtGui/QColor>
#include <QtGui/QImage>

class PixelImageFilter : public ImageFilter
{
  typedef PixelImageFilter Self;
  typedef ImageFilter Base;

public:
  virtual QImage &filterImage(QImage &image) const ///< \reimp
  {
    if (!image.isNull() && needsDisplay())
      for (int x = 0; x < image.width(); x++)
        for (int y = 0; y < image.height(); y++)
          image.setPixel(x, y,
            filterPixel(image.pixel(x, y)));
    return image;
  }
protected:
  virtual uint filterPixel(uint pixel) const
  {
    return qRgba(
      filterRed(qRed(pixel)),
      filterGreen(qGreen(pixel)),
      filterBlue(qBlue(pixel)),
      filterAlpha(qAlpha(pixel)));
  }

  virtual quint8 filterColor(quint8 value) const
  { return value; }

  quint8 filterRed(quint8 value) const
  { return filterColor(value); }

  quint8 filterGreen(quint8 value) const
  { return filterColor(value); }

  quint8 filterBlue(quint8 value) const
  { return filterColor(value); }

  quint8 filterAlpha(quint8 value) const
  { return value; }
};

#endif // PIXELIMAGEFILTER_H
