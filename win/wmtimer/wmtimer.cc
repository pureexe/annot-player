// wmtimer.cc
// 6/6/2012

#include "wmtimer.h"
#include <qt_windows.h>

//#define DEBUG "wmtimer"
#include "qtx/qxdebug.h"

namespace { namespace detail {
  VOID CALLBACK TimerProc(
    HWND hwnd,         // ウィンドウのハンドル
    UINT uMsg,         // WM_TIMER メッセージ
    UINT_PTR idEvent,  // タイマの識別子
    DWORD dwTime       // 現在のシステム時刻
  )
  {
    Q_UNUSED(hwnd)
    Q_UNUSED(dwTime)
    Q_UNUSED(uMsg)
    Q_ASSERT(idEvent);
    if (!idEvent)
      return;
    DOUT("enter");
    auto t = reinterpret_cast<WmTimer *>(idEvent);

    if (t->isSingleShot() && t->isActive())
      t->stop();
    t->trigger();
    DOUT("exit");
  }
} } // anonymous detail

// - Construction -

WId WmTimer::globalWindow_ = 0;

WId
WmTimer::createHiddenWindow()
{
  DOUT("enter: warning: hidden window used");
  QWidget *w = new QWidget;
  w->resize(QSize());
  w->show();
  DOUT("exit");
  return w->winId();
}

// - Timer -

void
WmTimer::start()
{
  DOUT("enter: active =" << active_ << ", interval =" << interval_);
  active_ = true;
  ::SetTimer(parentWindow_, reinterpret_cast<UINT_PTR>(this), interval_, detail::TimerProc);
  DOUT("exit");
}

void
WmTimer::stop()
{
  DOUT("enter: active =" << active_);
  active_ = false;
  ::KillTimer(parentWindow_, reinterpret_cast<UINT_PTR>(this));
  DOUT("exit");
}

// EOF
