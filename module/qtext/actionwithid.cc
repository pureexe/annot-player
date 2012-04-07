// qtext/actionwithid.cc
// 9/2/2011

#include "module/qtext/actionwithid.h"

void
QtExt::
ActionWithId::init()
{
  connect(this, SIGNAL(changed()), SLOT(emit_changedWithId()));
  connect(this, SIGNAL(hovered()), SLOT(emit_hoveredWithId()));
  connect(this, SIGNAL(toggled(bool)), SLOT(emit_toggledWithId(bool)));
  connect(this, SIGNAL(triggered(bool)), SLOT(emit_triggeredWithId(bool)));
}

// EOF
