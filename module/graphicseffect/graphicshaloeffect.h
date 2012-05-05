#ifndef GRAPHICSHALOEFFECT_H
#define GRAPHICSHALOEFFECT_H

#include <QtGui/QGraphicsEffect>

class PixmapHaloFilter;
class GraphicsHaloEffect: public QGraphicsEffect
{
  Q_OBJECT
  Q_PROPERTY(QPointF offset READ offset WRITE setOffset NOTIFY offsetChanged)
  Q_PROPERTY(qreal xOffset READ xOffset WRITE setXOffset NOTIFY offsetChanged)
  Q_PROPERTY(qreal yOffset READ yOffset WRITE setYOffset NOTIFY offsetChanged)
  Q_PROPERTY(qreal blurRadius READ blurRadius WRITE setBlurRadius NOTIFY blurRadiusChanged)
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_DISABLE_COPY(GraphicsHaloEffect)
  typedef GraphicsHaloEffect Self;
  typedef QGraphicsEffect Base;

  PixmapHaloFilter *filter_;
public:
  explicit GraphicsHaloEffect(QObject *parent = 0);

  QPointF offset() const;

  inline qreal xOffset() const
  { return offset().x(); }

  inline qreal yOffset() const
  { return offset().y(); }

  qreal blurRadius() const;
  QColor color() const;

public slots:
  void setOffset(const QPointF &ofs);

  void setOffset(qreal dx, qreal dy)
  { setOffset(QPointF(dx, dy)); }

  void setOffset(qreal d)
  { setOffset(QPointF(d, d)); }

  void setXOffset(qreal dx)
  { setOffset(QPointF(dx, yOffset())); }

  void setYOffset(qreal dy)
  { setOffset(QPointF(xOffset(), dy)); }

  void setBlurRadius(qreal blurRadius);
  void setColor(const QColor &color);

signals:
  void offsetChanged(const QPointF &offset);
  void blurRadiusChanged(qreal blurRadius);
  void colorChanged(const QColor &color);

public:
  virtual QRectF boundingRectFor(const QRectF &rect) const; ///< \override
protected:
  virtual void draw(QPainter *painter); ///< \override
};

#endif // GRAPHICSEFFECT_H
