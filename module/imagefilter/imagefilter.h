#ifndef IMAGEFILTER_H
#define IMAGEFILTER_H

// imagefilter.h
// 5/16/2012

#include <QtCore/QPointF>
#include <QtGui/QPainter>

QT_FORWARD_DECLARE_CLASS(QImage)

class ImageFilter
{
public:
  virtual ~ImageFilter() { }
  virtual void drawImage(QPainter &painter, const QPointF &pos, const QImage &image) const
  {
    if (needsDisplay()) {
      QImage copy(image);
      painter.drawImage(pos, filterImage(copy));
    } else
      painter.drawImage(pos, image);
  }

  void drawImage(QPainter &painter, const QImage &image) const
  { return drawImage(painter, QPointF(), image); }

  virtual bool needsDisplay() const { return true; }
  virtual QImage &filterImage(QImage &image) const { return image; }

  QImage &operator()(QImage &image) const
  { return filterImage(image); }
  operator bool() const
  { return needsDisplay(); }
};

#endif // IMAGEFILTER_H
