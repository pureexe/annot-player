// core/util/countdowntimer.cc
// 11/27/2011

#include "core/util/countdowntimer.h"
#include <QtCore>

// - Constructions -

Core::
CountdownTimer::CountdownTimer(QObject *parent)
  : Base(parent), count_(0)
{
  timer_ = new QTimer(this);
  connect(timer_, SIGNAL(timeout()), SLOT(decreaseCount()));
  connect(timer_, SIGNAL(timeout()), SIGNAL(timeout()));
}

// - Properties -

bool
Core::
CountdownTimer::isActive() const
{ return timer_->isActive(); }

int
Core::
CountdownTimer::interval() const
{ return timer_->interval(); }

void
Core::
CountdownTimer::setInterval(int msecs)
{ timer_->setInterval(msecs); }

int
Core::
CountdownTimer::count() const
{ return count_; }

void
Core::
CountdownTimer::setCount(int count)
{
  if (count_ != count)  {
    count_ = count;
    invalidateCount();
  }
}

// - Slots -

void
Core::
CountdownTimer::start(int count)
{
  stop();
  if (count)
    count_ = count;

  if (count_ <= 0)
    return;
  timer_->start();
}

void
Core::
CountdownTimer::stop()
{
  if (timer_->isActive())
    timer_->stop();
}

void
Core::
CountdownTimer::decreaseCount()
{
  count_--;
  invalidateCount();
}

void
Core::
CountdownTimer::invalidateCount()
{
  if (count_ <= 0)
    stop();
}

// EOF
