#ifndef RIPPLEIMAGEFILTER_H
#define RIPPLEIMAGEFILTER_H

// rippleimagefilter.h
// 5/11/2012

#include "module/imagefilter/imagefilter.h"
#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtCore/QPoint>

class RippleImageFilter : public QObject, public ImageFilter
{
  Q_OBJECT
  Q_DISABLE_COPY(RippleImageFilter)
  typedef RippleImageFilter Self;
  typedef QObject Base;

  mutable int **previousMap_; // Last ripple matrix wave
  mutable int **currentMap_;  // Current ripple matrix wave
  mutable QSize mapSize_;
  mutable bool dirty_;

  QPoint center_;

  qreal opacity_;
  qint8 damping_;
  qint8 heigth_;
  qint8 offset_;

public:
  explicit RippleImageFilter(QObject *parent = 0);
  ~RippleImageFilter();

  qreal opacity() const { return opacity_; }
  void setOpacity(qreal value) { opacity_ = value; }

  qint8 dampling() const { return damping_; }
  void setDampling(qint8 value) { damping_ = value; }

  qint8 heigth() const { return heigth_; }
  void setHeigth(qint8 value) { heigth_ = value; }

  const QPoint &center() const { return center_; }
  bool hasCenter() const { return center_.x() >= 0 && center_.y() >= 0; }
  void setCenter(const QPoint &value) { center_ = value; }
  void setCenter(int x, int y) { setCenter(QPoint(x, y)); }

  bool isDirty() const { return dirty_; }

  virtual bool needsDisplay() const { return isDirty() || hasCenter(); } ///< \override

public:
  virtual void drawImage(QPainter &painter, const QPointF &pos, const QImage &image) const; ///< \override

  // FIXME
  void drawImage(QPainter &painter, const QImage &image) const
  { return ImageFilter::drawImage(painter, image); }

public slots:
  void clearCenter() { setCenter(-1, -1); }
  void clear();
};

#endif // RIPPLEIMAGEFILTER_H
