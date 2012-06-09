#ifndef ANNOTATIONGRAPHICSITEMSCHEDULER_H
#define ANNOTATIONGRAPHICSITEMSCHEDULER_H

// annotationgraphicsitemstyle.h
// 7/16/2011

#include "annotationgraphicsitem.h"
#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QPoint>

QT_FORWARD_DECLARE_CLASS(QWidget)

QT_BEGIN_NAMESPACE
inline uint qHash(const QPoint& p)
{
  // Assume that p.x() and p.y() are less than INT16_MAX (2^15)
  return qHash(p.x() << 16 | p.y());
}
QT_END_NAMESPACE

class SignalHub;
class AnnotationGraphicsItemScheduler : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationGraphicsItemScheduler)
  typedef AnnotationGraphicsItemScheduler Self;
  typedef QObject Base;

  SignalHub *hub_;
  qint64 pauseTime_,
         resumeTime_;

  QHash<QPoint, qint64> cells_; // timestamps for each cell

signals:
  void message(const QString &text);

public:
  explicit AnnotationGraphicsItemScheduler(SignalHub *hub, QObject *parent = 0)
    : Base(parent), hub_(hub), pauseTime_(0), resumeTime_(0) { Q_ASSERT(hub); }

public slots:
  void pause();
  void resume();
  void clear()
  { if (!cells_.isEmpty()) cells_.clear(); pauseTime_ = resumeTime_ = 0; }

  // - Float Scheduling -
public:
  int nextY(int windowHeight, int visibleMsecs, qreal scale, AnnotationGraphicsItem::Style style);

  // - Motionless Scheduling -
public:
  QPointF nextPos(const QSize &windowSize, const QSizeF &itemSize, int visibleMsecs, qreal scale);
};

#endif // ANNOTATIONGRAPHICSITEMSCHEDULER_H
