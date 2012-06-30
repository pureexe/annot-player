#ifndef QTSETTINGS_H
#define QTSETTINGS_H

// qtsettings.h
// 6/24/2012

#include <QtCore/QObject>

class QtSettings : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(QtSettings)
  typedef QtSettings Self;
  typedef QObject Base;

  int animationTimingInterval_;

public:
  static Self *globalInstance() { static Self g; return &g; }

protected:
  explicit QtSettings(QObject *parent = 0);

public:
  /**
   *  QUnifiedTimer::timingInterval in msecs.
   *  DEFAULT_TIMING_INTERVAL defined in qabstractanimation.cpp is 16
   *  The smaller the interval, the updates and more CPU consumption.
   *  As 1 sec / 24 frames = 41.6 msec, timing interval should be at lease around 41/2 msec
   */
  int animationTimingInterval() const { return animationTimingInterval_; }
  void setAnimationTimingInterval(int msec);
  void resetAnmationTimingInterval();
};

#endif // QTSETTINGS_H
