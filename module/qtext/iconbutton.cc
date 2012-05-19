// qtext/iconbutton.cc
// 5/16/2012

#include "module/qtext/iconbutton.h"
#include <QtCore/QEvent>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QSize>
#include <QtCore/QTimer>

#define RADIUS_MIN      19 // default iconSize on Mac OS X
//#define RADIUS_MAX      40

// - Construction -

enum { ResizeDuration = 150 }; // in msecs
enum { LeaveDuration = 5 * 1000 }; // 8 seconds

QtExt::
IconButton::IconButton(QWidget *parent)
  : Base(parent),
    radius_(RADIUS_MIN), minimumRadius_(RADIUS_MIN), maximumRadius_(RADIUS_MIN),
    radiusAni_(0)
{
  leaveTimer_ = new QTimer(this);
  leaveTimer_->setSingleShot(true);
  leaveTimer_->setInterval(LeaveDuration);
  connect(leaveTimer_, SIGNAL(timeout()), SLOT(leave()));
}

// - Properties -

void
QtExt::
IconButton::setRadius(int value)
{
  if (radius_ != value) {
    radius_ = value;
    setIconSize(QSize(radius_, radius_));
  }
}

// - Events -

void
QtExt::
IconButton::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::EnabledChange && minimumRadius_ != maximumRadius_)
    setRadius(minimumRadius_);
  Base::changeEvent(event);
}

void
QtExt::
IconButton::enterEvent(QEvent *event)
{
  enter();
  Base::enterEvent(event);
}

void
QtExt::
IconButton::leaveEvent(QEvent *event)
{
  leave();
  Base::enterEvent(event);
}

void
QtExt::
IconButton::enter()
{
  if (isEnabled() && minimumRadius_ != maximumRadius_) {
    if (!radiusAni_) {
      radiusAni_ = new QPropertyAnimation(this, "radius", this);
      radiusAni_->setDuration(ResizeDuration);
    } else if (radiusAni_->state() != QAbstractAnimation::Stopped)
      radiusAni_->stop();
    radiusAni_->setStartValue(radius_);
    radiusAni_->setEndValue(maximumRadius_);
    radiusAni_->start();
  }
  leaveTimer_->start();
}

void
QtExt::
IconButton::leave()
{
  if (leaveTimer_->isActive())
    leaveTimer_->stop();
  if (isEnabled() && minimumRadius_ != maximumRadius_) {
    if (!radiusAni_) {
      radiusAni_ = new QPropertyAnimation(this, "radius", this);
      radiusAni_->setDuration(ResizeDuration);
    } else if (radiusAni_->state() != QAbstractAnimation::Stopped)
      radiusAni_->stop();
    radiusAni_->setStartValue(radius_);
    radiusAni_->setEndValue(minimumRadius_);
    //QTimer::singleShot(200, radiusAni_, SLOT(start()));
    radiusAni_->start();
  }
}

// EOF
