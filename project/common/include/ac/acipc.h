#ifndef _AC_IPC
#define _AC_IPC

// acplayer.h
// 4/9/2012
#ifdef WITH_MODULE_IPC
#  include "module/ipc/metacallpropagator.h"
#else
#  include <QObject>
#endif // WITH_MODULE_IPC

#include <QStringList>

#ifdef WITH_MODULE_IPC
typedef MetaCallPropagator AcIpcControllerBase;
#else
typedef QObject AcIpcControllerBase;
#endif // WITH_MODULE_IPC

class AcIpcController : public AcIpcControllerBase
{
  Q_OBJECT
  typedef AcIpcController Self;
  typedef AcIpcControllerBase Base;

  bool enabled_;

  // - Construction -
public:
  enum Role {
    NoRole = 0, // invalid
    Server,
    Client
  };

protected:
  explicit AcIpcController(Role role, QObject *parent = 0, uint id = 0);

signals:
  void message(const QString &text);
  void queuedMessage(const QString &text);

  void error(const QString &text);
  void queuedError(const QString &text);

  void warning(const QString &text);
  void queuedWarning(const QString &text);

  void notification(const QString &text);
  void queuedNotification(const QString &text);

  void arguments(const QStringList &args);
  void queuedArguments(const QStringList &args);

public:
  bool isEnabled() const { return enabled_; }
public slots:
  void setEnabled(bool t) { enabled_ = t; }

#ifndef WITH_MODULE_IPC
  void stop() { }
#endif // !WITH_MODULE_IPC

public:
  static bool isProcessRunning(const QString &processName);
  static bool open(const QString &app, const QStringList &args = QStringList());
};

#endif // _AC_IPC
