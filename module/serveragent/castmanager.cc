// castmanager.cc
// 11/15/2011
#include "module/serveragent/castmanager.h"
#include <QtCore/QMutexLocker>

bool
CastManager::containsEvent(const CastEvent &e) const
{
  QMutexLocker lock(&mutex_);
  bool ret = events_.contains(e);
  return ret;
}

bool
CastManager::registerEvent(const CastEvent &e)
{
  QMutexLocker lock(&mutex_);
  bool ret = false;

  if (!events_.contains(e)) {
    events_.append(e);
    ret = true;
  }

  return ret;
}

// EOF
