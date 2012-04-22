// downloadmanager.cc
// 2/20/2012
#include "downloadmanager.h"
#include "downloadtask.h"
#include "module/mrlanalysis/mrlanalysis.h"
#include <QtCore/QRegExp>
#include <QtCore/QTimer>

#define DEBUG "downloadmanager"
#include "module/debug/debug.h"

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

// - Controls -

void
DownloadManager::stopAll()
{
  foreach (DownloadTask *t, tasks_)
    t->stop();
}

void
DownloadManager::removeAll()
{
  foreach (DownloadTask *t, tasks_) {
    t->stop();
    if (t->parent() == this)
      QTimer::singleShot(0, t, SLOT(deleteLater()));
  }
  tasks_.clear();
}

void
DownloadManager::removeTask(DownloadTask *t)
{
  if (t) {
    tasks_.removeAll(t);
    t->stop();
    if (t->parent() == this)
      QTimer::singleShot(0, t, SLOT(deleteLater()));
  }
}

void
DownloadManager::addTask(DownloadTask *t)
{
  Q_ASSERT(t);
  connect(t, SIGNAL(finished(DownloadTask*)), SLOT(refreshSchedule()), Qt::QueuedConnection);
  connect(t, SIGNAL(error(QString)), SLOT(refreshSchedule()), Qt::QueuedConnection);
  tasks_.append(t);
}

void
DownloadManager::refreshSchedule()
{
  enum { NicoTaskInterval = 2000 }; // 3 seconds
  int nicoCount = 0;
  int count = 0;
  foreach (DownloadTask *t, tasks_)
    switch (t->state()) {
    case DownloadTask::Downloading:
      count++;
      break;
    case DownloadTask::Pending:
      t->setState(DownloadTask::Stopped);
      if (threadCount_ <= 0 || count < threadCount_) {
        count++;
        if (t->url().startsWith("http://" MA_EIGEN_NICOVIDEO, Qt::CaseInsensitive))
          t->startLater(NicoTaskInterval * nicoCount++);
        else
          t->start();
      } break;
    default: ;
    }
}

// EOF
