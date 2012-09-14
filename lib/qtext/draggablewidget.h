#ifndef _QTEXT_DRAGGABLEWIDGET_H
#define _QTEXT_DRAGGABLEWIDGET_H

// qtext/draggablewidget.h
// 9/3/2011

#include <QtGui/QWidget>

namespace QtExt {

class DraggableWidget : public QWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(DraggableWidget)
  typedef DraggableWidget Self;
  typedef QWidget Base;

  bool draggable_;
  QPoint dragPos_;

public:
  explicit DraggableWidget(QWidget *parent = nullptr, Qt::WindowFlags f = 0);

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

#endif // _QTEXT_DRAGGABLEWIDGET_H
