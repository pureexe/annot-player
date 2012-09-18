// qxdraggablemainwindow.cc
// 9/3/2011

#include "qtx/qxdraggablemainwindow.h"
#include <QtGui/QMouseEvent>

QTX_BEGIN_NAMESPACE

// - Events -

void
QxDraggableMainWindow::mousePressEvent(QMouseEvent *event)
{
  if (draggable_ && !isMaximized() && !isFullScreen() &&
      event->button() == Qt::LeftButton && dragPos_ == QTX_OFFSCREEN_POS) {
    dragPos_ = event->globalPos() - frameGeometry().topLeft();
    event->accept();
  }
  Base::mousePressEvent(event);
}

void
QxDraggableMainWindow::mouseMoveEvent(QMouseEvent *event)
{
  if (draggable_ && !isMaximized() && !isFullScreen() &&
      (event->buttons() & Qt::LeftButton) && dragPos_ != QTX_OFFSCREEN_POS) {
    QPoint newPos = event->globalPos() - dragPos_;
    move(newPos);
    event->accept();
  }
  Base::mouseMoveEvent(event);
}

void
QxDraggableMainWindow::mouseReleaseEvent(QMouseEvent *event)
{
  dragPos_ = QTX_OFFSCREEN_POS;
  Base::mouseReleaseEvent(event);
}

QTX_END_NAMESPACE
