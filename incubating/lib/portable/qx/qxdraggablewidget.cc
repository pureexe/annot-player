// qxdraggablewidget.cc
// 9/22/2012

#include "qx/qxdraggablewidget.h"
#include <QtGui/QMouseEvent>

QX_BEGIN_NAMESPACE

// - Private Data -

class QxDraggableWidgetPrivate
{
public:

  bool draggable;
  QPoint dragPos;

  QxDraggableWidgetPrivate()
    : draggable(true), dragPos(QX_OFFSCREEN_POS) { }
};

// - Construction -

QxDraggableWidget::QxDraggableWidget(QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f), d(new D)
{ }

QxDraggableWidget::~QxDraggableWidget()
{ delete d; }

bool
QxDraggableWidget::isDraggable() const
{ return d->draggable; }

void
QxDraggableWidget::setDraggable(bool value)
{ d->draggable = value; }

// - Events -

void
QxDraggableWidget::mousePressEvent(QMouseEvent *event)
{
  if (d->draggable && !isMaximized() && !isFullScreen() &&
      event->button() == Qt::LeftButton && d->dragPos == QX_OFFSCREEN_POS) {
    d->dragPos = event->globalPos() - frameGeometry().topLeft();
    event->accept();
  }
  Base::mousePressEvent(event);
}

void
QxDraggableWidget::mouseMoveEvent(QMouseEvent *event)
{
  if (d->draggable && !isMaximized() && !isFullScreen() &&
      (event->buttons() & Qt::LeftButton) && d->dragPos != QX_OFFSCREEN_POS) {
    move(event->globalPos() - d->dragPos);
    event->accept();
  }
  Base::mouseMoveEvent(event);
}

void
QxDraggableWidget::mouseReleaseEvent(QMouseEvent *event)
{
  d->dragPos = QX_OFFSCREEN_POS;
  Base::mouseReleaseEvent(event);
}

QX_END_NAMESPACE
