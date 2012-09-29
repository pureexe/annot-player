#ifndef QXDRAGGABLEQUICKVIEW_H
#define QXDRAGGABLEQUICKVIEW_H

// qxdraggablequickview.h
// 9/3/2011

#include "qx/qxglobal.h"
#include <QtQuick/QQuickView>

QX_BEGIN_NAMESPACE

class QxDraggableQuickViewPrivate;
class QxDraggableQuickView : public QQuickView
{
  Q_OBJECT
  Q_DISABLE_COPY(QxDraggableQuickView)
  QX_EXTEND_CLASS(QxDraggableQuickView, QQuickView)
  QX_DECLARE_PRIVATE(QxDraggableQuickViewPrivate)

public:
  explicit QxDraggableQuickView(QWindow *parent = nullptr);
  explicit QxDraggableQuickView(QQmlEngine *engine, QWindow *parent = nullptr);
  explicit QxDraggableQuickView(const QUrl &source, QWindow *parent = nullptr);
  ~QxDraggableQuickView();

  // - Properties -
public:
  bool isDraggable() const;
public slots:
  void setDraggable(bool value);

  // - Events -
protected:
  //void setVisible(bool visible) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
};

QX_END_NAMESPACE

#endif // QXDRAGGABLEQUICKVIEW_H
