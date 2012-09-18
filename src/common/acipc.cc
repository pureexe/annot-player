// acipc.cc
// 4/9/2012

#include "src/common/acipc.h"
#ifdef WITH_LIB_METACALL
# include "lib/metacall/metacallfilter.h"
#endif // WITH_LIB_METACALL
#ifdef Q_OS_WIN
# include "win/qtwin/qtwin.h"
#endif // Q_OS_WIN
#ifdef Q_OS_MAC
# include "mac/qtmac/qtmac.h"
#endif // Q_OS_MAC
#include <QtCore>
#include <QtNetwork/QHostAddress>

#define DEBUG "acipc"
#include "qtx/qxdebug.h"

#define IPC_HOST        "127.0.0.1"

#define IPC_PORT_START  6002
#define IPC_PORT_STOP   6880
#define IPC_PORT(_key)  (IPC_PORT_START + (_key) % (IPC_PORT_STOP - IPC_PORT_START))

// - Helpers -

bool
AcIpcController::isProcessRunning(const QString &processName)
{
#ifdef Q_OS_WIN
  QString exe = processName;
  if (!exe.endsWith(".exe", Qt::CaseInsensitive))
    exe.append(".exe");
  return QtWin::getProcessIdByName(exe);
#else
  Q_UNUSED(processName) // TODO
  return false;
#endif // Q_OS_MAC
}

bool
AcIpcController::open(const QString &app, const QStringList &args)
{
#ifdef Q_OS_MAC
  return QtMac::open(app, args);
#else
  return QProcess::startDetached(app, args);
#endif // Q_OS_MAC
}

#ifdef WITH_LIB_METACALL

// - State -

bool
AcIpcController::isActive() const
{ return filter_ && filter_->isActive(); }

void
AcIpcController::stop()
{ if (filter_) filter_->stop(); }

void
AcIpcController::startServer()
{
  DOUT("enter");
  if (!filter_)
    invalidateFilter();
  filter_->startServer(QHostAddress(IPC_HOST), IPC_PORT(id_));
  DOUT("exit");
}

void
AcIpcController::startClient()
{
  DOUT("enter");
  if (!filter_)
    invalidateFilter();
  filter_->startClient(QHostAddress(IPC_HOST), IPC_PORT(id_), false); // async = false
  DOUT("exit");
}

void
AcIpcController::invalidateFilter()
{
  DOUT("enter");
  if (filter_) {
    filter_->stop();
    filter_->setWatchedObject(0);
    filter_->deleteLater();
  }
  filter_ = new MetaCallFilter(this);
  filter_->setWatchedObject(this);
  DOUT("exit");
}

#endif // WITH_LIB_METACALL

// EOF
