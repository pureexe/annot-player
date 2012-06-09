// annotationgraphicsitemscheduler.cc
// 7/16/2011

#include "annotationgraphicsitemscheduler.h"
#include "annotationgraphicsstyle.h"
#include "signalhub.h"
#include <QtCore>

//#define DEBUG "annotationgraphicsitemscheduler"
#include "module/debug/debug.h"

// - Properties -

enum { LaneHeight = ANNOTATION_SIZE_DEFAULT + ANNOTATION_SIZE_MARGIN * 2 + 6 }; // height of a piece of danmu

void
AnnotationGraphicsItemScheduler::pause()
{ pauseTime_ = QDateTime::currentMSecsSinceEpoch();  }

void
AnnotationGraphicsItemScheduler::resume()
{ resumeTime_ = QDateTime::currentMSecsSinceEpoch();  }

// - Float Scheduling -

// Use std time() rather than QTime::currentTime() to improve performance.
int
AnnotationGraphicsItemScheduler::nextY(int window_height, int visible_time, qreal scale, AnnotationGraphicsItem::Style style)
{
  Q_ASSERT(hub_);
  enum { LaneCount = 200 };                      // number of vertical lanes, large enough

  Q_ASSERT(LaneCount * LaneHeight * 0.5 > 1200); // min scale is 0.5

  int laneHeight = LaneHeight * scale;
  if (!laneHeight)
    return 0;

  static qint64 last_time_fly_[LaneCount],
                last_time_top_[LaneCount],
                last_time_bottom_[LaneCount];

  Q_ASSERT(visible_time > 0);
  int wait_time = 500;
  switch (style) {
  case AnnotationGraphicsItem::FloatStyle:
  case AnnotationGraphicsItem::FlyStyle: wait_time += visible_time / 5; break;
  default: wait_time += visible_time;
  }

  qint64 *last_time_;
  switch (style) {
  case AnnotationGraphicsItem::FloatStyle:
  case AnnotationGraphicsItem::FlyStyle:      last_time_ = last_time_fly_; break;
  case AnnotationGraphicsItem::TopStyle:      last_time_ = last_time_top_; break;
  case AnnotationGraphicsItem::BottomStyle:
  case AnnotationGraphicsItem::SubtitleStyle: last_time_ = last_time_bottom_; break;
  default : Q_ASSERT(0);      last_time_ = last_time_fly_;
  }

  if (window_height <= laneHeight * 2) // Do not schedule when window size is so small
    return 0;

  qint64 current_time = QDateTime::currentMSecsSinceEpoch();
  int count = qMin<int>(LaneCount, window_height / laneHeight);

  int best_lane = 0;
  qint64 max_time = current_time;
  for (int lane = 0; lane < count; lane++) {
    qint64 last_time = last_time_[lane]; // difftime is in seconds, while wait_time is in msecs
    if (current_time - last_time > wait_time) {
      best_lane = lane;
      break;
    } else if (max_time > last_time) {
      max_time = last_time;
      best_lane = lane;
    }
  }

  //qDebug() << best_lane;

  last_time_[best_lane] = current_time;

  switch (style) {
  case AnnotationGraphicsItem::BottomStyle:
  case AnnotationGraphicsItem::SubtitleStyle:
    {
      int window_footer = !hub_->isNormalPlayerMode() ? int(laneHeight * 1.5)   : 0;
      return window_height - (best_lane + 2) * laneHeight - window_footer;
    }
  default:
    {
      int window_header = !hub_->isNormalPlayerMode() && !hub_->isMediaTokenMode() ? 50 : 0;
      return best_lane * laneHeight + window_header;
    }
  }
}

// - Motionless Scheduling -

QPointF
AnnotationGraphicsItemScheduler::nextPos(const QSize &windowSize, const QSizeF &itemSize, int visibleMsecs, qreal scale)
{
  enum { MarginLeft = 10 };
  enum { CellWidth = 250, CellHeight = int(LaneHeight * 1.2) };
  enum { MaxColumnCount = 2 }; // no more than 3 columns
  int cellWidth = CellWidth * scale,
      cellHeight = CellHeight * scale;
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
  qint64 now = QDateTime::currentMSecsSinceEpoch();
  bool ok = false;
  int x, y;
  qint64 pausedTime = resumeTime_ - pauseTime_;
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

  int cooldownTime = visibleMsecs;
  qint64 availableTime = now + visibleMsecs;
  for (int i = 0; i < itemXCount; i++)
    cells_[QPoint(x + i, y)] = i == 0 ?
      availableTime + cooldownTime :
      availableTime;
  return QPointF(MarginLeft + x * cellWidth, y * cellHeight);
}

// EOF
