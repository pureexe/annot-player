#ifndef GLASSIMAGEFILTER_H
#define GLASSIMAGEFILTER_H

// glassimagefilter.h
// 5/22/2012

#include "module/imagefilter/imagefilter.h"
#include <QtCore/QObject>
#include <QtCore/QPoint>

class GlassImageFilter : public QObject, public ImageFilter
{
  Q_OBJECT
  Q_DISABLE_COPY(GlassImageFilter)
  typedef GlassImageFilter Self;
  typedef QObject Base;

  int radius_;
  QPoint center_;
public:
  explicit GlassImageFilter(QObject *parent = 0)
    : Base(parent), radius_(0) { }

  int radius() const { return radius_; }
  const QPoint &center() const { return center_; }

public slots:
  void setRadius(int value) { radius_ = value; }
  void setCenter(const QPoint &pos) { center_ = pos; }

public:
  virtual QImage &filterImage(QImage &image) const ///< \override
  {
    if (!image.isNull())
      for (int x = 0; x < image.width(); x++)
        for (int y = 0; y < image.height(); y++)
          image.setPixel(x, y, qRgba(255, 255, 255, 0)); // CHECKPOINT
    return image;
  }
};

#endif // GLASSIMAGEFILTER_H
