// preferences.cc
// 5/24/2012

#include "preferences.h"
#include "preferences_p.h"
#include "annotationsettings.h"
#include <QtGui>

// - Constructions -

Preferences::Preferences(QWidget *parent)
  : Base(parent)
{
  addTab(new AnnotationPreferencesTab(AnnotationSettings::globalInstance(), this));
  finalize();
}

// EOF
