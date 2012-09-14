// doublespinbox.cc
// 8/21/2012

#include "lib/qtext/doublespinbox.h"
#include <QtGui/QKeyEvent>

// - Events -

void
QtExt::
DoubleSpinBox::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QtExt::
DoubleSpinBox::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

void
QtExt::
DoubleSpinBox::keyPressEvent(QKeyEvent *event)
{
  Base::keyPressEvent(event);
  if (event->key() == Qt::Key_Return)
    emit returnPressed();
}

// EOF
