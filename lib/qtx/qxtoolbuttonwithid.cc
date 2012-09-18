// qxtoolbuttonwithid.cc
// 8/6/2011

#include "qtx/qxtoolbuttonwithid.h"

QTX_BEGIN_NAMESPACE

void
QxToolButtonWithId::init()
{
  connect(this, SIGNAL(clicked(bool)), SLOT(emit_clickedWithId(bool)));
  connect(this, SIGNAL(pressed()), SLOT(emit_pressedWithId()));
  connect(this, SIGNAL(released()), SLOT(emit_releasedWithId()));
  connect(this, SIGNAL(toggled(bool)), SLOT(emit_toggledWithId(bool)));
}

QTX_END_NAMESPACE
