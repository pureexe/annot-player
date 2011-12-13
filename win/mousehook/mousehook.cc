// mousehook.cc
// 11/26/2011

#include "mousehook.h"
#include "qtwin/qtwin.h"
#include <qt_windows.h>
#include <QtGui>

#define DEBUG "mousehook"
#include "module/debug/debug.h"

#ifndef MOUSEHOOK_DLL_NAME
  #error "Please specify target DLL name."
#endif
#define MOUSEHOOK_MODULE_NAME        MOUSEHOOK_DLL_NAME

#define HOOKMAN  MouseHook::globalInstance()

// - Helper -

namespace { // anonymous, type-cast

  inline QPoint POINT2QPoint(const POINT &pt)
  { return QPoint((int)pt.x, (int)pt.y); }

  inline POINT QPoint2POINT(const QPoint &pos)
  { POINT ret = { pos.x(), pos.y() }; return ret; }

} // anonymous namespace

// - Hook implementation -

namespace { // anonymous, hook callbacks

  LRESULT CALLBACK
  MouseProc(__in int nCode, __in WPARAM wparam, __in LPARAM lparam)
  {
    #define NEXT ::CallNextHookEx(hHook, nCode, wparam, lparam)
    HHOOK hHook = (HHOOK)HOOKMAN->hook();
    Q_ASSERT(hHook);
    if (!hHook)
      return 0;

    // nCode can only be HC_ACTION(0) or HC_NOREMOVE (3)
    // See: http://msdn.microsoft.com/en-us/library/ms644988(v=vs.85).aspx
    if (nCode < 0 || !HOOKMAN->isActive())
      return NEXT;

    LPMOUSEHOOKSTRUCT lpMouseEvent = (LPMOUSEHOOKSTRUCT)lparam;
    Q_ASSERT(lpMouseEvent);
    QPoint globalPos = ::POINT2QPoint(lpMouseEvent->pt);
    QPoint pos = globalPos;

    if (HOOKMAN->isWindowPosEnabled()) {
      HWND hwnd = lpMouseEvent->hwnd;
      if (hwnd) {
        RECT rect;
        if (::GetWindowRect(hwnd, &rect))
          pos -= QPoint(rect.left, rect.top);
      }
    }

    QObject *listener = HOOKMAN->eventListener();
    if (listener) {
      QMouseEvent e(QEvent::MouseMove, pos, globalPos, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
      QCoreApplication::sendEvent(listener, &e);

      // Always return next.
      //if (e.isAccepted())
      //  return 0;
    }

    return NEXT;
    #undef NEXT
  }

} // anonymouss namespace

// - Constructions -

class MouseHookImpl
{
public:
  HHOOK hook;
  bool windowPosEnabled;
  QObject* listener;

public:
  MouseHookImpl() : hook(0), windowPosEnabled(false), listener(0) { }
};

MouseHook*
MouseHook::globalInstance()
{ static Self g; return &g; }

MouseHook::MouseHook()
{ impl_ = new Impl; }

MouseHook::~MouseHook()
{
  if (isActive())
    stop();

  Q_ASSERT(impl_);
  delete impl_;
}

// - Properties -

void*
MouseHook::hook() const
{ return impl_->hook; }

bool
MouseHook::isWindowPosEnabled() const
{ return impl_->windowPosEnabled; }

void
MouseHook::setWindowPosEnabled(bool enabled)
{ impl_->windowPosEnabled = enabled; }

QObject*
MouseHook::eventListener() const
{ return impl_->listener; }

void
MouseHook::setEventListener(QObject *obj)
{ impl_->listener = obj; }

// - Slots -

bool
MouseHook::isActive() const
{ return hook(); }

void
MouseHook::start()
{
  static HINSTANCE hInstance = 0;

  if (impl_->hook)
    return;

  if (!hInstance) {
    DOUT("start: DLL name:" << MOUSEHOOK_MODULE_NAME);
    hInstance = ::GetModuleHandle(MOUSEHOOK_MODULE_NAME);

    Q_ASSERT(hInstance);
    if (!hInstance) // This mostly happens when dll is not available.
      return;
  }

  // Global mode
  impl_->hook = ::SetWindowsHookEx(WH_MOUSE_LL, ::MouseProc, hInstance, 0);
  DOUT("start: started");
}

void
MouseHook::stop()
{
  if (impl_->hook) {
    ::UnhookWindowsHookEx((HHOOK)impl_->hook);
    impl_->hook = 0;
    DOUT("stop: stoppped");
  }
}

// EOF
