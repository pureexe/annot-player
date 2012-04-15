// acipc.cc
// 4/9/2012

#include "ac/acipc.h"
#ifdef Q_WS_WIN
#  include "win/qtwin/qtwin.h"
#endif // Q_WS_WIN
#ifdef Q_WS_MAC
#  include "mac/qtmac/qtmac.h"
#endif // Q_WS_MAC
#include <QtCore>
#include <QHostAddress>

// - Construction -

#ifdef WITH_MODULE_IPC

#define IPC_HOST        "127.0.0.1"

#define IPC_PORT_START  6002
#define IPC_PORT_STOP   6880
#define IPC_PORT(_key)  (IPC_PORT_START + (_key) % (IPC_PORT_STOP - IPC_PORT_START))

AcIpcController::AcIpcController(Role role, QObject *parent, uint id)
  : Base(parent)
{
  switch (role) {
  case Client: enabled_ = startClient(QHostAddress(IPC_HOST), IPC_PORT(id)); break;
  case Server: enabled_ = startServer(QHostAddress(IPC_HOST), IPC_PORT(id)); break;
  default: enabled_ = false;
  }

  connect(this, SIGNAL(queuedMessage(QString)), SIGNAL(message(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(queuedError(QString)), SIGNAL(error(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(queuedWarning(QString)), SIGNAL(warning(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(queuedNotification(QString)), SIGNAL(notification(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(queuedArguments(QStringList)), SIGNAL(arguments(QStringList)), Qt::QueuedConnection);
}

#else // WITH_MODULE_IPC

AcIpcController::AcIpcController(Role role, QObject *parent, uint id)
  : Base(parent), enabled_(true)
{
  Q_UNUSED(role);
  Q_UNUSED(id);
}

#endif // WITH_MODULE_IPC

// - Helpers -

bool
AcIpcController::isProcessRunning(const QString &processName)
{
#ifdef Q_WS_WIN
  return QtWin::getProcessIdByName(processName);
#else
  // TODO
  Q_UNUSED(processName);
  return false;
#endif // Q_WS_MAC
}

bool
AcIpcController::open(const QString &app, const QStringList &args)
{
#ifdef Q_WS_MAC
  return QtMac::open(app, args);
#else
  return QProcess::startDetached(app, args);
#endif // Q_WS_MAC
}

// EOF
