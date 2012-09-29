// mainwidget.cc
// 9/22/2012 jichi

#include "mainwidget.h"

#define DEBUG "mainwidget"
#include "qx/qxdebug.h"

// - Construction -

MainWidget::MainWidget()
{
  // Force create a valid hwnd
  winId();
  //resize(QSize());
  //setWindowFlags(Qt::FramelessWindowHint);
}

// EOF
