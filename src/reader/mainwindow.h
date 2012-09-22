#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// mainwindow.h
// 9/21/2012

#include <QtGui/QMainWindow>

///  Invisible background main window
class MainWindow : public QMainWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(MainWindow)
  typedef MainWindow Self;
  typedef QMainWindow Base;

public:
  explicit MainWindow(QWidget *parent = nullptr);
};

#endif // MAINWINDOW_H
