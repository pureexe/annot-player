#ifndef TRAY_H
#define TRAY_H

// tray.h
// 10/27/2011

#include "module/qtext/systemtrayicon.h"

QT_FORWARD_DECLARE_CLASS(QAction)

class MainWindow;

typedef QtExt::SystemTrayIcon TrayBase;
class Tray : public TrayBase
{
  Q_OBJECT
  typedef Tray Self;
  typedef TrayBase Base;

  // - Constructions -
public:
  explicit Tray(MainWindow *w, QObject *parent = 0);

  // - Implementations -
protected slots:
  void activate(QSystemTrayIcon::ActivationReason reason);
  void updateContextMenu();
private:
  void createActions();

private:
  MainWindow *w_;

  QAction *minimizeAct_,
          *restoreAct_,
          *toggleWindowOnTopAct_;
};

#endif // TRAY_H
