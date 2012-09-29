#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// mainwindow.h
// 9/22/2012 jichi

#include "qx/qxglobal.h"
#include <QtGui/QWindow>

class MainWindow : public QWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(MainWindow)
  QX_EXTEND_CLASS(MainWindow, QWidget)

public:
  MainWindow();
};

#endif // MAINWINDOW_H
