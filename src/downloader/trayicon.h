#ifndef TRAYICON_H
#define TRAYICON_H

// trayicon.h
// 10/27/2011

#include "qtx/qxsystemtrayicon.h"

QT_FORWARD_DECLARE_CLASS(QAction)

class MainWindow;

typedef QxSystemTrayIcon TrayIconBase;
class TrayIcon : public TrayIconBase
{
  Q_OBJECT
  Q_DISABLE_COPY(TrayIcon)
  typedef TrayIcon Self;
  typedef TrayIconBase Base;

  // - Construction -
public:
  explicit TrayIcon(MainWindow *w, QObject *parent = nullptr);

public slots:
  void setToolTip(const QString &text) { Base::setToolTip(text); }

  // - Implementation -
protected slots:
  void activate(QSystemTrayIcon::ActivationReason reason);
  void updateContextMenu();
private:
  void createActions();

private:
  MainWindow *w_;
  QAction *showAct_, *hideAct_;
};

#endif // SYSTEMTRAYICON_H
