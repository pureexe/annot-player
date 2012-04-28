#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QCheckBox;

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  MainWindow();

public slots:
  void clearAll();
  void setAll();

  void noMatch();
  void secretMessage();

private:
  QCheckBox *cb1;
  QCheckBox *cb2;
  QCheckBox *cb3;
  QCheckBox *cb4;
  QCheckBox *cb5;
};

#endif // MAINWINDOW_H
