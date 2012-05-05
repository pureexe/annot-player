// acwindow.cc
// 5/5/2012

#include "project/common/acwindow.h"
#include "project/common/acui.h"

// - Constructions -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowStaysOnTopHint | \
  Qt::WindowCloseButtonHint )

AcWindow::AcWindow(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{ AcUi::globalInstance()->setWindowStyle(this); }

// EOF
