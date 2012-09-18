// qxcountdowntimer.cc
// 11/27/2011

#include "qtx/qxcountdowntimer.h"
#include <QtCore/QTimer>

QTX_BEGIN_NAMESPACE

// - Constructions -

void
QxCountdownTimer::init()
{
  timer_ = new QTimer(this);
  connect(timer_, SIGNAL(timeout()), SLOT(decreaseCount()));
  connect(timer_, SIGNAL(timeout()), SIGNAL(timeout()));
}

// - Properties -

bool
QxCountdownTimer::isActive() const
{ return timer_->isActive(); }

int
QxCountdownTimer::interval() const
{ return timer_->interval(); }

void
QxCountdownTimer::setInterval(int msecs)
{ timer_->setInterval(msecs); }

int
QxCountdownTimer::count() const
{ return count_; }

void
QxCountdownTimer::setCount(int count)
{
  if (count_ != count)  {
    count_ = count;
    invalidateCount();
  }
}

// - Slots -

void
QxCountdownTimer::start(int count)
{
  stop();
  if (count)
    count_ = count;

  if (count_ <= 0)
    return;
  timer_->start();
}

void
QxCountdownTimer::stop()
{
  if (timer_->isActive())
    timer_->stop();
}

void
QxCountdownTimer::decreaseCount()
{
  count_--;
  invalidateCount();
}

void
QxCountdownTimer::invalidateCount()
{
  if (count_ <= 0)
    stop();
}

QTX_END_NAMESPACE
