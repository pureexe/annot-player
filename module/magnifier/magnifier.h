#ifndef MAGNIFIER_H
#define MAGNIFIER_H

// magnifier.h
// 5/11/2012

#include "module/qtext/draggablewidget.h"
#include <QtCore/QPoint>
#include <QtCore/QSize>

QT_FORWARD_DECLARE_CLASS(QTimer)
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QAction)

class ImageFilter;
class FadeAnimation;
class RippleImageFilter;
class Magnifier : public QtExt::DraggableWidget
{
  Q_OBJECT
  Q_PROPERTY(qreal windowOpacity READ windowOpacity WRITE setWindowOpacity)
  Q_DISABLE_COPY(Magnifier)
  typedef Magnifier Self;
  typedef QtExt::DraggableWidget Base;

  const QWidget *w_;
  QPoint center_;
  qreal scale_;
  QTimer *repaintTimer_;
  QTimer *hideTimer_;

  ImageFilter *darknessFilter_, *fogFilter_, *mosaicFilter_;
  RippleImageFilter *rippleFilter_;
  FadeAnimation *fadeAni_;

  QMenu *contextMenu_;
  QAction *darknessFilterAct_,
          *fogFilterAct_,
          *mosaicFilterAct_;
public:
  explicit Magnifier(QWidget *parent = nullptr);

  void setWidget(const QWidget *w) { w_ = w; }
  void setScale(qreal scale) { scale_ = scale; }
  void setCenter(const QPoint &pos) { center_ = pos; }

public slots:
  void setVisible(bool visible) override;
  void fadeOut();
  void trigger();

protected:
  void paintEvent(QPaintEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);
  void contextMenuEvent(QContextMenuEvent *event);
  void wheelEvent(QWheelEvent *event);
};

#endif // MAGNIFIER_H
