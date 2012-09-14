#ifndef _QTEXT_DRAGGABLEMAINWINDOW_H
#define _QTEXT_DRAGGABLEMAINWINDOW_H

// qtext/draggablemainwindow.h
// 9/3/2011

#include <QtGui/QMainWindow>

namespace QtExt {

class DraggableMainWindow : public QMainWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(DraggableMainWindow)
  typedef DraggableMainWindow Self;
  typedef QMainWindow Base;

  bool draggable_;
  QPoint dragPos_;

public:
  explicit DraggableMainWindow(QWidget *parent = nullptr, Qt::WindowFlags f = 0);

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

} // namespace QtExt

#endif // _QTEXT_DRAGGABLEMAINWINDOW_H
