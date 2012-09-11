// mrldownloadtask.cc
// 2/20/2012
#include "module/downloadtask/mrldownloadtask.h"
#ifdef WITH_MODULE_MRLRESOLVER
# include "module/mrlresolver/luamrlresolver.h"
#else
# error "require module mrlresolver"
#endif // WITH_MODULE_MRLRESOLVER
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>

#define DEBUG "mrldownloadtask"
#include "module/debug/debug.h"

enum { MaxDownloadRetries = 5 };

// - Helper -

bool
MrlDownloadTask::isMultiMediaMimeType(const QString &contentType)
{ return !contentType.isEmpty() && !contentType.startsWith("text/", Qt::CaseInsensitive); }

// - Construction -

MrlDownloadTask::~MrlDownloadTask()
{
  stop();
  if (resolver_)
    delete resolver_;
}

// - Task -

void
MrlDownloadTask::reset()
{
  retries_ = MaxDownloadRetries;
  stopped_ = false;
  Base::reset();
}

void
MrlDownloadTask::stop()
{
  sleep_.stop();
  if (!stopped_) {
    DOUT("enter");
    stopped_ = true;
    setState(Stopped);
    emit stopped();
    DOUT("exit");
  }
}

void
MrlDownloadTask::run(bool execute)
{
  DOUT("enter");
  if (stopped_) {
    DOUT("exit: stopped");
    return;
  }
  setState(Downloading);
  progress_.clear();

  if (resolver_)
    resolver_->deleteLater();
  resolver_ = new LuaMrlResolver;//new MrlResolverManager;
  resolver_->setSynchronized(true);
  connect(resolver_, SIGNAL(errorMessage(QString)), SIGNAL(errorMessage(QString)));
  connect(resolver_, SIGNAL(mediaResolved(MediaInfo,QNetworkCookieJar*)), SLOT(downloadMedia(MediaInfo,QNetworkCookieJar*)));
  bool ok = resolver_->resolveMedia(url());
  if (ok) {
    //const MediaInfo &mi = resolver_->resolvedMediaInfo();
    //QNetworkCookieJar *jar = resolver_->resolvedCookieJar();
    //downloadMedia(mi, jar);
    if (execute)
      exec();
  } else {
    setState(Error);
    emit errorMessage(tr("failed to download from URL") + ": " + url());
    //emit downloadAnnotationRequested(url);
  }
  DOUT("exit");
}

// - Download -

void
MrlDownloadTask::downloadMedia(const MediaInfo &mi, QNetworkCookieJar *jar)
{
  DOUT("enter");
  if (stopped_) {
    quit();
    DOUT("exit: stopped");
    return;
  }

  QDir dir(downloadPath());
  if (!dir.exists())
    dir.mkpath(dir.absolutePath());

  QString title = mi.title;
  if (title.isEmpty())
    title = tr("unknown");
  setTitle(title);

  if (!mi.refurl.isEmpty())
    setUrl(mi.refurl);

  if (!mi.suburl.isEmpty())
    emit downloadAnnotationRequested(mi.suburl, mi.refurl, mi.title);
  switch (mi.mrls.size()) {
  case 0: emit errorMessage(tr("failed to resolve media URL") +": " + mi.refurl); break;
  case 1: downloadSingleMedia(mi, jar); break;
  default: downloadMultipleMedia(mi, jar);
  }

  quit();
  DOUT("exit");
}

// EOF
