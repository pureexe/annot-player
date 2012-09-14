// mouseclickeventlistener.cc
// 7/14/2012

#include "lib/eventlistener/mouseclickeventlistener.h"
#include <QtCore/QDateTime>
#include <QtCore/QEvent>
#include <QtCore/QTimer>
#include <QtGui/QMouseEvent>

//#define DEBUG "mouseclickeventlistener"
#include "lib/debug/debug.h"

// - Construction -

MouseClickEventListener::MouseClickEventListener(QObject *parent)
  : Base(parent), pressButtons_(0), pressed_(false), pressTime_(0)
{
  timer_ = new QTimer(this);
  timer_->setSingleShot(true);
  connect(timer_, SIGNAL(timeout()), SIGNAL(triggered()));
  connect(this, SIGNAL(enabledChanged(bool)), SLOT(updateEnabled(bool)));
}

// - Properties -

qint64
MouseClickEventListener::interval() const
{ return timer_->interval(); }

void
MouseClickEventListener::setInterval(qint64 msecs)
{ timer_->setInterval(msecs); }

void
MouseClickEventListener::clear()
{
  stop();
  pressed_ = false;
}

void
MouseClickEventListener::updateEnabled(bool t)
{
  if (!t)
    clear();
}

void
MouseClickEventListener::start()
{
  DOUT("start timer");
  timer_->start();
}

void
MouseClickEventListener::stop()
{
  DOUT("stop timer");
  if (timer_->isActive())
    timer_->stop();
}

// - Events -

void
MouseClickEventListener::eventListener(QEvent *event)
{
  Q_ASSERT(event);
  switch (event->type()) {
    // Mouse events:
  case QEvent::MouseButtonPress:
    {
      DOUT("QEvent::MouseButtonPress");
      pressTime_ = QDateTime::currentMSecsSinceEpoch();

      auto e = static_cast<QMouseEvent *>(event);
      pressPos_ = e->globalPos();
      pressButtons_ = e->buttons();
      pressed_ = true;
      stop();
    }
    break;
  case QEvent::MouseButtonRelease:
    {
      DOUT("QEvent::MouseButtonRelease");
      auto e = static_cast<QMouseEvent *>(event);
      if (pressed_ && pressButtons_ == Qt::LeftButton && !e->modifiers() &&
          //e->globalPos() == pressPos_
          (e->globalPos() - pressPos_).manhattanLength() <= 4 && // minor movement is allowed, 4 must be consistent with player.cc
          QDateTime::currentMSecsSinceEpoch() - pressTime_ < interval()*2) // skip mouse holding event
        start();
      else
        stop();
      pressed_ = false;
    } break;
  case QEvent::MouseButtonDblClick:
    DOUT("QEvent::MouseButtonDblClick");
    stop();
    pressed_ = false;
    break;
  case QEvent::MouseMove:
    {
      DOUT("QEvent::MouseMove");
      auto e = static_cast<QMouseEvent *>(event);
      QPoint gp = e->globalPos();
      if (gp == pressPos_ || gp == movePos_)
        break;

      movePos_ = gp;
      stop();
    } break;

    // Change events:
  case QEvent::WindowStateChange:
    DOUT("QEvent::WindowStateChange");
    //stop();
    pressed_ = false;
    break;

  default: ;
  }
}

// EOF
