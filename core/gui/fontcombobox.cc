// core/gui/fontcombobox.cc
// 10/11/2011

#include "core/gui/fontcombobox.h"
#include <QtGui>

// - Constructions -

#define INITIALIZER \
  hover_(false), hoverCursor_(Qt::PointingHandCursor)

Core::Gui::
FontComboBox::FontComboBox(QWidget *parent)
  : Base(parent), INITIALIZER
{ }

#undef INITIALIZER

bool
Core::Gui::
FontComboBox::isHovered() const
{ return hover_; }

QCursor
Core::Gui::
FontComboBox::hoverCursor() const
{ return hoverCursor_; }

void
Core::Gui::
FontComboBox::setHoverCursor(const QCursor &cursor)
{ hoverCursor_ = cursor; }

// - Events -

void
Core::Gui::
FontComboBox::enterEvent(QEvent *event)
{
  Q_UNUSED(event);
  lastCursor_ = cursor();
  setCursor(hoverCursor());
  hover_ = true;
  //emit hovered();
}

void
Core::Gui::
FontComboBox::leaveEvent(QEvent *event)
{
  Q_UNUSED(event);
  setCursor(lastCursor_);
  hover_ = false;
  //emit leaved();
}

// EOF
