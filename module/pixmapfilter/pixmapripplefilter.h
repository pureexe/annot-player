#ifndef PIXMAPRIPPLEFILTER_H
#define PIXMAPRIPPLEFILTER_H

// pixmapripplefilter.h
// 5/11/2012

#include "module/pixmapfilter/pixmapfilter.h"
#include <QtCore/QSize>
#include <QtCore/QPoint>

QT_FORWARD_DECLARE_CLASS(QImage)

class PixmapRippleFilter : public QPixmapFilter
{
  Q_OBJECT
  Q_DISABLE_COPY(PixmapRippleFilter)
  typedef PixmapRippleFilter Self;
  typedef QPixmapFilter Base;

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
  explicit PixmapRippleFilter(QObject *parent = 0);
  ~PixmapRippleFilter();

  qreal opacity() const { return opacity_; }
  void setOpacity(qreal value) { opacity_ = value; }

  qint8 dampling() const { return damping_; }
  void setDampling(qint8 value) { damping_ = value; }

  qint8 heigth() const { return heigth_; }
  void setHeigth(qint8 value) { heigth_ = value; }

  const QPoint &center() const { return center_; }
  bool hasCenter() const { return center_.x() >= 0 && center_.y() >= 0; }
  void setCenter(const QPoint &value) { center_ = value; }
  void clearCenter() { setCenter(QPoint(-1, -1)); }

  bool isDirty() const { return dirty_; }

  bool needsDisplay() const { return isDirty() || hasCenter(); }
public:
  virtual void draw(QPainter *p, const QPointF &pos, const QPixmap &px, const QRectF &src = QRectF()) const; ///< \override
  void draw(QPainter *p, const QPointF &pos, const QImage &image) const;

public slots:
  void clear();
};

#endif // PIXMAPRIPPLEFILTER_H
