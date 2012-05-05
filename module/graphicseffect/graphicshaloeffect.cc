// graphicshaloeffect.cc
// 5/3/2012
#include "module/graphicseffect/graphicshaloeffect.h"
#ifdef WITH_MODULE_PIXMAPFILTER
#  include "module/pixmapfilter/pixmaphalofilter.h"
#else
#  error "pixmapfilter module is required"
#endif // WITH_MODULE_PIXMAPFILTER
#include <QtGui/QPainter>
#include <QtGui/QPaintEngine>

GraphicsHaloEffect::GraphicsHaloEffect(QObject *parent)
  : Base(parent)
{ filter_ = new PixmapHaloFilter(this); }

QPointF
GraphicsHaloEffect::offset() const
{ return filter_->offset(); }

void
GraphicsHaloEffect::setOffset(const QPointF &offset)
{
  if (filter_->offset() != offset) {
    filter_->setOffset(offset);
    updateBoundingRect();
    emit offsetChanged(offset);
  }
}

qreal
GraphicsHaloEffect::blurRadius() const
{ return filter_->blurRadius(); }

void
GraphicsHaloEffect::setBlurRadius(qreal blurRadius)
{
  if (!qFuzzyCompare(filter_->blurRadius(), blurRadius)) {
    filter_->setBlurRadius(blurRadius);
    updateBoundingRect();
    emit blurRadiusChanged(blurRadius);
  }
}

QColor
GraphicsHaloEffect::color() const
{ return filter_->color(); }

void
GraphicsHaloEffect::setColor(const QColor &color)
{
  if (filter_->color() != color) {
    filter_->setColor(color);
    update();
    emit colorChanged(color);
  }
}

QRectF
GraphicsHaloEffect::boundingRectFor(const QRectF &rect) const
{ return filter_->boundingRectFor(rect); }

void
GraphicsHaloEffect::draw(QPainter *painter)
{
  if (filter_->blurRadius() <= 0 && filter_->offset().isNull()) {
    drawSource(painter);
    return;
  }

  PixmapPadMode mode = PadToEffectiveBoundingRect;
  if (painter->paintEngine()->type() == QPaintEngine::OpenGL2)
    mode = NoPad;

  // Draw pixmap in device coordinates to avoid pixmap scaling.
  QPoint offset;
  const QPixmap pixmap = sourcePixmap(Qt::DeviceCoordinates, &offset, mode);
  if (pixmap.isNull())
    return;

  QTransform restoreTransform = painter->worldTransform();
  painter->setWorldTransform(QTransform());
  filter_->draw(painter, offset, pixmap);
  painter->setWorldTransform(restoreTransform);
}

// EOF
