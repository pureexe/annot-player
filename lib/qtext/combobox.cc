// qtext/combobox.cc
// 10/7/2011

#include "lib/qtext/combobox.h"

// - Construction -

QtExt::
ComboBox::ComboBox(QWidget *parent)
  : Base(parent)
{
  setInsertPolicy(InsertAtTop);
  connect(this, SIGNAL(activated(int)), SLOT(hidePopup()));
}

// - Events -

void
QtExt::
ComboBox::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QtExt::
ComboBox::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

// EOF
