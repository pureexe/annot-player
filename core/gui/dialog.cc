// core/gui/dialog.cc
// 9/3/2011

#include "core/gui/dialog.h"
#include <QtGui>

#define BAD_POS     QPoint(-1, -1)

// - Constructions -

Core::Gui::
Dialog::Dialog(QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f),
    draggable_(true), dragPos_(BAD_POS)
{ }

bool
Core::Gui::
Dialog::draggable() const
{ return draggable_; }

void
Core::Gui::
Dialog::setDraggable(bool draggable)
{ draggable_ = draggable; }

// - Events -

void
Core::Gui::
Dialog::mousePressEvent(QMouseEvent *event)
{
  if (!draggable_) {
    Base::mousePressEvent(event);
    return;
  }

  if (event && event->button() == Qt::LeftButton
      && dragPos_ == BAD_POS) {
    dragPos_ = event->globalPos() - frameGeometry().topLeft();
    event->accept();
  }
}

void
Core::Gui::
Dialog::mouseMoveEvent(QMouseEvent *event)
{
  if (!draggable_) {
    Base::mouseMoveEvent(event);
    return;
  }

  if (event && event->buttons() & Qt::LeftButton
      && dragPos_ != BAD_POS) {
    QPoint newPos = event->globalPos() - dragPos_;
    move(newPos);
    event->accept();
  }
}

void
Core::Gui::
Dialog::mouseReleaseEvent(QMouseEvent *event)
{
  if (!draggable_) {
    Base::mouseReleaseEvent(event);
    return;
  }

  dragPos_ = BAD_POS;
  if (event)
    event->accept();
}

// EOF
