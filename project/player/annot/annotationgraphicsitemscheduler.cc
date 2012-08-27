// annotationgraphicsitemscheduler.cc
// 7/16/2011

#include "annotationgraphicsitemscheduler.h"
#include "annotationsettings.h"
#include "signalhub.h"
#include <QtCore>

//#define DEBUG "annotationgraphicsitemscheduler"
#include "module/debug/debug.h"

#ifdef __clang__
# pragma clang diagnostic ignored "-Wlogical-op-parentheses" // '&&' within '||'
#endif // __clang__

// - Properties -

enum { LaneHeight = ANNOTATION_SIZE_DEFAULT + ANNOTATION_SIZE_MARGIN * 2 + 6 }; // height of a piece of danmu
enum { ItemHaloHeight = 2 * 2 };

void
AnnotationGraphicsItemScheduler::pause()
{ pauseTime_ = QDateTime::currentMSecsSinceEpoch();  }

void
AnnotationGraphicsItemScheduler::resume()
{ resumeTime_ = QDateTime::currentMSecsSinceEpoch();  }

void
AnnotationGraphicsItemScheduler::clear()
{
  //cells_.clear();
  for (auto p = cells_.begin(); p != cells_.end(); ++p)
    p.value() = 0;
  lastCell_ = std::make_pair(QPoint(), qint64(0));
  pauseTime_ = resumeTime_ = 0;

  qMemSet(floatLane_, 0, sizeof(floatLane_)/sizeof(*floatLane_));
  qMemSet(topLaneTime_, 0, sizeof(topLaneTime_)/sizeof(*topLaneTime_));
  qMemSet(bottomLaneTime_, 0, sizeof(bottomLaneTime_)/sizeof(*bottomLaneTime_));
  qMemSet(subLaneStyle_, 0, sizeof(subLaneStyle_)/sizeof(*subLaneStyle_));
}

void
AnnotationGraphicsItemScheduler::clearSubtitles()
{
  for (int i = 0; i < LaneCount; i++)
    if (subLaneStyle_[i]) {
      switch (subLaneStyle_[i]) {
      case AnnotationGraphicsItem::TopStyle: topLaneTime_[i] = 0; break;
      case AnnotationGraphicsItem::BottomStyle: bottomLaneTime_[i] = 0; break;
      }
      subLaneStyle_[i] = 0;
    }
}

// - Float Scheduling -

namespace { namespace detail { // anonymous detail
  // 1: old, 2: now, ww: window width, now:time2
  // v1 = (ww+width1)/life1
  // v2 = (ww+width2)/life2
  // is available if
  //   v1 * (time2 - time1) - width1 > 0 && (
  //     v2 < v1 ||
  //     ((v1 * (time2 - time1) - width1) / (v2-v1)) * v2 >= ww
  //   )
  //
  // Return if 1 is available to save 2
  inline bool available(qint64 now, qreal v2, const AnnotationGraphicsItemScheduler::Item &item1, int ww)
  {
    if (!item1.time)
      return true;

    Q_ASSERT(item1.life);
    qreal v1 = (ww + item1.width) / qreal(item1.life);
          //v2 = (ww + width2) / qreal(life2);
    qreal d = v1 * (now - item1.time) - item1.width;
    return d > 0 && (
      v2 < v1 ||
      d * v2 > ww * (v2 - v1)
    );
  }
} } // anonymous namespace detail

