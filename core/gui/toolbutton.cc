// core/gui/toolbutton.cc
// 8/6/2011

#include "core/gui/toolbutton.h"
#include <QtGui>

// - Constructions -

#define TOOLBUTTON_INIT \
  hover_(false), hoverCursor_(Qt::PointingHandCursor)

Core::Gui::
ToolButton::ToolButton(QWidget *parent)
  : Base(parent), TOOLBUTTON_INIT
{ }

#undef TOOLBUTTON_INIT

bool
Core::Gui::
ToolButton::isHovered() const
{ return hover_; }

QCursor
Core::Gui::
ToolButton::hoverCursor() const
{ return hoverCursor_; }

void
Core::Gui::
ToolButton::setHoverCursor(const QCursor &cursor)
{ hoverCursor_ = cursor; }

// - Events -

void
Core::Gui::
ToolButton::enterEvent(QEvent *event)
{
  Q_UNUSED(event);
  lastCursor_ = cursor();
  setCursor(hoverCursor());
  hover_ = true;
  //emit hovered();
}

void
Core::Gui::
ToolButton::leaveEvent(QEvent *event)
{
  Q_UNUSED(event);
  setCursor(lastCursor_);
  hover_ = false;
  //emit leaved();
}

// EOF
