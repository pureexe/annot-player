#ifndef _QTEXT_EVENTLISTENER_H
#define _QTEXT_EVENTLISTENER_H

// qtext/eventlistener.h
// 8/6/2011

#include <QtGlobal>

QT_FORWARD_DECLARE_CLASS(QContextMenuEvent)
QT_FORWARD_DECLARE_CLASS(QMouseEvent)

namespace QtExt {

class EventListener
{
public:
  virtual void sendContextMenuEvent(QContextMenuEvent *) { }

  virtual void sendMouseMoveEvent(QMouseEvent *) { }
  virtual void sendMousePressEvent(QMouseEvent *) { }
  virtual void sendMouseReleaseEvent(QMouseEvent *) { }
  virtual void sendMouseDoubleClickEvent(QMouseEvent *) { }
};

} // namespace QtExt

#endif // _QTEXT_EVENTLISTENER_H
