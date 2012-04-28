// mousegesturefilter_p.cc
// 4/25/2012

#include "mousegesturefilter_p.h"
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
#include <climits>
#include <list>

//#define DEBUG "mousegesturefilter_p"
#include "module/debug/debug.h"

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses
#endif // __GNUC__

// - Begin/End -

bool
MouseGestureFilterPrivate::endGesture(const QPoint &pos)
{
  bool triggered = true;;
  if (!positions_.empty() && pos != positions_.back())
    positions_.push_back(pos);
  DOUT("enter: positions count =" << positions_.size());
  if (positions_.size() > 1 )
    triggered = triggerGesture();
  positions_.clear();
  DOUT("exit: ret =" << triggered);
  return triggered;
}

void
MouseGestureFilterPrivate::addPoint(const QPoint &pos)
{
  if (positions_.empty())
    beginGesture(pos);
  QPoint d = pos - positions_.back();
  if (d.x()*d.x() + d.y()*d.y() >= minimumMovement2_)
    positions_.push_back(pos);
}

bool
MouseGestureFilterPrivate::triggerGesture()
{
  DOUT("enter: gestures count =" << gestures_.size());
  PointList directions = joinDirections_(toDirections_(positions_));
  qreal minLength = directionLength_(directions) * minimumMatch_;

  DOUT("directions count =" << directions.size());
  do {
    foreach (MouseGesture *g, gestures_)
      if (directions.size() == size_t(g->directions().size())) {
        bool match = true;
        BOOST_AUTO(pi, directions.begin());
        BOOST_AUTO(di, g->directions().begin());
        for (; di != g->directions().end() && match; ++di, ++pi)
          switch (*di) {
          case MouseGesture::Up:        if (pi->y() >= 0) match = false;  break;
          case MouseGesture::Down:      if (pi->y() <= 0) match = false;  break;
          case MouseGesture::Left:      if (pi->x() >= 0) match = false;  break;
          case MouseGesture::Right:     if (pi->x() <= 0) match = false;  break;
          case MouseGesture::Horizontal:if (pi->x() == 0) match = false;  break;
          case MouseGesture::Vertical:  if (pi->y() == 0) match = false;  break;
          case MouseGesture::NoMatch:
          default: match = false;
          }

        if (match) {
          g->trigger();
          DOUT("exit: ret = true, matched");
          return true;
        }
      }
    removeShortest_(directions);
    directions = joinDirections_(directions);
  } while (!directions.empty() && directionLength_(directions) > minLength);

  foreach (MouseGesture *g, gestures_)
    if (g->directions().size() == 1 && g->directions().back() == MouseGesture::NoMatch) {
      g->trigger();
      DOUT("exit: ret = true, NoMatch");
      return false; // triggered, but not matched
    }
  DOUT("exit: ret = false");
  return false;
}

// Support functions implementation

/*
 *  toDirections_ - limits the directions of a list to up, down, left or right.
 *
 *  Notice! This function converts the list to a set of relative moves instead of a set of screen coordinates.
 */
MouseGestureFilterPrivate::PointList
MouseGestureFilterPrivate::toDirections_(const PointList &positions)
{
  DOUT("enter: count =" << positions.size());
  PointList ret;
  QPoint last;

  bool first = true;
  BOOST_FOREACH (const QPoint &pos, positions) {
    if (first)
      first = false;
    else {
      QPoint d = pos - last;
      if (qAbs(d.x()) > qAbs(d.y()))
        d.setY(0);
      else
        d.setX(0);
      ret.push_back(d);
    }
    last = pos;
  }
  DOUT("exit: count =" << ret.size());
  return ret;
}

/*
 *  simplyDirections_ - joins together contignous movements in the same direction.
 *
 *  Notice! This function expects a list of limited directions.
 */
MouseGestureFilterPrivate::PointList
MouseGestureFilterPrivate::joinDirections_(const PointList &positions)
{
  DOUT("enter: count =" << positions.size());
  PointList ret;
  QPoint last;
  bool first = true;
  BOOST_FOREACH (const QPoint &pos, positions) {
    if (first) {
      last = pos;
      first = false;
    } else {
      bool joined = false;
      if (last.x() > 0 && pos.x() > 0 ||
          last.x() < 0 && pos.x() < 0) {
        last.rx() += pos.x();
        joined = true;
      }
      if (last.y() > 0 && pos.y() > 0 ||
          last.y() < 0 && pos.y() < 0) {
        last.ry() += pos.y();
        joined = true;
      }

      if (!joined) {
        ret.push_back(last);
        last = pos;
      }
    }
  }

  if (!last.isNull())
    ret.push_back(last);

  DOUT("exit: count =" << ret.size());
  return ret;
}

/*
 *  removeShortest_ - removes the shortest segment from a list of movements.
 *
 *  If there are several equally short segments, the first one is removed.
 */
void
MouseGestureFilterPrivate::removeShortest_(PointList &positions)
{
  PointList::iterator shortest;
  int shortestLength = INT_MAX;
  for (PointList::iterator p = positions.begin(); p != positions.end(); ++p ) {
    int len = p->x()*p->x() + p->y()*p->y();
    if (len < shortestLength) {
      shortestLength = len;
      shortest = p;
    }
  }
  positions.erase(shortest);
}

/*
 *  directionLength_ - calculates the total length of the movements from a list of relative movements.
 */
int
MouseGestureFilterPrivate::directionLength_(const PointList &positions)
{
  int ret = 0;
  BOOST_FOREACH (const QPoint &pos, positions)
    ret += pos.manhattanLength();
  return ret;
}

// EOF
