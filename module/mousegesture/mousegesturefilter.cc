// mousegesturefilter.cc
// 4/25/2012

#include "module/mousegesture/mousegesturefilter.h"
#include "module/mousegesture/mousegesturefilter_p.h"
#include "module/mousegesture/mousegesture.h"
#include <QtGui/QMouseEvent>
#include <boost/foreach.hpp>

//#define DEBUG "mousegesturefilter"
#include "module/debug/debug.h"

//#ifdef __GNUC__
//# pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses
//#endif // __GNUC__

// - Construction -

MouseGestureFilter::MouseGestureFilter(Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers, QObject *parent)
  : Base(parent), buttons_(buttons), modifiers_(modifiers), active_(false), cancelContextMenu_(false)
{ d_ = new Private; }

MouseGestureFilter::~MouseGestureFilter()
{ delete d_; }

// - Actions =

void
MouseGestureFilter::add(MouseGesture *gesture)
{ d_->addGesture(gesture); }

void
MouseGestureFilter::add(const QList<MouseGesture *> &l)
{ d_->addGestures(l); }

void
MouseGestureFilter::clear()
{
  d_->abortGesture();
  if (d_->hasGestures()) {
    foreach (MouseGesture *g, d_->gestures())
      if (g->parent() == this)
        delete g;
    d_->clearGestures();
  }
  active_ = false;
}

// - Events

bool
MouseGestureFilter::eventFilter(QObject *obj, QEvent *event)
{
  bool ret;
  switch(event->type()) {
  case QEvent::MouseButtonPress:   ret = mousePressEventFilter(static_cast<QMouseEvent *>(event)); break;
  case QEvent::MouseButtonRelease: ret = mouseReleaseEventFilter(static_cast<QMouseEvent *>(event)); break;
  case QEvent::MouseMove:          ret = mouseMoveEventFilter(static_cast<QMouseEvent *>(event)); break;
  case QEvent::ContextMenu:
    ret = (buttons_ & Qt::RightButton) && cancelContextMenu_;
    cancelContextMenu_ = false;
    DOUT("contextMenu: ret =" << ret);
    break;
  default: ret = false;
  }
  return ret || Base::eventFilter(obj, event);
}

bool
MouseGestureFilter::mousePressEventFilter(QMouseEvent *event)
{
  DOUT("enter");
  bool ret = false;
  if ((event->button() & buttons_) && event->modifiers() == modifiers_) {
    d_->beginGesture(event->pos());
    ret = active_ = true;
  }
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
MouseGestureFilter::mouseReleaseEventFilter(QMouseEvent *event)
{
  DOUT("enter");
  bool ret = false;
  if (active_ && (event->button() & buttons_) && event->modifiers() == modifiers_) {
    cancelContextMenu_ = d_->endGesture(event->pos()); // cancel context menu if triggered
    active_ = false;
    ret = true;
    DOUT("cancelContextMenu =" << cancelContextMenu_);
  }
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
MouseGestureFilter::mouseMoveEventFilter(QMouseEvent *event)
{
  //DOUT("enter");
  if (active_)
    d_->addPoint(event->pos());
  //DOUT("exit: ret =" << active_);
  return active_;
}

// EOF
