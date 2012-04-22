#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

// tray.h
// 10/27/2011

#include <QtGui/QSystemTrayIcon>

class MainWindow;

class SystemTrayIcon : public QSystemTrayIcon
{
  Q_OBJECT
  typedef SystemTrayIcon Self;
  typedef QSystemTrayIcon Base;

  // - Construction -
public:
  explicit SystemTrayIcon(MainWindow *w, QObject *parent = 0);

public slots:
  void setToolTip(const QString &text) { Base::setToolTip(text); }

  // - Implementation -
protected slots:
  void activate(QSystemTrayIcon::ActivationReason reason);
  void invalidateContextMenu();
private:
  void createActions();

private:
  MainWindow *w_;
};

#endif // SYSTEMTRAYICON_H
