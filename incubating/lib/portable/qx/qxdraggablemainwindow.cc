// qxdraggablewidget.cc
// 9/22/2012

#include "qx/qxdraggablemainwindow.h"
#include <QtGui/QMouseEvent>

QX_BEGIN_NAMESPACE

// - Private Data -

class QxDraggableMainWindowPrivate
{
public:

  bool draggable;
  QPoint dragPos;

  QxDraggableMainWindowPrivate()
    : draggable(true), dragPos(QX_OFFSCREEN_POS) { }
};

// - Construction -

QxDraggableMainWindow::QxDraggableMainWindow(QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f), d(new D)
{ }

QxDraggableMainWindow::~QxDraggableMainWindow()
{ delete d; }

bool
QxDraggableMainWindow::isDraggable() const
{ return d->draggable; }

void
QxDraggableMainWindow::setDraggable(bool value)
{ d->draggable = value; }

// - Events -

void
QxDraggableMainWindow::mousePressEvent(QMouseEvent *event)
{
  if (d->draggable && !isMaximized() && !isFullScreen() &&
      event->button() == Qt::LeftButton && d->dragPos == QX_OFFSCREEN_POS) {
    d->dragPos = event->globalPos() - frameGeometry().topLeft();
    event->accept();
  }
  Base::mousePressEvent(event);
}

void
QxDraggableMainWindow::mouseMoveEvent(QMouseEvent *event)
{
  if (d->draggable && !isMaximized() && !isFullScreen() &&
      (event->buttons() & Qt::LeftButton) && d->dragPos != QX_OFFSCREEN_POS) {
    move(event->globalPos() - d->dragPos);
    event->accept();
  }
  Base::mouseMoveEvent(event);
}

void
QxDraggableMainWindow::mouseReleaseEvent(QMouseEvent *event)
{
  d->dragPos = QX_OFFSCREEN_POS;
  Base::mouseReleaseEvent(event);
}

QX_END_NAMESPACE
