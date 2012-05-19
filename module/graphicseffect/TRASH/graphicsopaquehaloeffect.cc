// graphicsopaquehaloeffect.cc
// 5/3/2012
#include "module/graphicseffect/graphicsopaquehaloeffect.h"
#include <QtGui/QPainter>
#include <QtGui/QPaintEngine>

// - Construction -

GraphicsOpaqueHaloEffect::GraphicsOpaqueHaloEffect(QObject *parent)
  : Base(parent),
    opacity_(0.7), isFullyTransparent_(0), isFullyOpaque_(0), hasOpacityMask_(0)
{ }

// - Properties -

void
GraphicsOpaqueHaloEffect::setOpacity(qreal opacity)
{
  opacity = qBound<qreal>(0.0, opacity, 1.0);

  if (qFuzzyCompare(opacity_, opacity))
      return;

  opacity_ = opacity;
  if ((isFullyTransparent_ = qFuzzyIsNull(opacity_)))
    isFullyOpaque_ = 0;
  else
    isFullyOpaque_ = qFuzzyIsNull(opacity_ - 1);
  update();
  emit opacityChanged(opacity);
}

void
GraphicsOpaqueHaloEffect::setOpacityMask(const QBrush &mask)
{
  if (opacityMask_ == mask)
     return;

  opacityMask_ = mask;
  hasOpacityMask_ = (mask.style() != Qt::NoBrush);
  update();

  emit opacityMaskChanged(mask);
}

void
GraphicsOpaqueHaloEffect::draw(QPainter *painter)
{
  // Transparent; nothing to draw.
  if (isFullyTransparent_)
      return;

  // Opaque; draw directly without going through a pixmap.
  if (isFullyOpaque_ && !hasOpacityMask_) {
    drawSource(painter);
    return;
  }

  QPoint offset;
  Qt::CoordinateSystem system = sourceIsPixmap() ? Qt::LogicalCoordinates : Qt::DeviceCoordinates;
  QPixmap pixmap = sourcePixmap(system, &offset, QGraphicsEffect::NoPad);
  if (pixmap.isNull())
    return;

  painter->save();
  painter->setOpacity(opacity_);

  if (hasOpacityMask_) {
    QPainter pixmapPainter(&pixmap);
    pixmapPainter.setRenderHints(painter->renderHints());
    pixmapPainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    if (system == Qt::DeviceCoordinates) {
      QTransform worldTransform = painter->worldTransform();
      worldTransform *= QTransform::fromTranslate(-offset.x(), -offset.y());
      pixmapPainter.setWorldTransform(worldTransform);
      pixmapPainter.fillRect(sourceBoundingRect(), opacityMask_);
    } else {
      pixmapPainter.translate(-offset);
      pixmapPainter.fillRect(pixmap.rect(), opacityMask_);
    }
  }

  if (system == Qt::DeviceCoordinates)
    painter->setWorldTransform(QTransform());

  painter->drawPixmap(offset, pixmap);
  painter->restore();
}

// EOF
