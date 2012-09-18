#ifndef QXDRAGGABLEMAINWINDOW_H
#define QXDRAGGABLEMAINWINDOW_H

// qxdraggablemainwindow.h
// 9/3/2011

#include "qtx/qtxglobal.h"
#include <QtGui/QMainWindow>

QTX_BEGIN_NAMESPACE

class QxDraggableMainWindow : public QMainWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(QxDraggableMainWindow)
  typedef QxDraggableMainWindow Self;
  typedef QMainWindow Base;

  bool draggable_;
  QPoint dragPos_;

public:
  explicit QxDraggableMainWindow(QWidget *parent = nullptr, Qt::WindowFlags f = 0)
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

#endif // QXDRAGGABLEMAINWINDOW_H
