// qtext/combobox.cc
// 10/7/2011

#include "module/qtext/combobox.h"
#include <QtGui/QLineEdit>

// - Construction -

QtExt::
ComboBox::ComboBox(QWidget *parent)
  : Base(parent), hovered_(false), hoverCursor_(Qt::PointingHandCursor)
{
  setInsertPolicy(InsertAtTop);
  connect(this, SIGNAL(activated(int)), SLOT(hidePopup()));
}

// - Events -

void
QtExt::
ComboBox::enterEvent(QEvent *event)
{
  Q_UNUSED(event);
  lastCursor_ = cursor();
  setCursor(hoverCursor());
  hovered_ = true;
  emit hovered();
  Base::enterEvent(event);
}

void
QtExt::
ComboBox::leaveEvent(QEvent *event)
{
  setCursor(lastCursor_);
  hovered_ = false;
  emit leaved();
  Base::leaveEvent(event);
}

// EOF
