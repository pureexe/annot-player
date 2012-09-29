#ifndef QXDRAGGABLEMAINWINDOW_H
#define QXDRAGGABLEMAINWINDOW_H

// qxdraggablemainwindow.h
// 9/3/2011

#include "qx/qxglobal.h"
#include <QtWidgets/QMainWindow>

QX_BEGIN_NAMESPACE

class QxDraggableMainWindowPrivate;
class QxDraggableMainWindow : public QMainWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(QxDraggableMainWindow)
  QX_EXTEND_CLASS(QxDraggableMainWindow, QMainWindow)
  QX_DECLARE_PRIVATE(QxDraggableMainWindowPrivate)

public:
  explicit QxDraggableMainWindow(QWidget *parent = nullptr, Qt::WindowFlags f = 0);
  ~QxDraggableMainWindow();

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

#endif // QXDRAGGABLEMAINWINDOW_H
