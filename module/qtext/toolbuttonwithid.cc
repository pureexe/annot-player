// qtext/toolbuttonwithid.cc
// 8/6/2011

#include "module/qtext/toolbuttonwithid.h"

// - Construction -

QtExt::
ToolButtonWithId::ToolButtonWithId(int id, QWidget *parent)
  : Base(parent), id_(id)
{
  connect(this, SIGNAL(clicked(bool)), SLOT(emit_clickedWithId(bool)));
  connect(this, SIGNAL(pressed()), SLOT(emit_pressedWithId()));
  connect(this, SIGNAL(released()), SLOT(emit_releasedWithId()));
  connect(this, SIGNAL(toggled(bool)), SLOT(emit_toggledWithId(bool)));
}

// EOF
