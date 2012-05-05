// application.cc
// 11/18/2011
#include "application.h"
#include "global.h"
#include "module/download/downloader.h"
#ifdef Q_OS_WIN
#  include "win/qtwin/qtwin.h"
#endif // Q_OS_WIN
#ifdef Q_OS_MAC
#  include "mac/qtmac/qtmac.h"
#elif defined Q_OS_UNIX
extern "C" {
#  include <sys/stat.h>
#  include <sys/types.h>
#  include <unistd.h>
#  include <fcntl.h>
} // extern "C"
#endif // Q_OS_
#include <QtGui>

#define DEBUG "application"
#include "module/debug/debug.h"

// - Constructions -

Application::Application(int &argc, char **argv, bool gui)
  : Base(argc, argv, gui), closed_(false)
{
  DOUT("enter");
  setOrganizationDomain(G_DOMAIN);
  setOrganizationName(G_ORGANIZATION);
  setApplicationName(G_APPLICATION);
  setApplicationVersion(G_VERSION);

  createDirectories();
  DOUT("exit");
}

void
Application::createDirectories()
{
  QDir profile(G_PATH_PROFILE);
  if (!profile.exists())
    profile.mkpath(profile.absolutePath());

  QDir caches(G_PATH_CACHES);
  if (!caches.exists())
    caches.mkpath(profile.absolutePath());

  QDir logs(G_PATH_LOGS);
  if (!logs.exists())
    logs.mkpath(logs.absolutePath());
}

void
Application::close()
{
  if (closed_)
    return;
  closed_ = true;

  DownloaderController::globalController()->abort();

  if (QThreadPool::globalInstance()->activeThreadCount()) {
#if QT_VERSION >= 0x040800
    // wait for at most 2 seconds ant kill all threads
    enum { CloseTimeout = 2000 };
    QThreadPool::globalInstance()->waitForDone(CloseTimeout);
#else
    //DOUT("WARNING: killing active threads; will be fixed in Qt 4.8");
    QThreadPool::globalInstance()->waitForDone();
#endif  // QT_VERSION
  }

  closeAllWindows();

  DOUT("enter: abort in 3 seconds");
  QTimer::singleShot(3000, this, SLOT(abort()));
  DOUT("exit");
}

void
Application::abort()
{
  DOUT("enter");
  qint64 pid = applicationPid();
#ifdef Q_OS_WIN
  QtWin::killCurrentProcess();
  // If failed
  QProcess::startDetached(QString("tskill %1").arg(QString::number(pid)));
#else
  QProcess::startDetached(QString("kill -9 %1").arg(QString::number(pid)));
#endif // Q_OS_WIN
  DOUT("exit");
}

void
Application::abortAll()
{
  DOUT("enter");
  QString app = QFileInfo(applicationFilePath()).fileName();
#ifdef Q_OS_WIN
  QProcess::startDetached("tskill", QStringList(app));
#else
  QProcess::startDetached("killall", QStringList(app));
#endif // Q_OS_WIN
  DOUT("exit");
}

// - Properties -

bool
Application::isSingleInstance() const
{
#ifdef Q_OS_WIN
  QFileInfo fi(applicationFilePath());
  QString processName = fi.fileName();
  QList<ulong> pids = QtWin::getProcessIdsByName(processName);
  return pids.size() <= 1;
#elif defined(Q_OS_MAC)
  return true;
#elif defined(Q_OS_UNIX)
  // See: http://www.linuxquestions.org/questions/programming-9/restricting-multiple-instance-of-a-program-242069/
  static int fd_lock = -1;
  if (fd_lock < 0) {
    fd_lock = ::open((G_PATH_LOCK_RUNNING).toLocal8Bit(), O_WRONLY|O_CREAT, 0666);
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
#else
  return true;
#endif // Q_OS_
}

// - Events -

bool
Application::event(QEvent *e)
{
  bool accept = true;
  switch (e->type()) {
  case QEvent::FileOpen:
    {
      QWidget *w = activeWindow();
      if (w) sendEvent(w, e);
    } break;
  default: accept = Base::event(e);
  }

  return accept;
}

// - Windows -

QWidget*
Application::activeMainWindow() const
{ return windows_.isEmpty() ? 0 : windows_.last(); }

void
Application::addWindow(QWidget *w)
{ windows_.removeAll(w); windows_.append(w); }

void
Application::removeWindow(QWidget *w)
{ windows_.removeAll(w); }

// - Debug -

void
Application::messageHandler(QtMsgType type, const char *msg)
{
  foreach (QtMsgHandler callback, globalInstance()->messageHandlers_)
    callback(type, msg);
}

// See: http://www.cppblog.com/lauer3912/archive/2011/04/10/143870.html
void
Application::loggedMessageHandler(QtMsgType type, const char *msg)
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

  QFile file(G_PATH_DEBUG);
  if (file.open(QIODevice::WriteOnly | QIODevice::Append))
    QTextStream(&file) << output;
#undef TIMESTAMP
}

void
Application::installMessageHandlers()
{
  qDebug() << "application::installMessageHandlers";
  QFile debug(G_PATH_DEBUG);
  if (debug.open(QIODevice::WriteOnly | QIODevice::Append)) {
    QTextStream(&debug)
      << "\n################################################################################\n\n";
    debug.close();

    qInstallMsgHandler(messageHandler);
    addMessageHandler(loggedMessageHandler);
    DOUT("debug message handler installed");
  }
}

// EOF
