#ifndef PIXMAPHALOFILTER_H
#define PIXMAPHALOFILTER_H

// pixmaphalofilter.h
// 5/3/2012

#include "module/pixmapfilter/pixmapfilter.h"

class PixmapHaloFilter : public QPixmapFilter
{
  Q_OBJECT
  Q_DISABLE_COPY(PixmapHaloFilter)
  typedef PixmapHaloFilter Self;
  typedef QPixmapFilter Base;

  int radius_;

public:
  explicit PixmapHaloFilter(QObject *parent = 0);

  int radius() const { return radius_; }
  void setRadius(int radius) { radius_ = radius; }
public:
  virtual void draw(QPainter *p, const QPointF &pos, const QPixmap &pm, const QRectF &src = QRectF()) const; ///< \override
  QImage &transform(QImage &image) const;
};

#endif // PIXMAPHALOFILTER_H
