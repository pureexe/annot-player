#ifndef GRAPHICSHALOEFFECT_H
#define GRAPHICSHALOEFFECT_H

// graphicshaloeffect.h
// 5/3/2012
// See: gui/effects/qgraphicseffect.h

#include <QtGui/QGraphicsEffect>

class GraphicsHaloEffectPrivate;
class GraphicsHaloEffect: public QGraphicsEffect
{
  Q_OBJECT
  Q_PROPERTY(QPointF offset READ offset WRITE setOffset NOTIFY offsetChanged)
  Q_PROPERTY(qreal xOffset READ xOffset WRITE setXOffset NOTIFY offsetChanged)
  Q_PROPERTY(qreal yOffset READ yOffset WRITE setYOffset NOTIFY offsetChanged)
  Q_PROPERTY(qreal blurRadius READ blurRadius WRITE setBlurRadius NOTIFY blurRadiusChanged)
  Q_PROPERTY(QColor color READ color WRITE setColor) //NOTIFY colorChanged), notification disabled to improve performance
  Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity) //NOTIFY opacityChanged, notification disabled to improve performance
  Q_DECLARE_PRIVATE(GraphicsHaloEffect)
  Q_DISABLE_COPY(GraphicsHaloEffect)
  typedef GraphicsHaloEffect Self;
  typedef QGraphicsEffect Base;
public:
  explicit GraphicsHaloEffect(QObject *parent = 0);

  QRectF boundingRectFor(const QRectF &rect) const;
  QPointF offset() const;

  qreal xOffset() const { return offset().x(); }

  qreal yOffset() const { return offset().y(); }

  qreal blurRadius() const;
  QColor color() const;

  qreal opacity() const;

public slots:
  void setOffset(const QPointF &ofs);
  void setOffset(qreal dx, qreal dy) { setOffset(QPointF(dx, dy)); }
  void setOffset(qreal d) { setOffset(QPointF(d, d)); }

  void setXOffset(qreal dx) { setOffset(QPointF(dx, yOffset())); }
  void setYOffset(qreal dy) { setOffset(QPointF(xOffset(), dy)); }

  void setBlurRadius(qreal blurRadius);
  void setColor(const QColor &color);
  void setOpacity(qreal opacity);

signals:
  void offsetChanged(const QPointF &offset);
  void blurRadiusChanged(qreal blurRadius);
  //void colorChanged(const QColor &color);
  //void opacityChanged(qreal opacity);

protected:
  void draw(QPainter *painter);
};

#endif // GRAPHICSHALOEFFECT_H
