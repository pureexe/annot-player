// preferences.cc
// 5/24/2012

#include "preferences.h"
#include "preferences_p.h"

// - Constructions -

Preferences::Preferences(QWidget *parent)
  : Base(parent, false)
{
  setTabs(NetworkProxyTab);
  addTab(new GesturePreferencesTab(this));
  finalize();
}

// EOF
