// qtext/fontcombobox.cc
// 10/11/2011

#include "module/qtext/fontcombobox.h"

// - Events -

void
QtExt::
FontComboBox::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QtExt::
FontComboBox::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

// EOF
