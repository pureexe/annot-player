// application.cc
// 11/18/2011
#include "application.h"
#include "global.h"
#ifdef Q_OS_WIN
  #include "win/qtwin/qtwin.h"
#endif // Q_OS_WIN
#include <QtGui>
#ifdef Q_OS_UNIX
extern "C" {
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
  #include <fcntl.h>
} // extern "C"
#endif // Q_OS_UNIX

#define DEBUG "Application"
#include "module/debug/debug.h"

// - Debug -

namespace { // anonymous, debug

  // See: http://www.cppblog.com/lauer3912/archive/2011/04/10/143870.html
  void
  debugMessageHandler(QtMsgType type, const char *msg)
  {
#define TIMESTAMP QDateTime::currentDateTime().toString("MM:dd: hh:mm:ss")
    QString output;
    switch (type) {
    case QtDebugMsg:    output = QString("%1: %2\n").arg(TIMESTAMP).arg(msg); break;
    case QtWarningMsg:  output = QString("%1: warning: %2\n").arg(TIMESTAMP).arg(msg); break;
    case QtCriticalMsg: output = QString("%1: critical: %2\n").arg(TIMESTAMP).arg(msg); break;
    case QtFatalMsg:    output = QString("%1: fatal: %2\n").arg(TIMESTAMP).arg(msg); break;
    }

    QFile file(G_PATH_DEBUG);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append))
      QTextStream(&file) << output;
#undef TIMESTAMP
  }

} // anonymous namespace


// - Constructions -

Application::Application(int &argc, char **argv)
  : Base(argc, argv), w_(0)
{
  setOrganizationDomain(G_DOMAIN);
  setOrganizationName(G_ORGANIZATION);
  setApplicationName(G_APPLICATION);
  setApplicationVersion(G_VERSION);

  createDirectories();
}

void
Application::createDirectories()
{
  QDir profile(G_PATH_PROFILE);
  if (!profile.exists())
    profile.mkpath(profile.absolutePath());

  QDir lock(G_PATH_LOCK);
  if (!lock.exists())
    lock.mkpath(lock.absolutePath());

  QDir caches(G_PATH_CACHES);
  if (!caches.exists())
    caches.mkpath(caches.absolutePath());

#ifdef USE_MODE_DEBUG
  QDir logs(G_PATH_LOGS);
  if (!logs.exists())
    logs.mkpath(logs.absolutePath());
#endif // USE_MODE_DEBUG
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
    fd_lock = ::open(G_PATH_LOCK_RUNNING, O_WRONLY|O_CREAT, 0666));
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
  case QEvent::FileOpen: if (w_) sendEvent(w_, e); break;
  default: accept = Base::event(e);
  }

  return accept;
}

// - Logging -

void
Application::startLoggingDebugMessage()
{
  QFile debug(G_PATH_DEBUG);
  if (debug.open(QIODevice::WriteOnly | QIODevice::Append)) {
    QTextStream(&debug)
      << "\n--------------------------------------------------------------------------------\n\n";
    debug.close();

    qInstallMsgHandler(::debugMessageHandler);
    DOUT("main: debug message handler installed");
  }
}

// EOF
