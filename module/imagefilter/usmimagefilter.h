#ifndef USMIMAGEFILTER_H
#define USMIMAGEFILTER_H

// usmimagefilter.h
// 5/17/2012
// Unsharp Masking (USM).
// See: http://stackoverflow.com/questions/2938162/how-does-an-unsharp-mask-work
// See: http://en.wikipedia.org/wiki/Unsharp_masking

#include "module/imagefilter/imagefilter.h"
#include <QtGui/QObject>

class UsmImageFilter : public QObject, public ImageFilter
{
  typedef UsmImageFilter Self;
  typedef QObject Base;

public:
  static QImage &filterImage(const QImage &original, int radius, int amountPercent, int threshold)
  {
    // CHECKPOINT
    return image;
    /*
    // copy original for our return value
    QImage retval = original;

    // create the blurred copy
    // http://en.wikipedia.org/wiki/Gaussian_blur
    QImage blurred = gaussianBlur(original, radius);

    // subtract blurred from original, pixel-by-pixel to make unsharp mask
    QImage unsharpMask = difference(original, blurred);

    QImage highContrast = increaseContrast(original, amountPercent);

    // assuming row-major ordering
    for (int x = 0; x < original.width(); x++)
      for (int y = 0; y < original.height(); y++) {
        uint originalColor = original.pixel(x, y),
             contrastColor = highContrast.pixel(x, y);

        int difference = contrastColor - originalColor;
        qreal percent = luminanceAsPercent(unsharpMask.pixel(x, y));
        int delta = difference * percent;

        if (qAbs(delta) > threshold)
          retval.pixel(x, y) += delta;
      }

    return retval;
    */
  }
};

#endif // USMIMAGEFILTER_H
