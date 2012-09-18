// metacallpropagator.cc
// 4/9/2012
#include "lib/metacall/metacallpropagator.h"
#include "lib/metacall/metacallfilter.h"

#define DEBUG "metacallpropagator"
#include "qtx/qxdebug.h"

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
MetaCallPropagator::isActive() const
{ return filter_ && filter_->isActive(); }

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
{ if (isActive()) filter_->stop(); }

// EOF
