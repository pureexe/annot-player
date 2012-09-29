// qxdraggablewidget.cc
// 9/22/2012

#include "qx/qxdraggabledialog.h"
#include <QtGui/QMouseEvent>

QX_BEGIN_NAMESPACE

// - Private Data -

class QxDraggableDialogPrivate
{
public:

  bool draggable;
  QPoint dragPos;

  QxDraggableDialogPrivate()
    : draggable(true), dragPos(QX_OFFSCREEN_POS) { }
};

// - Construction -

QxDraggableDialog::QxDraggableDialog(QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f), d(new D)
{ }

QxDraggableDialog::~QxDraggableDialog()
{ delete d; }

bool
QxDraggableDialog::isDraggable() const
{ return d->draggable; }

void
QxDraggableDialog::setDraggable(bool value)
{ d->draggable = value; }

// - Events -

void
QxDraggableDialog::mousePressEvent(QMouseEvent *event)
{
  if (d->draggable && !isMaximized() && !isFullScreen() &&
      event->button() == Qt::LeftButton && d->dragPos == QX_OFFSCREEN_POS) {
    d->dragPos = event->globalPos() - frameGeometry().topLeft();
    event->accept();
  }
  Base::mousePressEvent(event);
}

void
QxDraggableDialog::mouseMoveEvent(QMouseEvent *event)
{
  if (d->draggable && !isMaximized() && !isFullScreen() &&
      (event->buttons() & Qt::LeftButton) && d->dragPos != QX_OFFSCREEN_POS) {
    move(event->globalPos() - d->dragPos);
    event->accept();
  }
  Base::mouseMoveEvent(event);
}

void
QxDraggableDialog::mouseReleaseEvent(QMouseEvent *event)
{
  d->dragPos = QX_OFFSCREEN_POS;
  Base::mouseReleaseEvent(event);
}

QX_END_NAMESPACE
