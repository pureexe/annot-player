// aciconbutton.cc
// 5/16/2012

#include "src/common/aciconbutton.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QEvent>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QTimer>

#define RADIUS_MIN      19 // default iconSize on Mac OS X
//#define RADIUS_MAX      40

// - Construction -

enum { ResizeDuration = 150 }; // in msecs
enum { LeaveDuration = 5 * 1000 }; // 8 seconds

AcIconButton::AcIconButton(QWidget *parent)
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
AcIconButton::setRadius(int value)
{
  if (radius_ != value) {
    radius_ = value;
    setIconSize(QSize(radius_, radius_));
  }
}

// - Events -

void
AcIconButton::enter()
{
  QEvent e(QEvent::Enter);
  QCoreApplication::sendEvent(this, &e);
}

void
AcIconButton::leave()
{
  QEvent e(QEvent::Leave);
  QCoreApplication::sendEvent(this, &e);
}

void
AcIconButton::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::EnabledChange && minimumRadius_ != maximumRadius_)
    setRadius(minimumRadius_);
  Base::changeEvent(event);
}

void
AcIconButton::enterEvent(QEvent *event)
{
  qApp->setProperty("cursor", Qt::PointingHandCursor);
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
  Base::enterEvent(event);
}

void
AcIconButton::leaveEvent(QEvent *event)
{
  qApp->setProperty("cursor", Qt::ArrowCursor);
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
  Base::enterEvent(event);
}

// EOF
