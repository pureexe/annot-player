// qtext/dialog.cc
// 9/3/2011

#include "lib/qtext/dialog.h"

// - Constructions -

#define WINDOW_FLAGS \
  (Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint)

QtExt::
Dialog::Dialog(QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f ? f : WINDOW_FLAGS)
{ }

// EOF
