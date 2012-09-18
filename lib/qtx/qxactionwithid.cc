// qxactionwithid.cc
// 9/2/2011

#include "qtx/qxactionwithid.h"

QTX_BEGIN_NAMESPACE

void
QxActionWithId::init()
{
  connect(this, SIGNAL(changed()), SLOT(emit_changedWithId()));
  connect(this, SIGNAL(hovered()), SLOT(emit_hoveredWithId()));
  connect(this, SIGNAL(toggled(bool)), SLOT(emit_toggledWithId(bool)));
  connect(this, SIGNAL(triggered(bool)), SLOT(emit_triggeredWithId(bool)));
}

QTX_END_NAMESPACE
