#ifndef TRAY_H
#define TRAY_H

// tray.h
// 10/27/2011

#include <QSystemTrayIcon>

QT_FORWARD_DECLARE_CLASS(QAction)

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
protected slots:
  void activate(QSystemTrayIcon::ActivationReason reason);
  void invalidateContextMenu();
private:
  void createActions();

private:
  MainWindow *w_;

  QAction *minimizeAct_,
          *restoreAct_;
};

#endif // TRAY_H
