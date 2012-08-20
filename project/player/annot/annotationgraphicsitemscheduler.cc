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

  qMemSet(flyLane_, 0, sizeof(flyLane_)/sizeof(*flyLane_));
  qMemSet(topLane_, 0, sizeof(topLane_)/sizeof(*topLane_));
  qMemSet(bottomLane_, 0, sizeof(bottomLane_)/sizeof(*bottomLane_));
  qMemSet(subLaneStyle_, 0, sizeof(subLaneStyle_)/sizeof(*subLaneStyle_));
}

void
AnnotationGraphicsItemScheduler::clearSubtitles()
{
  for (int i = 0; i < LaneCount; i++)
    if (subLaneStyle_[i]) {
      switch (subLaneStyle_[i]) {
      case AnnotationGraphicsItem::TopStyle: topLane_[i].time = 0; break;
      case AnnotationGraphicsItem::BottomStyle: bottomLane_[i].time = 0; break;
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
AnnotationGraphicsItemScheduler::nextY(const QSize &windowSize, const QSizeF &itemSize, int visibleTime, AnnotationGraphicsItem::Style style, bool sub)
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

  Item *laneItem;
  switch (style) {
  case AnnotationGraphicsItem::FloatStyle:
  case AnnotationGraphicsItem::FlyStyle:      laneItem = flyLane_; break;
  case AnnotationGraphicsItem::TopStyle:      laneItem = topLane_; break;
  case AnnotationGraphicsItem::BottomStyle:
  case AnnotationGraphicsItem::SubtitleStyle: laneItem = bottomLane_; break;
  default : Q_ASSERT(0);                      laneItem = flyLane_;
  }
  Q_ASSERT(laneItem);

  if (windowSize.height() <= laneHeight * 2) // Do not schedule when window size is so small
    return 0;

  enum { ItemHaloHeight = 2 * 2 };
  int itemHeight = qMax<int>(0, itemSize.height() - ItemHaloHeight);
  int itemLaneCount = itemHeight <= laneHeight ? 1 :
                      itemHeight / laneHeight + 1;
  Q_ASSERT(itemLaneCount);
  //qDebug () << "itemLaneCount =" << itemLaneCount << ", itemHeight =" << itemHeight << ", laneHeight =" << laneHeight;

  const qint64 now = QDateTime::currentMSecsSinceEpoch();
  int laneCount = qMin<int>(LaneCount, windowSize.height() / laneHeight);

  qreal itemVelocity = (windowSize.width() + itemSize.width()) / qreal(visibleTime);
  int bestLane = 0;
  qint64 minTime = now;
  for (int lane = 0; lane < laneCount - itemLaneCount + 1; lane++) {
    if (itemLaneCount == 1) {
      if (detail::available(now, itemVelocity, laneItem[lane], windowSize.width())) {
        bestLane = lane;
        break;
      }
    } else {
      bool best = true;
      for (int i = 0; i < itemLaneCount; i++) {
        if (!detail::available(now, itemVelocity, laneItem[lane+i], windowSize.width())) {
          best = false;
          break;
        }
      }
      if (best) {
        bestLane = lane;
        break;
      }
    }

    if (minTime > laneItem[lane].time) {
      minTime = laneItem[lane].time;
      bestLane = lane;
    }
  }

  //qDebug() << bestLane;

  if (itemLaneCount == 1) {
    Item &item = laneItem[bestLane];
    item.time = now;
    item.life = visibleTime;
    item.width = itemSize.width();
  } else
    for (int i = 0; i < itemLaneCount; i++) {
      Item &item = laneItem[bestLane+i];
      item.time = now;
      item.life = visibleTime;
      item.width = itemSize.width();
    }

  if (hub_->isSignalTokenMode() && sub)
    for (int i = 0; i < itemLaneCount; i++)
      subLaneStyle_[bestLane + i] = style;

  switch (style) {
  case AnnotationGraphicsItem::BottomStyle:
  case AnnotationGraphicsItem::SubtitleStyle:
    {
      int windowFooter = !hub_->isNormalPlayerMode() ? int(laneHeight * 1.5) : 0;
      return windowSize.height() - (bestLane + 2) * laneHeight - windowFooter;
    }
  default:
    {
      int windowHeader = !hub_->isNormalPlayerMode() && !hub_->isMediaTokenMode() ? 50 : 0;
      return bestLane * laneHeight + windowHeader;
    }
  }
}

// - Motionless Scheduling -

QPointF
AnnotationGraphicsItemScheduler::nextPos(const QSize &windowSize, const QSizeF &itemSize, int visibleMsecs)
{
  enum { MarginLeft = 10 };
  enum { CellWidth = 250, CellFullScreenWidth = 300, CellHeight = int(LaneHeight * 1.2) };
  enum { MaxColumnCount = 2 }; // no more than 3 columns
  int cellWidth = scale_ * (hub_->isFullScreenWindowMode() ? CellFullScreenWidth : CellWidth),
      cellHeight = scale_ * CellHeight;
  if (!cellWidth || !cellHeight)
    return QPointF();
  int cellXCount = (windowSize.width() - MarginLeft)  / cellWidth,
      cellYCount = windowSize.height() / cellHeight - 1; // do not use the last row
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
