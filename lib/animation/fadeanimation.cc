// fadeanimation.cc
// 5/5/2012

#include "lib/animation/fadeanimation.h"

// - Actions -

enum { FadeInDuration = 500, FadeOutDuration = 500 };

void
FadeAnimation::fadeIn(qreal opacity)
{ restart(0.0, opacity, FadeInDuration, QEasingCurve::InOutQuart); }

void
FadeAnimation::fadeOut(qreal opacity)
{ restart(opacity, 0.0, FadeOutDuration, QEasingCurve::InOutQuart); }

void
FadeAnimation::restart(qreal begin, qreal end, qint64 duration, QEasingCurve::Type curve)
{
  if (state() != Stopped)
    stop();
  setStartValue(begin);
  setEndValue(end);
  setDuration(duration);
  setEasingCurve(curve);
  start();
}

// EOF
