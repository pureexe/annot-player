// qtext/fontcombobox.cc
// 10/11/2011

#include "module/qtext/fontcombobox.h"
#include <QtGui>

// - Constructions -

#define INITIALIZER \
  hover_(false), hoverCursor_(Qt::PointingHandCursor)

QtExt::
FontComboBox::FontComboBox(QWidget *parent)
  : Base(parent), INITIALIZER
{ }

#undef INITIALIZER

bool
QtExt::
FontComboBox::isHovered() const
{ return hover_; }

QCursor
QtExt::
FontComboBox::hoverCursor() const
{ return hoverCursor_; }

void
QtExt::
FontComboBox::setHoverCursor(const QCursor &cursor)
{ hoverCursor_ = cursor; }

// - Events -

void
QtExt::
FontComboBox::enterEvent(QEvent *event)
{
  Q_UNUSED(event);
  lastCursor_ = cursor();
  setCursor(hoverCursor());
  hover_ = true;
  //emit hovered();
}

void
QtExt::
FontComboBox::leaveEvent(QEvent *event)
{
  Q_UNUSED(event);
  setCursor(lastCursor_);
  hover_ = false;
  //emit leaved();
}

// EOF
