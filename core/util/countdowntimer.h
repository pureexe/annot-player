#ifndef _CORE_COUNTDOWNTIMER_H
#define _CORE_COUNTDOWNTIMER_H
// core/util/countdowntimer.h
// 11/27/2011

#include <QObject>
QT_FORWARD_DECLARE_CLASS(QTimer)

namespace Core {

  class CountdownTimer : public QObject
  {
    Q_OBJECT
    typedef CountdownTimer Self;
    typedef QObject Base;

  public:
    explicit CountdownTimer(QObject *parent = 0);

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

} // namespace Core;

#endif // _CORE_COUNTDOWNTIMER_H
