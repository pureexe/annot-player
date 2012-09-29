// qxdraggablewidget.cc
// 9/22/2012

#include "qx/qxdraggablequickview.h"
#include <QtGui/QMouseEvent>

QX_BEGIN_NAMESPACE

// - Private Data -

class QxDraggableQuickViewPrivate
{
public:

  bool draggable;
  QPoint dragPos;

  QxDraggableQuickViewPrivate()
    : draggable(true), dragPos(QX_OFFSCREEN_POS) { }
};

// - Construction -

QxDraggableQuickView::QxDraggableQuickView(QWindow *parent)
  : Base(parent), d(new D)
{ }

QxDraggableQuickView::QxDraggableQuickView(QQmlEngine *engine, QWindow *parent)
  : Base(engine, parent), d(new D)
{ }

QxDraggableQuickView::QxDraggableQuickView(const QUrl &source, QWindow *parent)
  : Base(source, parent), d(new D)
{ }

QxDraggableQuickView::~QxDraggableQuickView()
{ delete d; }

bool
QxDraggableQuickView::isDraggable() const
{ return d->draggable; }

void
QxDraggableQuickView::setDraggable(bool value)
{ d->draggable = value; }

// - Events -

void
QxDraggableQuickView::mousePressEvent(QMouseEvent *event)
{
  if (d->draggable &&
      !(windowFlags() & (Qt::WindowMaximized | Qt::WindowFullScreen)) &&
      event->button() == Qt::LeftButton && d->dragPos == QX_OFFSCREEN_POS) {
    d->dragPos = event->globalPos() - frameGeometry().topLeft();
    if (!(windowFlags() & Qt::FramelessWindowHint) &&
        windowFlags() != Qt::Popup) // expect to have problem, such as combination with WindowStaysOnTopHint
      d->dragPos -= QPoint(8, 30); // x: frame border width, y: frame header height
    event->accept();
  }
  Base::mousePressEvent(event);
}

void
QxDraggableQuickView::mouseMoveEvent(QMouseEvent *event)
{
  if (d->draggable &&
      !(windowFlags() & (Qt::WindowMaximized | Qt::WindowFullScreen)) &&
      (event->buttons() & Qt::LeftButton) && d->dragPos != QX_OFFSCREEN_POS) {
    setPos(event->globalPos() - d->dragPos);
    event->accept();
  }
  Base::mouseMoveEvent(event);
}

void
QxDraggableQuickView::mouseReleaseEvent(QMouseEvent *event)
{
  d->dragPos = QX_OFFSCREEN_POS;
  Base::mouseReleaseEvent(event);
}

QX_END_NAMESPACE
