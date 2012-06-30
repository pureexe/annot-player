// graphicsshadoweffect.cc
// 5/3/2012
#include "module/graphicseffect/graphicsshadoweffect.h"
//#ifdef WITH_MODULE_PIXMAPFILTER
//# include "module/pixmapfilter/pixmapshadowfilter.h"
//#else
//# error "pixmapfilter module is required"
//#endif // WITH_MODULE_PIXMAPFILTER
//#include <QtGui/QPaintEngine>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QImage>

// - Properties -

void
GraphicsShadowEffect::setOpacity(qreal opacity)
{
  if (!qFuzzyCompare(opacity, opacity_)) {
    opacity_ = opacity;
    update();
    //emit opacityChanged(opacity);
  }
}

// - Draw -

void
GraphicsShadowEffect::draw(QPainter *painter)
{
  if (!qFuzzyCompare(opacity_, 1))
    painter->setOpacity(opacity_);
  Base::draw(painter);
  //if (filter_->blurRadius() <= 0 && filter_->offset().isNull()) {
  //  drawSource(painter);
  //  return;
  //}
//
  //PixmapPadMode mode = PadToEffectiveBoundingRect;
  //if (painter->paintEngine()->type() == QPaintEngine::OpenGL2)
  //  mode = NoPad;
//
  //// Draw pixmap in device coordinates to avoid pixmap scaling.
  //QPoint offset;
  //const QPixmap pixmap = sourcePixmap(Qt::DeviceCoordinates, &offset, mode);
  //if (pixmap.isNull())
  //  return;
//
  //QTransform restoreTransform = painter->worldTransform();
  //painter->setWorldTransform(QTransform());
  //filter_->draw(painter, offset, pixmap);
  //painter->setWorldTransform(restoreTransform);
}

// EOF

/*
GraphicsShadowEffect::GraphicsShadowEffect(QObject *parent)
  : Base(parent), opacity_(1.0)
{ filter_ = new PixmapShadowFilter(this); }

// - Properties -

QPointF
GraphicsShadowEffect::offset() const
{ return filter_->offset(); }

void
GraphicsShadowEffect::setOffset(const QPointF &offset)
{
  if (filter_->offset() != offset) {
    filter_->setOffset(offset);
    updateBoundingRect();
    //emit offsetChanged(offset);
  }
}

qreal
GraphicsShadowEffect::blurRadius() const
{ return filter_->blurRadius(); }

void
GraphicsShadowEffect::setBlurRadius(qreal blurRadius)
{
  if (!qFuzzyCompare(filter_->blurRadius(), blurRadius)) {
    filter_->setBlurRadius(blurRadius);
    updateBoundingRect();
    //emit blurRadiusChanged(blurRadius);
  }
}

QColor
GraphicsShadowEffect::color() const
{ return filter_->color(); }

void
GraphicsShadowEffect::setColor(const QColor &color)
{
  if (filter_->color() != color) {
    filter_->setColor(color);
    update();
    //emit colorChanged(color);
  }
}

QRectF
GraphicsShadowEffect::boundingRectFor(const QRectF &rect) const
{ return filter_->boundingRectFor(rect); }

*/
