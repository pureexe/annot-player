#ifndef _QTEXT_EVENTFORWARDER_H
#define _QTEXT_EVENTFORWARDER_H

// qtext/eventforwarder.h
// 8/6/2011

#include <QtGlobal>

QT_FORWARD_DECLARE_CLASS(QContextMenuEvent)
QT_FORWARD_DECLARE_CLASS(QMouseEvent)

namespace QtExt {

class EventForwarder
{
public:
  virtual void sendContextMenuEvent(QContextMenuEvent *) { }

  virtual void sendMouseMoveEvent(QMouseEvent *) { }
  virtual void sendMousePressEvent(QMouseEvent *) { }
  virtual void sendMouseReleaseEvent(QMouseEvent *) { }
  virtual void sendMouseDoubleClickEvent(QMouseEvent *) { }
};

} // namespace QtExt

#endif // _QTEXT_EVENTFORWARDER_H
