// downloadmanager.cc
// 2/20/2012
#include "module/downloadtask/downloadmanager.h"
#include <QtCore/QRegExp>
#include <QtCore/QThreadPool>

#define DEBUG "downloadmanager"
#include "module/debug/debug.h"

// - Properties -

void
DownloadManager::setMaxThreadCount(int n)
{
  threadCount_ = n;
  if (QThreadPool::globalInstance()->maxThreadCount() < threadCount_)
    QThreadPool::globalInstance()->setMaxThreadCount(threadCount_);
}

// - Helper -

QString
DownloadManager::normalizeUrl(const QString &url)
{
  QString ret = url.trimmed().toLower();
  if (!ret.isEmpty()) {
    ret.remove(QRegExp("/index.html$"))
       .remove(QRegExp("/index_1.html$"))
       .remove(QRegExp("/$"))
       .replace("http://acfun.tv/" , "http://www.acfun.tv")
       .replace("http://bilibili.tv/" , "http://www.bilibili.tv");
    if (ret.startsWith("http://www.nicovideo.jp/watch/"))
      ret.remove(QRegExp("\\?.*"));
  }
  return ret;
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
  connect(t, SIGNAL(error(QString)), SLOT(refreshSchedule()), Qt::QueuedConnection);
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

// EOF
