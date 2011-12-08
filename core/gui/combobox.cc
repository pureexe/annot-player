// core/gui/combobox.cc
// 10/7/2011

#include "core/gui/combobox.h"
#include <QtGui>

// - Constructions -

#define INITIALIZER \
  hover_(false), hoverCursor_(Qt::PointingHandCursor)

Core::Gui::
ComboBox::ComboBox(QWidget *parent)
  : Base(parent), INITIALIZER
{ }

#undef INITIALIZER

bool
Core::Gui::
ComboBox::isHovered() const
{ return hover_; }

QCursor
Core::Gui::
ComboBox::hoverCursor() const
{ return hoverCursor_; }

void
Core::Gui::
ComboBox::setHoverCursor(const QCursor &cursor)
{ hoverCursor_ = cursor; }

// - Events -

void
Core::Gui::
ComboBox::enterEvent(QEvent *event)
{
  Q_UNUSED(event);
  lastCursor_ = cursor();
  setCursor(hoverCursor());
  hover_ = true;
  //emit hovered();
}

void
Core::Gui::
ComboBox::leaveEvent(QEvent *event)
{
  Q_UNUSED(event);
  setCursor(lastCursor_);
  hover_ = false;
  //emit leaved();
}

// EOF
