// qxdoublespinbox.cc
// 8/21/2012

#include "qtx/qxdoublespinbox.h"
#include <QtGui/QKeyEvent>

QTX_BEGIN_NAMESPACE

// - Events -

void
QxDoubleSpinBox::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QxDoubleSpinBox::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

void
QxDoubleSpinBox::keyPressEvent(QKeyEvent *event)
{
  Base::keyPressEvent(event);
  if (event->key() == Qt::Key_Return)
    emit returnPressed();
}

QTX_END_NAMESPACE
