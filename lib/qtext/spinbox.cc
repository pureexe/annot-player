// spinbox.cc
// 5/1/2012

#include "lib/qtext/spinbox.h"
#include <QtGui/QKeyEvent>

// - Events -

void
QtExt::
SpinBox::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QtExt::
SpinBox::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

void
QtExt::
SpinBox::keyPressEvent(QKeyEvent *event)
{
  Base::keyPressEvent(event);
  if (event->key() == Qt::Key_Return)
    emit returnPressed();
}

// EOF
