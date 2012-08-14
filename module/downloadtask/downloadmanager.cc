// downloadmanager.cc
// 2/20/2012
#include "module/downloadtask/downloadmanager.h"
#ifdef WITH_MODULE_ANNOTDOWN
# include "module/annotdown/annotationdownloader.h"
#endif // WITH_MODULE_ANNOTDOWN
#ifdef WITH_MODULE_MRLANALYSIS
# include "module/mrlanalysis//mrlanalysis.h"
#endif // WITH_MODULE_MRLANALYSIS
#include <QtCore/QRegExp>
#include <QtCore/QThreadPool>

#define DEBUG "downloadmanager"
#include "module/debug/debug.h"

// - Construction -

DownloadManager::DownloadManager(QObject *parent)
  : Base(parent), threadCount_(0)
{
#ifdef WITH_MODULE_ANNOTDOWN
  downloader_ = new AnnotationDownloader(this);
  connect(downloader_, SIGNAL(errorMessage(QString)), SIGNAL(errorMessage(QString)));
  connect(downloader_, SIGNAL(warning(QString)), SIGNAL(warning(QString)));
  connect(downloader_, SIGNAL(message(QString)), SIGNAL(message(QString)));

  connect(downloader_, SIGNAL(fileSaved(QString)), SIGNAL(fileSaved(QString)));
#endif // WITH_MODULE_ANNOTDOWN
}

// - Properties -

void
DownloadManager::setMaxThreadCount(int n)
{
  threadCount_ = n;
  if (QThreadPool::globalInstance()->maxThreadCount() < threadCount_)
    QThreadPool::globalInstance()->setMaxThreadCount(threadCount_);
}

QString
DownloadManager::downloadsLocation() const
{
  return
#ifdef WITH_MODULE_ANNOTDOWN
    downloader_->downloadsLocation()
#else
    downloadsLocation_
#endif // WITH_MODULE_ANNOTDOWN
  ;
}

void
DownloadManager::setDownloadsLocation(const QString &path)
{
#ifdef WITH_MODULE_ANNOTDOWN
  downloader_->setDownloadsLocation(path);
#else
  downloadsLocation_ = path;
#endif // WITH_MODULE_ANNOTDOWN
}

// - Helper -

QString
DownloadManager::normalizeUrl(const QString &url)
{
  return
#ifdef WITH_MODULE_MRLANALYSIS
    MrlAnalysis::normalizeUrl(url)
#else
    url
#endif // WITH_MODULE_MRLANALYSIS
  ;
}

// - Queries -

DownloadTask*
DownloadManager::taskWithUrl(const QString &url)
{
  if (!tasks_.isEmpty()) {
    QString nurl = normalizeUrl(url);
    foreach (DownloadTask *t, tasks_)
      if (nurl == normalizeUrl(t->url()))
        return t;
  }
  return 0;
}

DownloadTask*
DownloadManager::taskWithId(int tid)
{
  foreach (DownloadTask *t, tasks_)
    if (tid == t->id())
      return t;
  return 0;
}

bool
DownloadManager::isRunning() const
{
  foreach (const DownloadTask *t, tasks_)
    switch (t->state()) {
    case DownloadTask::Downloading:
    case DownloadTask::Pending:
      return true;
    default: ;
    }
  return false;
}

// - Controls -

void
DownloadManager::stopAll()
{
  DOUT("enter");
  foreach (DownloadTask *t, tasks_)
    t->stop();
  DOUT("exit");
}

void
DownloadManager::removeAll()
{
  DOUT("enter");
  if (!tasks_.isEmpty()) {
    foreach (DownloadTask *t, tasks_) {
      t->stop();
      if (t->parent() == this)
        t->deleteLater();
    }
    tasks_.clear();
  }
  DOUT("exit");
}

void
DownloadManager::removeTask(DownloadTask *t)
{
  if (t) {
    tasks_.removeAll(t);
    t->stop();
    emit taskRemoved(t);
    if (t->parent() == this)
      t->deleteLater();
  }
}

void
DownloadManager::addTask(DownloadTask *t)
{
  Q_ASSERT(t);
  connect(t, SIGNAL(finished(DownloadTask*)), SLOT(refreshSchedule()), Qt::QueuedConnection);
  connect(t, SIGNAL(errorMessage(QString)), SLOT(refreshSchedule()), Qt::QueuedConnection);
  connect(t, SIGNAL(downloadAnnotationRequested(QString)), SLOT(downloadAnnotation(QString)));
  connect(t, SIGNAL(downloadAnnotationRequested(QString,QString,QString)), SLOT(downloadAnnotation(QString,QString,QString)));
  tasks_.append(t);
  emit taskAdded(t);
}

void
DownloadManager::refreshSchedule()
{
  enum { PendingTaskInterval = 3000 }; // 3 seconds
  int pendingCount = 0;
  int count = 0;
  DOUT("enter: task count =" << tasks_.size());
  foreach (DownloadTask *t, tasks_)
    switch (t->state()) {
    case DownloadTask::Downloading:
      count++;
      break;
    case DownloadTask::Pending:
      if (threadCount_ <= 0 || count < threadCount_) {
        count++;
        t->startLater(PendingTaskInterval * pendingCount++);
      } break;
    default: ;
    }
  DOUT("exit: count =" << count << ", pending count =" << pendingCount);
}

// - Annotation -

void
DownloadManager::downloadAnnotation(const QString &url)
{
  DOUT("refurl =" << url);
#ifdef WITH_MODULE_ANNOTDOWN
  downloader_->download(url);
#else
  Q_UNUSED(url);
#endif // WITH_MODULE_ANNOTDOWN
}

void
DownloadManager::downloadAnnotation(const QString &url, const QString &refurl, const QString &title)
{
  DOUT("suburl =" << url << ", refurl =" << refurl << ", title =" << title);
#ifdef WITH_MODULE_ANNOTDOWN
  downloader_->download(url, refurl, title);
#else
  Q_UNUSED(url);
  Q_UNUSED(refurl);
  Q_UNUSED(title);
#endif // WITH_MODULE_ANNOTDOWN
}


// EOF
