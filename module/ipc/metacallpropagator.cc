// metacallpropagator.cc
// 4/9/2012
#include "metacallpropagator.h"
#include "metacallfilter.h"
#include <QtCore>

#define DEBUG "metacallpropagator"
#include "module/debug/debug.h"

// - Construction -

void
MetaCallPropagator::installMetaCallFilter()
{
  if (!filter_) {
    filter_ = new MetaCallFilter(this);
    filter_->setWatchedObject(this);
  }
}

// - Filter -

bool
MetaCallPropagator::isRunning() const
{ return filter_ && filter_->isRunning(); }


bool
MetaCallPropagator::startServer(const QHostAddress &address, int port)
{
  installMetaCallFilter();
  return filter_->startServer(address, port);
}

bool
MetaCallPropagator::startClient(const QHostAddress &address, int port)
{
  installMetaCallFilter();
  return filter_->startClient(address, port);
}

bool
MetaCallPropagator::isServer() const
{ return filter_ && filter_->isServer(); }

bool
MetaCallPropagator::isClient() const
{ return filter_ && filter_->isClient(); }

void
MetaCallPropagator::stop()
{ if (isRunnint()) filter_->stop(); }

// EOF
