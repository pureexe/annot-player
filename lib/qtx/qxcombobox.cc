// qxcombobox.cc
// 10/7/2011

#include "qtx/qxcombobox.h"

QTX_BEGIN_NAMESPACE

// - Construction -

QxComboBox::QxComboBox(QWidget *parent)
  : Base(parent)
{
  setInsertPolicy(InsertAtTop);
  connect(this, SIGNAL(activated(int)), SLOT(hidePopup()));
}

// - Events -

void
QxComboBox::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QxComboBox::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

QTX_END_NAMESPACE

// EOF
