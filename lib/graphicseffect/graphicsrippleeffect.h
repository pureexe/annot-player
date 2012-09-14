#ifndef GRAPHICSRIPPLEEFFECT_H
#define GRAPHICSRIPPLEEFFECT_H

// graphicsrippleeffect.h
// 5/3/2012
// See: class RippleEffect from kdelibs/plasma/private/effects/ripple_p.h
// See: http://freespace.virgin.net/hugo.elias/graphics/x_water.htm

#include <QtGui/QGraphicsEffect>

///  Simulates a ripple effect on the source. This class can be used to simulate a "water" animation.
class GraphicsRippleEffect : public QGraphicsEffect
{
  Q_OBJECT
  Q_PROPERTY(qint8 offset READ offset WRITE setOffset NOTIFY offsetChanged)
  Q_PROPERTY(qint8 damping READ damping WRITE setDamping NOTIFY dampingChanged)
  Q_PROPERTY(qint8 heigth READ heigth WRITE setHeigth NOTIFY heigthChanged)
  Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
  Q_PROPERTY(QPoint center READ center WRITE setCenter)
  Q_DISABLE_COPY(GraphicsRippleEffect)
  typedef GraphicsRippleEffect Self;
  typedef QGraphicsEffect Base;

  qint8 offset_;      // Ripple offset (default is 1)
  qint8 damping_;     // Ripple damping factor (default is 16)
  qint8 heigth_;      // Ripple wave heigth factor (default is 1)
  qreal opacity_;     // Ripple opacity level (default is 0.0)
  QSize mapSize_;     // Ripple matrix wave size
  int **previousMap_; // Last ripple matrix wave
  int **currentMap_;  // Current ripple matrix wave

  QPoint center_;

public:
  explicit GraphicsRippleEffect(QObject *parent = nullptr);
  ~GraphicsRippleEffect();

  // - Properties -
public:
  /**
   * Returns the ripple offset. The offset is used to calculate the distande between
   * neighbour pixels.
   * \return ripple offset
   */
  qint8 offset() const { return offset_; }

  /**
   * Returns the ripple damping factor. The damping factor is used to reduce the wave height
   * through each pass.
   * \return ripple damping factor
   */
  qint8 damping() const { return damping_; }

  /**
   * Returns the ripple wave heigth factor. The heigth factor is used to enlarge or reduce the
   * initial wave heigth.
   * \return ripple wave heigth factor
   */
  qint8 heigth() const { return heigth_; }

  /**
   * Returns the ripple opacity. The opacity is used to reduce the effect opacity when
   * animating.
   * \return ripple opacity level
   */
  qreal opacity() const { return opacity_; }

  QPoint center() const { return center_; }

public slots:
  ///  Set ripple offset (e.g. 1).
  void setOffset(qint8 offset)
  { emit offsetChanged(offset_ = offset); }

  ///  Set ripple damping factor (e.g. 16).
  void setDamping(qint8 damping)
  { emit dampingChanged(damping_ = damping); }

  ///  Set ripple wave heigth factor (e.g. 1).
  void setHeigth(qint8 heigth)
  { emit heigthChanged(heigth_ = heigth); }

  void setOpacity(qreal opacity); ///< Set ripple opacity level (e.g. 1.0).

  void setCenter(const QPoint &pos) { center_ = pos; }
  void setCenter(int x, int y) { setCenter(QPoint(x, y)); }

signals:
  void offsetChanged(qint8 offset);
  void dampingChanged(qint8 damping);
  void heigthChanged(qint8 heigth);

  // - Implementation -
protected:
  void draw(QPainter *painter) override;
};

#endif // GRAPHICSRIPPLEEFFECT_H
