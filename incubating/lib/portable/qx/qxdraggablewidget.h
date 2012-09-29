#ifndef QXDRAGGABLEWIDGET_H
#define QXDRAGGABLEWIDGET_H

// qxdraggablewindow.h
// 9/3/2011

#include "qx/qxglobal.h"
#include <QtWidgets/QWidget>

QX_BEGIN_NAMESPACE

class QxDraggableWidgetPrivate;
class QxDraggableWidget : public QWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(QxDraggableWidget)
  QX_EXTEND_CLASS(QxDraggableWidget, QWidget)
  QX_DECLARE_PRIVATE(QxDraggableWidgetPrivate)

public:
  explicit QxDraggableWidget(QWidget *parent = nullptr, Qt::WindowFlags f = 0);
  ~QxDraggableWidget();

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

#endif // QXDRAGGABLEWIDGET_H
