#ifndef TRAY_H
#define TRAY_H

// tray.h
// 10/27/2011

#include <QSystemTrayIcon>

class MainWindow;

class Tray : public QSystemTrayIcon
{
  Q_OBJECT
  typedef Tray Self;
  typedef QSystemTrayIcon Base;

  // - Constructions -
public:
  explicit Tray(MainWindow *w, QObject *parent = 0);

  // - Implementations -
private:
  void createActions();

private:
  MainWindow *w_;
};

#endif // TRAY_H
