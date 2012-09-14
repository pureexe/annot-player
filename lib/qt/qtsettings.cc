// player.cc
// 6/23/2012

#include "lib/qt/qtsettings.h"
#include <corelib/animation/qabstractanimation_p.h>

#ifndef DEFAULT_TIMING_INTERVAL
# define DEFAULT_TIMING_INTERVAL   16  // see qabstractanimation.cpp
#endif // DEFAULT_TIMING_INTERVAL

// - Construction -

QtSettings::QtSettings(QObject *parent)
  : Base(parent), animationTimingInterval_(DEFAULT_TIMING_INTERVAL)
{ }

// - Properties -

void
QtSettings::setAnimationTimingInterval(int secs)
{ QUnifiedTimer::instance()->setTimingInterval(animationTimingInterval_ = secs); }

void
QtSettings::resetAnmationTimingInterval()
{ setAnimationTimingInterval(DEFAULT_TIMING_INTERVAL); }

// EOF
