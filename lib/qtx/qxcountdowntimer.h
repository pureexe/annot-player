#ifndef QXCOUNTDOWNTIMER_H
#define QXCOUNTDOWNTIMER_H

// qxcountdowntimer.h
// 11/27/2011

#include "qtx/qtxglobal.h"
#include <QtCore/QObject>

QT_FORWARD_DECLARE_CLASS(QTimer)

QTX_BEGIN_NAMESPACE

class QxCountdownTimer : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(QxCountdownTimer)
  typedef QxCountdownTimer Self;
  typedef QObject Base;

public:
  explicit QxCountdownTimer(QObject *parent = nullptr)
    : Base(parent), count_(0) { init(); }
private:
  void init();

signals:
  void timeout();

public:
  int interval() const;

  bool isActive() const;
  int count() const;

public slots:
  void start(int count = 0); ///< use previous count if \param count is zero
  void stop();

  void setInterval(int msecs);
  void setCount(int count);

  // - Implementation -

protected slots:
  void decreaseCount();
  void invalidateCount();

private:
  QTimer *timer_;
  qint64 count_;
};

QTX_END_NAMESPACE

#endif // QXCOUNTDOWNTIMER_H
