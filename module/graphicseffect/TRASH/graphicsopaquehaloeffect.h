#ifndef GRAPHICSOPAQUEHALOEFFECT_H
#define GRAPHICSOPAQUEHALOEFFECT_H

// graphicsopaquehaloeffect.h
// 5/3/2012
// See: QGraphicsOpacityEffectPrivate in qgraphicseffect_p.h

#include "module/graphicseffect/graphicshaloeffect.h"

// FIXME: Runtime corruption!

//class GraphicsOpaqueHaloEffect: public GraphicsHaloEffect
class GraphicsOpaqueHaloEffect: public QGraphicsEffect
{
  Q_OBJECT
  Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
  Q_PROPERTY(QBrush opacityMask READ opacityMask WRITE setOpacityMask NOTIFY opacityMaskChanged)
  Q_DISABLE_COPY(GraphicsOpaqueHaloEffect)
  typedef GraphicsOpaqueHaloEffect Self;
  typedef QGraphicsEffect Base;

  qreal opacity_;
  QBrush opacityMask_;
  uint isFullyTransparent_;
  uint isFullyOpaque_;
  uint hasOpacityMask_;

public:
  explicit GraphicsOpaqueHaloEffect(QObject *parent = nullptr);

  qreal opacity() const { return opacity_; }
  QBrush opacityMask() const { return opacityMask_; }

public slots:
  void setOpacity(qreal opacity);
  void setOpacityMask(const QBrush &mask);

signals:
  void opacityChanged(qreal opacity);
  void opacityMaskChanged(const QBrush &mask);

protected:
  void draw(QPainter *painter) override;
};

#endif // GRAPHICSOPAQUEEFFECT_H
