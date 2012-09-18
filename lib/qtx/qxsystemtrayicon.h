#ifndef QXSYSTEMTRAY_H
#define QXSYSTEMTRAY_H

// qxsystemray.h
// 4/22/2012

#include "qtx/qtxglobal.h"
#include <QtGui/QSystemTrayIcon>

QTX_BEGIN_NAMESPACE

class QxSystemTrayIcon : public QSystemTrayIcon
{
  Q_OBJECT
  Q_DISABLE_COPY(QxSystemTrayIcon)
  typedef QxSystemTrayIcon Self;
  typedef QSystemTrayIcon Base;

  // - Constructions -
public:
  explicit QxSystemTrayIcon(QObject *parent = nullptr)
    : Base(parent) { }
  explicit QxSystemTrayIcon(const QIcon &icon, QObject *parent = nullptr)
    : Base(icon, parent) { }

public slots:
  void setToolTip(const QString &text) { Base::setToolTip(text); }

  void showMessage (const QString &title, const QString &message)
  { Base::showMessage(title, message); }
};

QTX_END_NAMESPACE

#endif // QXSYSTEMTRAYICON_H
