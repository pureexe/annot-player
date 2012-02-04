// qtext/combobox.cc
// 10/7/2011

#include "module/qtext/combobox.h"
#include <QtGui>

// - Constructions -

#define INITIALIZER \
  hover_(false), hoverCursor_(Qt::PointingHandCursor)

QtExt::
ComboBox::ComboBox(QWidget *parent)
  : Base(parent), INITIALIZER
{ }

#undef INITIALIZER

bool
QtExt::
ComboBox::isHovered() const
{ return hover_; }

QCursor
QtExt::
ComboBox::hoverCursor() const
{ return hoverCursor_; }

void
QtExt::
ComboBox::setHoverCursor(const QCursor &cursor)
{ hoverCursor_ = cursor; }

// - Events -

void
QtExt::
ComboBox::enterEvent(QEvent *event)
{
  Q_UNUSED(event);
  lastCursor_ = cursor();
  setCursor(hoverCursor());
  hover_ = true;
  //emit hovered();
}

void
QtExt::
ComboBox::leaveEvent(QEvent *event)
{
  Q_UNUSED(event);
  setCursor(lastCursor_);
  hover_ = false;
  //emit leaved();
}

// EOF
