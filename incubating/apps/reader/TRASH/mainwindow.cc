// mainwindow.cc
// 9/22/2012 jichi

#include "mainwindow.h"

#define DEBUG "mainwindow"
#include "qx/qxdebug.h"

// - Construction -

MainWindow::MainWindow()
{
  // Force create a valid hwnd
  winId();
  //resize(QSize());
  //setWindowFlags(Qt::FramelessWindowHint);
}

// EOF
