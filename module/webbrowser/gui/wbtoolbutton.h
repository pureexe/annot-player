#ifndef WBTOOLBUTTON_H
#define WBTOOLBUTTON_H

// gui/wbtoolbutton.h
// 4/25/2012

#include "module/qtext/toolbutton.h"

typedef QtExt::ToolButton WbToolButtonBase;
class WbToolButton : public WbToolButtonBase
{
  Q_OBJECT
  typedef WbToolButton Self;
  typedef WbToolButtonBase Base;

public:
  explicit WbToolButton(QWidget *parent = 0)
    : Base(parent) { }

};

#endif // WBTOOLBUTTON_H
