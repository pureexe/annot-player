// qxspinbox.cc
// 5/1/2012

#include "qtx/qxspinbox.h"
#include <QtGui/QKeyEvent>

QTX_BEGIN_NAMESPACE

// - Events -

void
QxSpinBox::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QxSpinBox::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

void
QxSpinBox::keyPressEvent(QKeyEvent *event)
{
  Base::keyPressEvent(event);
  if (event->key() == Qt::Key_Return)
    emit returnPressed();
}

QTX_END_NAMESPACE
