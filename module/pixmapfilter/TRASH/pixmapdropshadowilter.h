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

  QPointF offset_;
  QColor color_;
  qreal radius_;

public:
  explicit PixmapHaloFilter(QObject *parent = 0);

  qreal blurRadius() const { return radius_; }
  void setBlurRadius(qreal radius) { radius_ = radius; }

  QColor color() const { return color_; }
  void setColor(const QColor &color) { color_ = color; }

  QPointF offset() const { return offset_; }
  void setOffset(const QPointF &offset) { offset_ = offset; }
  void setOffset(qreal dx, qreal dy) { setOffset(QPointF(dx, dy)); }

public:
  virtual QRectF boundingRectFor(const QRectF &rect) const; ///< \override
  virtual void draw(QPainter *p, const QPointF &pos, const QPixmap &px, const QRectF &src = QRectF()) const; ///< \override
};

#endif // PIXMAPHALOFILTER_H