// Use std time() rather than QTime::currentTime() to improve performance.
int
AnnotationGraphicsItemScheduler::nextMovingY(const QSizeF &itemSize, int visibleTime)
{
  Q_ASSERT(hub_);
  // min scale is 1/2 (0.5), LaneHeight is around 30, 2000 is the max vertical resolution
  static_assert(LaneCount * LaneHeight / 2 > 2000, "insufficient vertical lanes");

  int laneHeight = LaneHeight * scale_;
  if (!laneHeight)
    return 0;

  enum { WaitTime = 500 }; // in msecs, less than 1 second
  Q_ASSERT(visibleTime > 0);
  visibleTime += WaitTime;
  Q_ASSERT(visibleTime > 0);

  //int waitTime = 500; // in msecs
  //switch (style) {
  //case AnnotationGraphicsItem::FloatStyle:
  //case AnnotationGraphicsItem::FlyStyle: waitTime += visibleTime / 3; break;
  //default: waitTime += visibleTime;
  //}

  if (viewSize_.height() <= laneHeight * 2) // Do not schedule when window size is so small
    return 0;

  int itemHeight = qMax<int>(0, itemSize.height() - ItemHaloHeight);
  int itemLaneCount = itemHeight <= laneHeight ? 1 :
                      itemHeight / laneHeight + 1;
  Q_ASSERT(itemLaneCount);
  //qDebug () << "itemLaneCount =" << itemLaneCount << ", itemHeight =" << itemHeight << ", laneHeight =" << laneHeight;

  const qint64 now = QDateTime::currentMSecsSinceEpoch();
  int laneCount = qMin<int>(LaneCount, viewSize_.height() / laneHeight);

  qreal itemVelocity = (viewSize_.width() + itemSize.width()) / qreal(visibleTime);
  int bestLane = 0;
  qint64 minTime = now;
  for (int lane = 0; lane < laneCount - itemLaneCount + 1; lane++) {
    if (itemLaneCount == 1) {
      if (detail::available(now, itemVelocity, floatLane_[lane], viewSize_.width())) {
        bestLane = lane;
        break;
      }
    } else {
      bool best = true;
      for (int i = 0; i < itemLaneCount; i++)
        if (!detail::available(now, itemVelocity, floatLane_[lane+i], viewSize_.width())) {
          best = false;
          break;
        }
      if (best) {
        bestLane = lane;
        break;
      }
    }

    if (minTime > floatLane_[lane].time) {
      minTime = floatLane_[lane].time;
      bestLane = lane;
    }
  }

  //qDebug() << bestLane;

  if (itemLaneCount == 1) {
    Item &item = floatLane_[bestLane];
    item.time = now;
    item.life = visibleTime;
    item.width = itemSize.width();
  } else
    for (int i = 0; i < itemLaneCount; i++) {
      Item &item = floatLane_[bestLane+i];
      item.time = now;
      item.life = visibleTime;
      item.width = itemSize.width();
    }

  int windowHeader = !hub_->isNormalPlayerMode() && !hub_->isMediaTokenMode() ? 50 : 0;
  return bestLane * laneHeight + windowHeader;
}

// Use std time() rather than QTime::currentTime() to improve performance.
int
AnnotationGraphicsItemScheduler::nextStationaryY(int itemHeight, int visibleTime, AnnotationGraphicsItem::Style style)
{
  Q_ASSERT(hub_);
  // min scale is 1/2 (0.5), LaneHeight is around 30, 2000 is the max vertical resolution
  static_assert(LaneCount * LaneHeight / 2 > 2000, "insufficient vertical lanes");

  int laneHeight = LaneHeight * scale_;
  if (!laneHeight)
    return 0;

  enum { MinVisibleTime = 1000 }; // 1 second
  Q_ASSERT(visibleTime);
  if (visibleTime < MinVisibleTime)
    visibleTime = MinVisibleTime;

  //int waitTime = 500; // in msecs
  //switch (style) {
  //case AnnotationGraphicsItem::FloatStyle:
  //case AnnotationGraphicsItem::FlyStyle: waitTime += visibleTime / 3; break;
  //default: waitTime += visibleTime;
  //}

  if (viewSize_.height() <= laneHeight * 2) // Do not schedule when window size is so small
    return 0;

  bool top = false;
  if (style == AnnotationGraphicsItem::TopStyle ||
      style == AnnotationGraphicsItem::SubtitleStyle && AnnotationSettings::globalSettings()->isSubtitleOnTop())
    top = true;

  qint64 *laneTime = top ? topLaneTime_ : bottomLaneTime_;

  itemHeight = qMax<int>(0, itemHeight - ItemHaloHeight);
  int itemLaneCount = itemHeight <= laneHeight ? 1 :
                      itemHeight / laneHeight + 1;
  Q_ASSERT(itemLaneCount);
  //qDebug () << "itemLaneCount =" << itemLaneCount << ", itemHeight =" << itemHeight << ", laneHeight =" << laneHeight;

  const qint64 now = QDateTime::currentMSecsSinceEpoch();
  int laneCount = qMin<int>(LaneCount, viewSize_.height() / laneHeight);

  int bestLane = 0;
  qint64 minTime = now;
  for (int lane = 0; lane < laneCount - itemLaneCount + 1; lane++) {
    if (itemLaneCount == 1) {
      if (now >= visibleTime + laneTime[lane]) {
        bestLane = lane;
        break;
      }
    } else {
      bool best = true;
      for (int i = 0; i < itemLaneCount; i++)
        if (now < visibleTime + laneTime[lane + i]) {
          best = false;
          break;
        }
      if (best) {
        bestLane = lane;
        break;
      }
    }

    if (minTime > laneTime[lane]) {
      minTime = laneTime[lane];
      bestLane = lane;
    }
  }

  //qDebug() << bestLane;

  if (itemLaneCount == 1)
    laneTime[bestLane] = now;
  else
    for (int i = 0; i < itemLaneCount; i++)
      laneTime[bestLane + i] = now;

  if (hub_->isSignalTokenMode() && style == AnnotationGraphicsItem::SubtitleStyle)
    for (int i = 0; i < itemLaneCount; i++)
      subLaneStyle_[bestLane + i] = style;

  if (top) {
    int windowHeader = !hub_->isNormalPlayerMode() && !hub_->isMediaTokenMode() ? 50 : 0;
    return bestLane * laneHeight + windowHeader;
  } else { // bottom
    int windowFooter = !hub_->isNormalPlayerMode() ? int(laneHeight * 1.5) : 0;
    return viewSize_.height() - (bestLane + 2) * laneHeight - windowFooter;
  }
}

