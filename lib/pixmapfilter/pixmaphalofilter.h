#ifndef PIXMAPHALOFILTER_H
#define PIXMAPHALOFILTER_H

// pixmaphalofilter.h
// 5/3/2012
// See: gui/image/pixmapfilter_p.h

#include "lib/pixmapfilter/pixmapfilter.h"
#include <QtCore/QPointF>
#include <QtGui/QColor>

class PixmapHaloFilter : public QPixmapFilter
{
  Q_OBJECT
  typedef PixmapHaloFilter Self;
  typedef QPixmapFilter Base;

  QPointF offset_;
  QColor color_;
  qreal radius_;

public:
  //enum { Type = UserFilter + DropShadowFilter }; ///< must be unique to all QPixMapFilter class
  explicit PixmapHaloFilter(QObject *parent = nullptr);

  QRectF boundingRectFor(const QRectF &rect) const override;
  void draw(QPainter *p, const QPointF &pos, const QPixmap &px, const QRectF &src = QRectF()) const override;

  qreal blurRadius() const { return radius_; }
  void setBlurRadius(qreal radius) { radius_ = radius; }

  QColor color() const { return color_; }
  void setColor(const QColor &color) { color_ = color; }

  QPointF offset() const { return offset_; }
  void setOffset(const QPointF &offset) { offset_ = offset; }
  void setOffset(qreal dx, qreal dy) { setOffset(QPointF(dx, dy)); }
};


#endif // PIXMAPHALOFILTER_H
