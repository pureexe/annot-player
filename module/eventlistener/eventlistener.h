#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H

// eventlistener.h
// 7/14/2012

#include <QtCore/QObject>

class EventListener : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(EventListener)
  typedef EventListener Self;
  typedef QObject Base;

  bool enabled_;
public:
  explicit EventListener(QObject *parent = nullptr)
    : Base(parent), enabled_(true) { }

signals:
  void enabledChanged(bool t);

public:
  bool isEnabled() const { return enabled_; }

public slots:
  void setEnabled(bool t) { emit enabledChanged(enabled_ = t); }

protected:
  bool eventFilter(QObject *obj, QEvent *event) override
  {
    if (enabled_)
      eventListener(event);
    return Base::eventFilter(obj, event);
  }

  virtual void eventListener(QEvent *event) = 0;
};

#endif // EVENTLISTENER_H
