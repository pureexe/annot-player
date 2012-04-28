#ifndef _QTEXT_SYSTEMTRAY_H
#define _QTEXT_SYSTEMTRAY_H

// qtext/systemray.h
// 4/22/2012

#include <QtGui/QSystemTrayIcon>

namespace QtExt {

class SystemTrayIcon : public QSystemTrayIcon
{
  Q_OBJECT
  typedef SystemTrayIcon Self;
  typedef QSystemTrayIcon Base;

  // - Constructions -
public:
  explicit SystemTrayIcon(QObject *parent = 0)
    : Base(parent) { }
  explicit SystemTrayIcon(const QIcon &icon, QObject *parent = 0)
    : Base(icon, parent) { }

public slots:
  void setToolTip(const QString &text) { Base::setToolTip(text); }

  void showMessage (const QString &title, const QString &message)
  { Base::showMessage(title, message); }
};

} // namespace QtExt

#endif // QTEXT_SYSTEMTRAYICON_H
