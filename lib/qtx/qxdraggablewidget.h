#ifndef QXDRAGGABLEWIDGET_H
#define QXDRAGGABLEWIDGET_H

// qxdraggablewidget.h
// 9/3/2011

#include "qtx/qtxglobal.h"
#include <QtGui/QWidget>

QTX_BEGIN_NAMESPACE

class QxDraggableWidget : public QWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(QxDraggableWidget)
  typedef QxDraggableWidget Self;
  typedef QWidget Base;

  bool draggable_;
  QPoint dragPos_;

public:
  explicit QxDraggableWidget(QWidget *parent = nullptr, Qt::WindowFlags f = 0)
    : Base(parent, f), draggable_(true), dragPos_(QTX_OFFSCREEN_POS) { }

  // - Properties -
public:
  bool isDraggable() const { return draggable_; }
public slots:
  void setDraggable(bool t) { draggable_ = t; }

  // - Events -
protected:
  //void setVisible(bool visible) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
};

QTX_END_NAMESPACE

#endif // QXDRAGGABLEWIDGET_H
