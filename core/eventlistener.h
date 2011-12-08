#ifndef _CORE_EVENTLISTENER_H
#define _CORE_EVENTLISTENER_H

// core/eventlistener.h
// 8/6/2011

#include <QtGlobal>

QT_FORWARD_DECLARE_CLASS(QContextMenuEvent)
QT_FORWARD_DECLARE_CLASS(QMouseEvent)

namespace Core {

  class EventListener
  {
  public:
    virtual void sendContextMenuEvent(QContextMenuEvent *) { }

    virtual void sendMouseMoveEvent(QMouseEvent *) { }
    virtual void sendMousePressEvent(QMouseEvent *) { }
    virtual void sendMouseReleaseEvent(QMouseEvent *) { }
    virtual void sendMouseDoubleClickEvent(QMouseEvent *) { }
  };

} // namespace Core

#endif // _CORE_EVENTLISTENER_H
