#pragma once

// wmtimer.h
// 6/6/2012

#include <QtGui/QWidget>
#include <boost/bind.hpp>
#include <boost/function.hpp>

class WmTimer
{
  typedef WmTimer Self;

  static WId globalWindow_;

  WId parentWindow_;
  qint64 interval_;
  bool singleShot_;
  bool active_;
  boost::function<void ()> callback_;

  // - Construction -
public:
  static void setGlobalWindow(WId winId) { globalWindow_ = winId; }

  static WId globalWindow()
  {
    if (!globalWindow_)
      globalWindow_ = createHiddenWindow();
    return globalWindow_;
  }

protected:
  static WId createHiddenWindow();

public:
  explicit WmTimer(WId parentWindow = 0)
    : parentWindow_(parentWindow ? parentWindow : globalWindow_), interval_(0),
      singleShot_(false), active_(false)
  { }

  // - Properties -
public:
  bool isSingleShot() const { return singleShot_; }
  void setSingleShot(bool t) { singleShot_ = t; }

  bool isActive() const { return active_; }

  WId parentWindow() const { return parentWindow_; }
  void setParentWindow(WId winId) { parentWindow_ = winId ? winId : globalWindow_; }

  qint64 interval() const { return interval_; }
  void setInterval(qint64 msecs) { interval_ = msecs; }

  void setCallback(const boost::function<void ()> &f) { callback_ = f; }

  template <typename Class, typename Member>
    void setCallback(Class *obj, Member mfunc)
    { callback_ = boost::bind(mfunc, obj); }

  // - Actions -
public:
  void trigger() { callback_(); }
  void stop();
  void start();
  void start(qint64 interval)
  { setInterval(interval); start(); }
};

// EOF
