// downloadmanager.cc
// 2/20/2012
#include "downloadmanager.h"
#include "downloadtask.h"
#include <boost/typeof/typeof.hpp>
#include <QtCore>

#define DEBUG "downloadmanager"
#include "module/debug/debug.h"

// - Tasks -

void
DownloadManager::stopAll()
{
  foreach (DownloadTask *t, tasks_)
    t->stop();
}

DownloadTask*
DownloadManager::taskWithId(int tid)
{
  foreach (DownloadTask *t, tasks_)
    if (tid == t->id())
      return t;
  return 0;
}

void
DownloadManager::removeTaskWithId(int tid)
{
  BOOST_AUTO(p, tasks_.begin());
  while (p != tasks_.end()) {
    if ((*p)->id() == tid)
      p = tasks_.erase(p);
    else
      ++p;
  }
}

// EOF
