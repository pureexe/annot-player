#ifndef MOUSEGESTUREFILTER_H
#define MOUSEGESTUREFILTER_H

// mousegesturefilter.h
// 4/25/2012

#include <QtCore/QList>
#include <QtCore/QObject>

QT_FORWARD_DECLARE_CLASS(QMouseEvent)

class MouseGesture;
class MouseGestureFilterPrivate;

///  Role: event filter + gesture manager
class MouseGestureFilter : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(MouseGestureFilter)
  typedef MouseGestureFilter Self;
  typedef QObject Base;
  typedef MouseGestureFilterPrivate Private;

  Private *d_;
  QList<MouseGesture *> gestures_;
  Qt::MouseButtons buttons_;
  Qt::KeyboardModifiers modifiers_;
  bool active_;
  bool cancelContextMenu_;

public:
  /**
   *  The gestureButton controls which mouse button
   *  that has to be pressed during the mouse gesture.
   *  Notice that this all events for this button are
   *  swallowed by the filter.
   */
  explicit MouseGestureFilter(Qt::MouseButtons = Qt::RightButton, Qt::KeyboardModifiers = Qt::NoModifier, QObject *parent = 0);
  ~MouseGestureFilter();

  void add(MouseGesture *gesture);
  void add(const QList<MouseGesture *> &l);

  bool isActive() const { return active_; }

public slots:
  void clear(); ///< also delete the gesetures owned by the object

protected:
  virtual bool eventFilter(QObject *obj, QEvent *event); ///< \reimp

  bool mousePressEventFilter(QMouseEvent *event);
  bool mouseReleaseEventFilter(QMouseEvent *event);
  bool mouseMoveEventFilter(QMouseEvent *event);
  //bool contextMenuEventFilter(QContextMenuEvent *event);
};

#endif // MOUSEGESTUREFILTER_H