// - Motionless Scheduling -

QPointF
AnnotationGraphicsItemScheduler::nextStationaryPos(const QSizeF &itemSize, int visibleMsecs)
{
  enum { MarginLeft = 10 };
  enum { CellWidth = 250, CellFullScreenWidth = 300, CellHeight = int(LaneHeight * 1.2) };
  enum { MaxColumnCount = 2 }; // no more than 3 columns
  int cellWidth = scale_ * (hub_->isFullScreenWindowMode() ? CellFullScreenWidth : CellWidth),
      cellHeight = scale_ * CellHeight;
  if (!cellWidth || !cellHeight)
    return QPointF();
  int cellXCount = (viewSize_.width() - MarginLeft)  / cellWidth,
      cellYCount = viewSize_.height() / cellHeight - 1; // do not use the last row
  if (hub_->isSignalTokenMode())
    cellYCount = cellYCount * 3 / 4;
  if (cellXCount <= 0 || cellYCount <= 0)
    return QPointF();
  int itemXCount = itemSize.width() / cellWidth + 1; // TODO: itemYCount is ignored
  if (itemXCount > MaxColumnCount) // long annotations are skipped
    return QPointF();

  int maxXCount = cellXCount - itemXCount + 1;
  if (maxXCount <= 0)
    return QPointF();
  if (maxXCount > MaxColumnCount)
    maxXCount = MaxColumnCount;
  const qint64 now = QDateTime::currentMSecsSinceEpoch();
  bool ok = false;
  qint64 pausedTime = resumeTime_ - pauseTime_;
  int x, y;
  if (pausedTime >= 0 && (
        lastCell_.second >= now ||
        lastCell_.second < resumeTime_ && lastCell_.second > pauseTime_ && lastCell_.second + pausedTime > now
     )) {
    x = lastCell_.first.x();
    y = lastCell_.first.y();
    if (y == cellYCount - 1) {
      x++;
      y = 0;
    } else
      y++;

    if (x < maxXCount && y < cellYCount)
      for (int i = 0; i < itemXCount; i++) {
        qint64 t = cells_[QPoint(x + i, y)];
        if (t < resumeTime_ && t > pauseTime_)
          t += pausedTime;
        if (now < t)
          break;
        else if (i == itemXCount - 1)
          ok = true;
      }
  }
  if (!ok) {
    for (x = 0; x < maxXCount; x++) {
      for (y = 0; y < cellYCount; y++) {
        for (int i = 0; i < itemXCount; i++) {
          qint64 t = cells_[QPoint(x + i, y)];
          if (pausedTime && t > pauseTime_) {
            if (t < resumeTime_)
              t += pausedTime;
            else if (pausedTime < 0)
              break;
          }
          if (now < t)
            break;
          else if (i == itemXCount - 1)
            ok = true;
        }
        if (ok)
          break;
      }
      if (ok)
        break;
    }
    if (!ok)
      return QPointF();
  }

  //int cooldownTime = visibleMsecs + 1010;
  //int cooldownTime = visibleMsecs;
  enum { CooldownTime = 1010 };
  qint64 availableTime = now + visibleMsecs;
  lastCell_ = std::make_pair(QPoint(x, y), availableTime);
  for (int i = 0; i < itemXCount; i++)
    cells_[QPoint(x + i, y)] = i == 0 ?
      availableTime + CooldownTime :
      availableTime;
  return QPointF(MarginLeft + x * cellWidth, y * cellHeight);
}

// EOF
