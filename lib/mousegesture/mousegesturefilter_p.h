#ifndef MOUSEGESTUREFILTER_P_H
#define MOUSEGESTUREFILTER_P_H

// mousegesturefilter_p.h
// 4/25/2012

#include "lib/mousegesture/mousegesture.h"
#include <QtCore/QPoint>
#include <list>

class MouseGestureFilterPrivate
{
  typedef std::list<QPoint> PointList;

  PointList positions_;
  QList<MouseGesture *> gestures_;

  int minimumMovement2_;
  qreal minimumMatch_;

public:
  explicit MouseGestureFilterPrivate(int minimumMovement = 5, qreal minimumMatch = 0.9)
    : minimumMovement2_(minimumMovement*minimumMovement), minimumMatch_(minimumMatch) { }

  void beginGesture(const QPoint &pos) { positions_.push_back(pos); }
  void addPoint(const QPoint &pos);
  bool endGesture(const QPoint &pos); ///< return if matched
  void abortGesture() { positions_.clear(); }

  const QList<MouseGesture *> &gestures() const { return gestures_; }
  bool hasGestures() const { return !gestures_.isEmpty(); }
  void addGesture(MouseGesture *gesture) { gestures_.append(gesture); }
  void addGestures(const QList<MouseGesture *> &l) { gestures_.append(l); }
  void clearGestures() { gestures_.clear(); }

protected:
  bool triggerGesture();

private:
  static PointList joinDirections_(const PointList &positions); // join vectors in same directions
  static PointList toDirections_(const PointList &positions); // keep only max(dx,dy)
  static int directionLength_(const PointList &positions);
  static void removeShortest_(PointList &positions);
};

#endif // MOUSEGESTUREFILTER_P_H
