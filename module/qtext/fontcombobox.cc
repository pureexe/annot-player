// qtext/fontcombobox.cc
// 10/11/2011

#include "module/qtext/fontcombobox.h"

// - Events -

void
QtExt::
FontComboBox::enterEvent(QEvent *event)
{
  lastCursor_ = cursor();
  setCursor(hoverCursor());
  hovered_ = true;
  emit hovered();
  Base::enterEvent(event);
}

void
QtExt::
FontComboBox::leaveEvent(QEvent *event)
{
  Q_UNUSED(event);
  setCursor(lastCursor_);
  hovered_ = false;
  emit leaved();
  Base::leaveEvent(event);
}

// EOF
