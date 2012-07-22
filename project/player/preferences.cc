// preferences.cc
// 5/24/2012

#include "preferences.h"
#include "annotationprefs.h"
#include "associationprefs.h"

// - Constructions -

Preferences::Preferences(QWidget *parent)
  : Base(parent, false)
{
#ifdef Q_OS_WIN
  addTab(new AssociationPreferencesTab(this));
#endif // Q_OS_WIN
  addTab(new AnnotationPreferencesTab(this));
  finalize();
}

// EOF
