// qtext/toolbutton.cc
// 8/6/2011

#include "module/qtext/toolbutton.h"
#include <QtGui>

// - Constructions -

#define TOOLBUTTON_INIT \
  hover_(false), hoverCursor_(Qt::PointingHandCursor)

QtExt::
ToolButton::ToolButton(QWidget *parent)
  : Base(parent), TOOLBUTTON_INIT
{ }

#undef TOOLBUTTON_INIT

bool
QtExt::
ToolButton::isHovered() const
{ return hover_; }

QCursor
QtExt::
ToolButton::hoverCursor() const
{ return hoverCursor_; }

void
QtExt::
ToolButton::setHoverCursor(const QCursor &cursor)
{ hoverCursor_ = cursor; }

// - Events -

void
QtExt::
ToolButton::enterEvent(QEvent *event)
{
  Q_UNUSED(event);
  lastCursor_ = cursor();
  setCursor(hoverCursor());
  hover_ = true;
  //emit hovered();
}

void
QtExt::
ToolButton::leaveEvent(QEvent *event)
{
  Q_UNUSED(event);
  setCursor(lastCursor_);
  hover_ = false;
  //emit leaved();
}

// EOF
