#ifndef QXEVENTFORWARDER_H
#define QXEVENTFORWARDER_H

// qxeventforwarder.h
// 8/6/2011

#include "qtx/qtxglobal.h"
#include <QtGlobal>

QT_FORWARD_DECLARE_CLASS(QContextMenuEvent)
QT_FORWARD_DECLARE_CLASS(QMouseEvent)

QTX_BEGIN_NAMESPACE

class QxEventForwarder
{
public:
  virtual ~QxEventForwarder() { }

  virtual void sendContextMenuEvent(QContextMenuEvent *) { }

  virtual void sendMouseMoveEvent(QMouseEvent *) { }
  virtual void sendMousePressEvent(QMouseEvent *) { }
  virtual void sendMouseReleaseEvent(QMouseEvent *) { }
  virtual void sendMouseDoubleClickEvent(QMouseEvent *) { }
};

QTX_END_NAMESPACE

#endif // QXEVENTFORWARDER_H
