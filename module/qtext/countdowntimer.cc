// qtext/countdowntimer.cc
// 11/27/2011

#include "module/qtext/countdowntimer.h"
#include <QtCore/QTimer>

// - Constructions -

QtExt::
CountdownTimer::CountdownTimer(QObject *parent)
  : Base(parent), count_(0)
{
  timer_ = new QTimer(this);
  connect(timer_, SIGNAL(timeout()), SLOT(decreaseCount()));
  connect(timer_, SIGNAL(timeout()), SIGNAL(timeout()));
}

// - Properties -

bool
QtExt::
CountdownTimer::isActive() const
{ return timer_->isActive(); }

int
QtExt::
CountdownTimer::interval() const
{ return timer_->interval(); }

void
QtExt::
CountdownTimer::setInterval(int msecs)
{ timer_->setInterval(msecs); }

int
QtExt::
CountdownTimer::count() const
{ return count_; }

void
QtExt::
CountdownTimer::setCount(int count)
{
  if (count_ != count)  {
    count_ = count;
    invalidateCount();
  }
}

// - Slots -

void
QtExt::
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
QtExt::
CountdownTimer::stop()
{
  if (timer_->isActive())
    timer_->stop();
}

void
QtExt::
CountdownTimer::decreaseCount()
{
  count_--;
  invalidateCount();
}

void
QtExt::
CountdownTimer::invalidateCount()
{
  if (count_ <= 0)
    stop();
}

// EOF
