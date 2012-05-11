// acapplication.cc
// 11/18/2011
#include "project/common/acapplication.h"
#ifdef Q_WS_WIN
#  include "win/qtwin/qtwin.h"
#endif // Q_OS_WIN
#ifdef Q_WS_MAC
#  include "mac/qtmac/qtmac.h"
#endif // Q_OS_MAC
#ifdef Q_WS_X11
extern "C" {
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <unistd.h>
#  include <fcntl.h>
} // extern "C"
#endif // Q_WS_X11
#include <QtGui>

#define DEBUG "acapplication"
#include "module/debug/debug.h"

// - Construction -

AcApplication::AcApplication(int &argc, char **argv)
  : Base(argc, argv)
{
  DOUT("enter");
  DOUT("exit");
}

AcApplication::AcApplication(int &argc, char **argv, bool gui)
  : Base(argc, argv, gui)
{
  DOUT("enter");
  DOUT("exit");
}

AcApplication::~AcApplication()
{
  DOUT("enter: abort in 3 seconds");
  QTimer::singleShot(3000, this, SLOT(abort()));
  DOUT("exit");
}

void
AcApplication::abort()
{
  DOUT("enter");
  qint64 pid = applicationPid();
#ifdef Q_WS_WIN
  //QtWin::killCurrentProcess();
  QString cmd = QString("tskill %1").arg(QString::number(pid));
  QtWin::run(cmd, false); // visible = false
#else
  QProcess::startDetached(QString("kill -9 %1").arg(QString::number(pid)));
#endif // Q_WS_WIN
  DOUT("exit");
}

void
AcApplication::abortAll()
{
  DOUT("enter");
  QString app = QFileInfo(applicationFilePath()).fileName();
#ifdef Q_WS_WIN
  QString cmd = QString("tskill \"%1\"").arg(app);
  QtWin::run(cmd, false); // visible = fales
#else
  QProcess::startDetached("killall", QStringList(app));
#endif // Q_WS_WIN
  DOUT("exit");
}

// - Properties -

bool
AcApplication::isSingleInstance() const
{
#ifdef Q_WS_WIN
  QFileInfo fi(applicationFilePath());
  QString processName = fi.fileName();
  QList<ulong> pids = QtWin::getProcessIdsByName(processName);
  return pids.size() <= 1;
  return true;
#elif defined Q_WS_X11
  // See: http://www.linuxquestions.org/questions/programming-9/restricting-multiple-instance-of-a-program-242069/
  static int fd_lock = -1;
  if (fd_lock < 0) {
    fd_lock = ::open(lockFileName_.toLocal8Bit(), O_WRONLY|O_CREAT, 0666);
    if (fd_lock >= 0) {
      flock fl; {
        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = 0;
        fl.l_len = 1;
      }
      if (::fcntl(fd_lock, F_SETLK, &fl) < 0) {
        ::close(fd_lock);
        fd_lock = -1;
      }
    }
  }
  return fd_lock >= 0;
}
#else // Q_WS_MAC
  return true;
#endif // Q_WS_
}

// - Debug -

void
AcApplication::messageHandler(QtMsgType type, const char *msg)
{
  foreach (QtMsgHandler callback, globalInstance()->messageHandlers_)
    callback(type, msg);
}

// See: http://www.cppblog.com/lauer3912/archive/2011/04/10/143870.html
void
AcApplication::loggedMessageHandler(QtMsgType type, const char *msg)
{
#define TIMESTAMP QDateTime::currentDateTime().toString("MM:dd: hh:mm:ss")
  QString output;
  switch (type) {
  case QtDebugMsg:    output = QString("%1: %2\n").arg(TIMESTAMP).arg(msg); break;
  case QtWarningMsg:  output = QString("%1: warning: %2\n").arg(TIMESTAMP).arg(msg); break;
  case QtCriticalMsg: output = QString("%1: critical: %2\n").arg(TIMESTAMP).arg(msg); break;
  case QtFatalMsg:    output = QString("%1: fatal: %2\n").arg(TIMESTAMP).arg(msg); break;
  default: return;
  }

  QFile file(globalInstance()->logFileName_);
  if (file.open(QIODevice::WriteOnly | QIODevice::Append))
    QTextStream(&file) << output;
#undef TIMESTAMP
}

void
AcApplication::installMessageHandlers()
{
  qDebug() << "application::installMessageHandlers";
  QFile debug(globalInstance()->logFileName_);
  if (debug.open(QIODevice::WriteOnly | QIODevice::Append)) {
    QTextStream(&debug)
      << "\n################################################################################\n\n";
    debug.close();

    qInstallMsgHandler(messageHandler);
    addMessageHandler(loggedMessageHandler);
    DOUT("debug message handler installed");
  }
}

// - Events -

bool
AcApplication::event(QEvent *e)
{
  bool accept = true;
  switch (e->type()) {
  case QEvent::FileOpen:
    {
      QWidget *w = mainWindow();
      if (w) sendEvent(w, e);
    } break;
  default: accept = Base::event(e);
  }

  return accept;
}

// - Windows -

QWidget*
AcApplication::mainWindow() const
{ return windows_.isEmpty() ? 0 : windows_.last(); }

void
AcApplication::addWindow(QWidget *w)
{ windows_.removeAll(w); windows_.append(w); }

void
AcApplication::removeWindow(QWidget *w)
{ windows_.removeAll(w); }

// EOF
