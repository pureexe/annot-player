// streamservice.cc
// 2/11/2012

#include "streamservice.h"
#include "streamthread.h"
#include "live555_config.h"
#include "module/datastream/inputstream.h"
#include <QtCore>

#define DEBUG "streamservice"
#include "module/debug/debug.h"

// - Construction -

StreamService::StreamService(QObject *parent)
  : Base(parent)
{
  t_ = new StreamThread(this);
  connect(t_, SIGNAL(errorReceived(QString)), SIGNAL(errorReceived(QString)), Qt::QueuedConnection);
  connect(t_, SIGNAL(messageReceived(QString)), SIGNAL(messageReceived(QString)), Qt::QueuedConnection);
  connect(t_, SIGNAL(streamReady(QString)), SIGNAL(streamReady(QString)), Qt::QueuedConnection);
}

StreamService::~StreamService()
{
  if (t_->isRunning())
    t_->stop();
}

// - Properties -

bool
StreamService::hasStream() const
{ return t_->hasStream(); }

int
StreamService::addStream(MediaType t, InputStream *in, MediaToc *toc)
{
  Q_ASSERT(in);
  if (in)
    in->reset();
  return t_->addStream(t, in, toc);
}

void
StreamService::removeStream(int streamId)
{ t_->removeStream(streamId); }

void
StreamService::clear()
{
  t_->removeStreams();
  t_->setDuration(0);
}

void
StreamService::setDuration(qint64 msecs)
{ t_->setDuration(msecs); }

// - Service -

bool
StreamService::isActive() const
{ return t_->isRunning(); }

void
StreamService::stop(ulong timeout)
{
  DOUT("enter");
  t_->stop(timeout);
  DOUT("exit");
}

void
StreamService::terminateService()
{
  DOUT("enter");
  t_->terminate();
  DOUT("exit");
}

void
StreamService::start()
{
  DOUT("enter");
  if (isActive())
    stop();
  Q_ASSERT(!isActive());
  t_->start();
  DOUT("exit");
}

QString
StreamService::url() const
{ return t_->url(); }

// EOF
