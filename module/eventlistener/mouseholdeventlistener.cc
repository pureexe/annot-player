// mouseholdeventlistener.cc
// 7/14/2012

#include "module/eventlistener/mouseholdeventlistener.h"
#include <QtGui/QMouseEvent>
#include <QtCore/QDateTime>
#include <QtCore/QEvent>
#include <QtCore/QTimer>

// - Construction -

MouseHoldEventListener::MouseHoldEventListener(QObject *parent)
  : Base(parent)
{
  timer_ = new QTimer(this);
  timer_->setSingleShot(true);
  connect(timer_, SIGNAL(timeout()), SIGNAL(triggered()));
}

// - Properties -

qint64
MouseHoldEventListener::interval() const
{ return timer_->interval(); }

void
MouseHoldEventListener::setInterval(qint64 msecs)
{ timer_->setInterval(msecs); }

void
MouseHoldEventListener::clear()
{
  timer_->stop();
  pressPos_ = movePos_ = QPoint();
}

// - Events -

void
MouseHoldEventListener::eventListener(QEvent *event)
{
  Q_ASSERT(event);
  switch (event->type()) {
    // Mouse events:
  case QEvent::MouseButtonPress:
    {
      auto e = static_cast<QMouseEvent *>(event);
      pressPos_ = e->globalPos();

      if (e->buttons() == Qt::LeftButton && !e->modifiers())
        timer_->start();
    }
    break;
  case QEvent::MouseButtonRelease:
    timer_->stop();
    break;
  case QEvent::MouseButtonDblClick:
    timer_->stop();
    QTimer::singleShot(0, timer_, SLOT(stop()));
    break;
  case QEvent::MouseMove:
    {
      auto e = static_cast<QMouseEvent *>(event);
      QPoint gp = e->globalPos();
      if (gp == pressPos_ || gp == movePos_)
        break;

      movePos_ = gp;
      timer_->stop();
    } break;

    // Change events:
  case QEvent::WindowStateChange:
    timer_->stop();
    break;

  default: ;
  }
}

// EOF
