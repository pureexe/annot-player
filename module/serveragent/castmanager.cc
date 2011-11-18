// castmanager.cc
// 11/15/2011
#include "castmanager.h"
#include <QtCore>

// - Constructions -

CastManager::CastManager(QObject *parent)
  : Base(parent)
{ }

bool
CastManager::containsEvent(const CastEvent &e) const
{
  mutex_.lock();
  bool ret = events_.contains(e);
  mutex_.unlock();
  return ret;
}

bool
CastManager::registerEvent(const CastEvent &e)
{
  bool ret = false;

  mutex_.lock();
  if (!events_.contains(e)) {
    events_.append(e);
    ret = true;
  }
  mutex_.unlock();

  return ret;
}

// EOF
