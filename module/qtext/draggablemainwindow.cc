#include <QDebug>
// qtext/draggablemainwindow.cc
// 9/3/2011

#include "module/qtext/draggablemainwindow.h"
#include <QtGui/QMouseEvent>

#define BAD_POS     QPoint(-1, -1)

// - Construction -

QtExt::
DraggableMainWindow::DraggableMainWindow(QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f), draggable_(true), dragPos_(BAD_POS)
{ }

// - Events -

void
QtExt::
DraggableMainWindow::mousePressEvent(QMouseEvent *event)
{
  if (draggable_ && event->button() == Qt::LeftButton
      && dragPos_ == BAD_POS) {
    dragPos_ = event->globalPos() - frameGeometry().topLeft();
    event->accept();
  }
  Base::mousePressEvent(event);
}

void
QtExt::
DraggableMainWindow::mouseMoveEvent(QMouseEvent *event)
{
  if (draggable_ && event->buttons() & Qt::LeftButton
      && dragPos_ != BAD_POS) {
    QPoint newPos = event->globalPos() - dragPos_;
    move(newPos);
    event->accept();
  }
  Base::mouseMoveEvent(event);
}

void
QtExt::
DraggableMainWindow::mouseReleaseEvent(QMouseEvent *event)
{
  if (draggable_) {
    dragPos_ = BAD_POS;
    event->accept();
  }
  Base::mouseReleaseEvent(event);
}

// EOF
