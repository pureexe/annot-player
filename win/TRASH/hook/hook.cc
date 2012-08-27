// hook.cc
// 7/20/2011

#include "hook.h"
#include "qtwin/qtwin.h"
#include <QtGui>
#include <tuple>
#include <qt_windows.h>

#define DEBUG "hook"
//#define USE_MODE_DEBUG
#include "module/debug/debug.h"

#ifndef HOOK_DLL_NAME
# error "Please specify target DLL name."
#endif
#define HOOK_MODULE_NAME        HOOK_DLL_NAME

#define HOOK_MANAGER    WindowsHookManager::globalInstance()

enum HOOK_PROC_TYPE {
  HOOK_PROC_MOUSE = 0,
  HOOK_PROC_KEYBOARD,
  HOOK_PROC_COUNT
};

// - Helper -

namespace { // anonymous, type-cast

  inline QPoint POINT2QPoint(const POINT &pt)
  { return QPoint((int)pt.x, (int)pt.y); }

  inline POINT QPoint2POINT(const QPoint &pos)
  { POINT ret = { pos.x(), pos.y() }; return ret; }

} // anonymous namespace

// - Hook implementation -
// Specification: http://msdn.microsoft.com/en-us/library/ms632589(v=vs.85).aspx
// Tutorial: http://msdn.microsoft.com/en-us/library/ms644960(v=vs.85).aspx
// Example: http://msdn.microsoft.com/en-us/library/ms644959%28VS.85%29.aspx

#ifdef USE_MOUSE_HOOK

namespace { // anonymous, hook callbacks

  // Mouse hook struct: http://msdn.microsoft.com/en-us/library/ms644968(v=VS.85).aspx
  //
  // typedef struct tagMOUSEHOOKSTRUCT {
  //   POINT     pt;
  //   HWND      hwnd;
  //   UINT      wHitTestCode;
  //   ULONG_PTR dwExtraInfo;
  // } MOUSEHOOKSTRUCT, *PMOUSEHOOKSTRUCT, *LPMOUSEHOOKSTRUCT;
  //LPMOUSEHOOKSTRUCT pMouseHook = (MOUSEHOOKSTRUCT FAR *)lparam;
  LRESULT CALLBACK
  MouseProc(__in int nCode, __in WPARAM wparam, __in LPARAM lparam)
  {
    #define NEXT ::CallNextHookEx(hHook, nCode, wparam, lparam)
    HHOOK hHook = (HHOOK)HOOK_MANAGER->hookHandleForThreadId(::GetCurrentThreadId(), HOOK_PROC_MOUSE);
    Q_ASSERT(hHook);
    if (!hHook)
      return 0;

    // nCode can only be HC_ACTION(0) or HC_NOREMOVE (3)
    // See: http://msdn.microsoft.com/en-us/library/ms644988(v=vs.85).aspx
    if (nCode < 0 || !HOOK_MANAGER->isMouseHookEnabled())
      return NEXT;

    LPMOUSEHOOKSTRUCT lpMouseEvent = (LPMOUSEHOOKSTRUCT)lparam;
    Q_ASSERT(lpMouseEvent);
    HWND hwnd = lpMouseEvent->hwnd;
    if (!HOOK_MANAGER->isWindowsFilterEnabled()
        || HOOK_MANAGER->containsWinId(hwnd)) {

      QPoint globalPos = ::POINT2QPoint(lpMouseEvent->pt);
      QPoint pos = globalPos;
      if (hwnd) {
        RECT rect;
        if (::GetWindowRect(hwnd, &rect))
          pos -= QPoint(rect.left, rect.top);
      }

      // jichi 11/26/2011: Use sendEvent instead of postEvent, which caused troubles if events were sent out of order.
      //QCoreApplication::postEvent(HOOK_MANAGER,
      //  new QMouseEvent(QEvent::MouseMove, pos, globalPos, Qt::NoButton, Qt::NoButton, Qt::NoModifier)
      //);
      QMouseEvent e(QEvent::MouseMove, pos, globalPos, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
      QCoreApplication::sendEvent(HOOK_MANAGER, &e);
      if (e.isAccepted())
        return 0;
    }

    return NEXT;
    #undef NEXT
  }

} // anonymouss namespace

namespace { // anonymous, low level mouse procedure with double-click support

  LRESULT CALLBACK
  LowLevelMouseProc(__in int nCode, __in WPARAM wparam, __in LPARAM lparam); // Forward declaration.

  namespace shared_ {

    // Any bit that doesn't conflict with of MK_ event IDs (http://msdn.microsoft.com/en-us/library/ms645607(v=VS.85).aspx).
    // See: http://topic.csdn.net/t/20020318/10/582776.html
    enum { custom_event_bit = 0x8000 };

    // Timer ID. Could be any non-zero number.
    UINT_PTR timer_id; // This variable is needed to be synchronized, or there could be event leak.

    // Parameters of HookProc.
    typedef std::tuple<int, WPARAM, LPARAM> param_tuple;

    // Need a synchronized thread-safe queue here, implement a reentrant one myself.
    // But the overheads of synchronization is so high.
    // This structure is not thread-safe either, and could cause event leak. But I think it is still OK.
    //std::list<param_tuple> callbacks;
    enum {
      callbacks_reserved_size = 30,     // large enough to prevent out_of_bound.
      callbacks_size = 4                // maximum callbacks count
    };
    param_tuple callbacks[callbacks_reserved_size];     // Using boost::signal would be so expensive.
    int callbacks_count = 0;    // Not synchronized. Using CRITICAL_SECTION could be so expensive.

    inline UINT single_click_timeout() { return ::GetDoubleClickTime() / 1.2; } // Get this value at runtime to enable user runtime modification.
    inline UINT double_click_timeout() { return ::GetDoubleClickTime() * 2; }   // Used to recognize dblclick, maximal interval between D and DUDU

    inline void clear() { callbacks_count = 0; }

    // Flush callbacks.
    inline void flush()
    {
      //if (!callbacks.empty()) {
      //   for each(param_tuple param, callbacks)
      //    LowLevelMouseProc(param.get<0>(), param.get<1>(), param.get<2>());
      //  callbacks.clear();
      //}
      if (callbacks_count > 0) {
        for (int i = 0; i < callbacks_count; i++) {
          auto param = callbacks[i];
          ::LowLevelMouseProc(std::get<0>(param), std::get<1>(param), std::get<2>(param));
        }
        callbacks_count = 0;
      }
    }

  } // namespace shared

  // TimerProc: http://msdn.microsoft.com/en-us/library/ms644907(v=vs.85).aspx
  // SetTimer: http://msdn.microsoft.com/en-us/library/ms644906(v=vs.85).aspx
  // KillTimer: http://msdn.microsoft.com/en-us/library/ms644903(v=vs.85).aspx

  VOID CALLBACK
  TimerProc(__in HWND hwnd = 0, __in UINT uMsg = 0, __in UINT_PTR idEvent = 0, __in DWORD dwTime = 0)
  {
    Q_UNUSED(hwnd)
    Q_UNUSED(uMsg)
    Q_UNUSED(dwTime)
    //Q_ASSERT(!hwnd);
    //Q_ASSERT(idEvent == shared_::timer_id);

    if (idEvent)
      ::KillTimer(0, idEvent); // single shot

    shared_::flush();
  }

  // Return if matched doubleClick, and modify wparam.
  // Double click, see: http://topic.csdn.net/t/20020318/10/582776.html
  bool
  matchDoubleClickEvent_(__inout WPARAM *wparam)
  {
    enum HISTORY { RESET = 0x00, D = 0x01, DU = 0x02, DUD = 0x03 };
    enum BUTTON { NONE = 0x10, L = 0x20, R = 0x30 }; // Middle button is not considered

    static HISTORY history = RESET;
    static BUTTON button = NONE;
    static QTime last_time;

    bool matched = false;

    #define CASE(_l, _r) \
      case WM_##_l##BUTTONDOWN: \
        switch (button | history) { \
        case _l | DU: \
          history = DUD; \
          break; \
        case NONE | RESET: case _l | RESET: case _r | RESET: \
          button = _l; \
          history = D; \
          last_time = QTime::currentTime(); \
          break; \
        default: history = RESET; \
        } break; \
      case WM_##_l##BUTTONUP: \
        switch (button | history) { \
        case _l | D: \
          history = DU; \
          break; \
        case _l | DUD: { \
            history = RESET; \
            if ((UINT)last_time.msecsTo(QTime::currentTime()) < shared_::double_click_timeout()) { \
              *wparam = WM_##_l##BUTTONDBLCLK; \
              matched = true; \
            } \
          } break; \
        default: history = RESET; \
        } break;

    Q_ASSERT(wparam);
    switch (*wparam) {
    CASE(L, R)
    CASE(R, L)
    default: history = RESET;
    }

    return matched;
    #undef CASE
  }

  // Example: http://msdn.microsoft.com/en-us/library/ms644986(v=vs.85).aspx
  // Struct: http://msdn.microsoft.com/en-us/library/ms644970(v=vs.85).aspx
  //
  // typedef struct tagMSLLHOOKSTRUCT {
  //   POINT     pt;
  //   DWORD     mouseData;
  //   DWORD     flags;
  //   DWORD     time;
  //   ULONG_PTR dwExtraInfo;
  // } MSLLHOOKSTRUCT, *PMSLLHOOKSTRUCT, *LPMSLLHOOKSTRUCT;
  //
  LRESULT CALLBACK
  LowLevelMouseProc(__in int nCode, __in WPARAM wparam, __in LPARAM lparam)
  {
    HHOOK hHook = (HHOOK)HOOK_MANAGER->globalHookHandle(HOOK_PROC_MOUSE);
    Q_ASSERT(hHook);
    if (!hHook)
      return 0;
    #define NEXT ::CallNextHookEx(hHook, nCode, wparam, lparam)

    // nCode can only be HC_ACTION(0)
    // See: http://msdn.microsoft.com/en-us/library/ms644986(v=vs.85).aspx
    if (nCode < 0 || !HOOK_MANAGER->isMouseHookEnabled())
      return NEXT;

    LPMSLLHOOKSTRUCT lpMouseEvent = (LPMSLLHOOKSTRUCT)lparam;
    Q_ASSERT(lpMouseEvent);
    POINT pt = lpMouseEvent->pt;

    HWND hwnd = 0;
    if (HOOK_MANAGER->isDoubleClickEventEnabled()) {
      if (wparam & shared_::custom_event_bit)
        wparam &= ~shared_::custom_event_bit; // reset custom bit
      else {
        if (HOOK_MANAGER->isMouseMoveEventEscaped() && wparam == WM_MOUSEMOVE)
          ; // Escape this mouse event from windows filter
        else if (HOOK_MANAGER->isWindowsFilterEnabled()) {
          if (!HOOK_MANAGER->hasWindows())
            return NEXT;
          hwnd = ::WindowFromPoint(pt);
          if (hwnd && !HOOK_MANAGER->containsWinId(hwnd))
            return NEXT;
        }

        // Though double click event could be matched, the previous press/down events are not eliminated!!!
        ::matchDoubleClickEvent_(&wparam);

        switch (wparam) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:

          //shared_::callbacks.push_back(std::make_tuple(nCode, wparam | shared_::custom_event_bit, lparam));
          if (shared_::callbacks_count < shared_::callbacks_size)
            shared_::callbacks[shared_::callbacks_count++] =
              std::make_tuple(nCode, wparam | shared_::custom_event_bit, lparam);
          else
            shared_::flush();
          shared_::timer_id = ::SetTimer(0, shared_::timer_id, shared_::single_click_timeout(), ::TimerProc);
          return 0;

          // jichi 8/2/2011: Right button support is disabled to improve performance for contextMenuEvent.
        case WM_RBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDOWN:

        case WM_MOUSEMOVE:
        case WM_MOUSEHWHEEL:
        case WM_MOUSEWHEEL:
          shared_::flush();
          break;

        case WM_LBUTTONDBLCLK:
          shared_::clear();
          ::KillTimer(0, shared_::timer_id);
          break;

        default:
          Q_ASSERT(0);
        }
      }
    }

    QPoint globalPos(pt.x, pt.y);

    QPoint pos = globalPos;
    if (!hwnd)
      hwnd = ::WindowFromPoint(pt);
    if (hwnd) {
      RECT rect;
      if (::GetWindowRect(hwnd, &rect))
        pos -= QPoint(rect.left, rect.top);
    }

    bool accepted = false; // if sent event is accepted
    bool matched = true; // If mouse event matched

    // QMouseEvent
    QEvent::Type type;
    Qt::MouseButton button;

    // QWheelEvent
    //
    // http://doc.qt.nokia.com/latest/qwheelevent.html#delta
    // delta: distance that the wheel is rotated, in eighths of a degree.
    int delta;
    Qt::Orientation orient = Qt::Vertical;

    switch (wparam) {
    case WM_MOUSEMOVE:     type = QEvent::MouseMove;           button = Qt::NoButton; break;
    case WM_LBUTTONDOWN:   type = QEvent::MouseButtonPress;    button = Qt::LeftButton; break;
    case WM_LBUTTONUP:     type = QEvent::MouseButtonRelease;  button = Qt::LeftButton; break;
    case WM_MBUTTONDOWN:   type = QEvent::MouseButtonPress;    button = Qt::MiddleButton; break;
    case WM_MBUTTONUP:     type = QEvent::MouseButtonRelease;  button = Qt::MiddleButton; break;
    case WM_RBUTTONDOWN:   type = QEvent::MouseButtonPress;    button = Qt::RightButton; break;

    case WM_RBUTTONUP:
      if (HOOK_MANAGER->isContextMenuEventEnabled())           type = QEvent::ContextMenu;
      else {               type = QEvent::MouseButtonRelease;  button = Qt::RightButton; }
      break;

      // Non-standard
    case WM_LBUTTONDBLCLK: type = QEvent::MouseButtonDblClick; button = Qt::LeftButton; break;
    case WM_RBUTTONDBLCLK: type = QEvent::MouseButtonDblClick; button = Qt::RightButton; break;
    case WM_MBUTTONDBLCLK: type = QEvent::MouseButtonDblClick; button = Qt::MiddleButton; break; // Though never happens since not implemented
    //case RockLeft:
    //case RockRight:

      // Wheel
    case WM_MOUSEHWHEEL:
      orient = Qt::Horizontal;
    case WM_MOUSEWHEEL:
      type = QEvent::Wheel;
      button = Qt::NoButton;

      // Only direction make sense. The exact value of delta is useless.
      delta = HIWORD(lpMouseEvent->mouseData);
      if (delta != WHEEL_DELTA)
        delta = - WHEEL_DELTA;

      //else //(delta == WHEEL_DELTA), make this as click by wheel button
      //  delta = 0;

      break;

    default:
      matched = false;
    }

    Qt::MouseButtons buttons = button;
    Qt::KeyboardModifier modifiers = Qt::NoModifier;
    if (matched) {
      QEvent *event;

      switch (type) {
      case QEvent::ContextMenu: event = new QContextMenuEvent(QContextMenuEvent::Mouse, pos, globalPos); break;
      case QEvent::Wheel:       event = new QWheelEvent(pos, globalPos, delta, buttons, modifiers, orient); break;
      default:                  event = new QMouseEvent(type, pos, globalPos, button, buttons, modifiers);
      }

      // jichi 11/26/2011: Use sendEvent instead of postEvent, which caused troubles if events were sent out of order.
      //QCoreApplication::postEvent(HOOK_MANAGER, event);
      //accepted = true; // Always accept forwarded events
      QCoreApplication::sendEvent(HOOK_MANAGER, event);
      accepted = event->isAccepted();
      delete event;
    }

    if (accepted)
      return 0;
    else
      return NEXT;
    #undef NEXT
  }
} // anonymous namespace

#endif // USE_MOUSE_HOOK

#ifdef USE_KEYBOARD_HOOK

namespace { // anonymous, keyboard hook

  // See: http://msdn.microsoft.com/en-us/library/ms644984(v=vs.85).aspx
  LRESULT CALLBACK
  KeyboardProc(__in int nCode, __in WPARAM wparam, __in LPARAM lparam)
  {
    HHOOK hHook = (HHOOK)HOOK_MANAGER->globalHookHandle(HOOK_PROC_KEYBOARD);
    Q_ASSERT(hHook);
    if (!hHook)
      return 0;
    #define NEXT CallNextHookEx(hHook, nCode, wparam, lparam)

    // nCode can be HC_ACTION(0) or HC_ACTION(3)
    // See: http://msdn.microsoft.com/en-us/library/ms644984(v=vs.85).aspx
    if (nCode < 0 || !HOOK_MANAGER->isKeyboardHookEnabled())
      return NEXT;

    // See: http://msdn.microsoft.com/en-us/library/ms644984(v=vs.85).aspx
    // - wparam  The virtual-key code of the key that generated the keystroke message.
    //   See: http://msdn.microsoft.com/en-us/library/dd375731(v=vs.85).aspx
    // - lparam  The repeat count, scan code, extended-key flag, context code, previous key-state flag, and transition-state flag. For more information about the lParam parameter, see Keystroke Message Flags. The following table describes the bits of this value.
    //   See: http://msdn.microsoft.com/en-us/library/ms646267(v=vs.85).aspx#_win32_Keystroke_Message_Flags

    HWND hwnd = ::GetFocus();
    if (hwnd && HOOK_MANAGER->isWindowsFilterEnabled()
        && (!HOOK_MANAGER->hasWindows()
            || !HOOK_MANAGER->containsWinId(hwnd)))
      return NEXT;

    // See: http://msdn.microsoft.com/en-us/library/ms644984(v=vs.85).aspx
    //      http://msdn.microsoft.com/en-us/library/ms646267(v=vs.85).aspx#_win32_Keystroke_Message_Flags
    //  16-23   The scan code. The value depends on the OEM.
    //  24      Indicates whether the key is an extended key, such as a function key or a key on the numeric keypad. The value is 1 if the key is an extended key; otherwise, it is 0.
    //  25-28   Reserved.
    //  29      The context code. The value is 1 if the ALT key is down; otherwise, it is 0.
    //  30      The previous key state. The value is 1 if the key is down before the message is sent; it is 0 if the key is up.
    //  31      The transition state. The value is 0 if the key is being pressed and 1 if it is being released.oft.com/en-us/library/dd375731(v=vs.85).aspx
    //
    //  KF_ALTDOWN      Manipulates the ALT key flag, which indicates whether the ALT key is pressed.
    //  KF_DLGMODE      Manipulates the dialog mode flag, which indicates whether a dialog box is active.
    //  KF_EXTENDED     Manipulates the extended key flag.
    //  KF_MENUMODE     Manipulates the menu mode flag, which indicates whether a menu is active.
    //  KF_REPEAT       Manipulates the repeat count.
    //  KF_UP           Manipulates the transition state flag.

    bool released = lparam & KF_UP;
    int key = 0;

    // TODO: fill out other keys
    switch (wparam) {
    case VK_ESCAPE:
      key = Qt::Key_Escape;
      break;
    }

    if (key) {
      QEvent::Type type = released ? QEvent::KeyRelease : QEvent::KeyPress;
      Qt::KeyboardModifiers modifiers = Qt::NoModifier;
      QCoreApplication::postEvent(HOOK_MANAGER,
        new QKeyEvent(type, key, modifiers)
      );
      if (event.isAccepted())
        return 0;
    }


    return NEXT;
    #undef NEXT
  }

} // anonymous namespace

#endif // USE_KEYBOARD_HOOK

// - Windows Hook Manager Impl -

class WindowsHookManagerImpl
{
public:
  QHash<DWORD, QVector<HHOOK> > threadHooks;
  QVector<HHOOK> globalHooks; // Global hook.

  QList<WindowsHookListener*> listeners;

  QList<HWND> windows;

  bool active;
  bool mouseHookEnabled, keyboardHookEnabled;
  bool threadsFilterEnabled, windowsFilterEnabled;
  bool contextMenuEventEnabled, doubleClickEventEnabled;
  bool mouseMoveEventEscaped;

public:
  WindowsHookManagerImpl()
    : globalHooks(HOOK_PROC_COUNT), active(false),
      mouseHookEnabled(true), keyboardHookEnabled(true),
      threadsFilterEnabled(true), windowsFilterEnabled(true),
      contextMenuEventEnabled(true), doubleClickEventEnabled(true),
      mouseMoveEventEscaped(true)
  { }
};

// - Windows Hook Manager -

// + WindowsHookManager.Constructions +

WindowsHookManager*
WindowsHookManager::globalInstance()
{ static Self g; return &g; }

WindowsHookManager::WindowsHookManager(QObject *parent)
  : Base(parent) { impl_ = new Impl; }

WindowsHookManager::~WindowsHookManager()
{
  if (isActive())
    stop();

  Q_ASSERT(impl_);
  delete impl_;
}

void
WindowsHookManager::clear()
{
  removeAllHooks();
  removeAllListeners();
  removeAllWindows();
}

void*
WindowsHookManager::globalHookHandle(int procId) const
{
  if (procId < 0 || procId >= HOOK_PROC_COUNT)
    return 0;

  return impl_->globalHooks[procId];
}

#ifdef USE_MOUSE_HOOK
bool
WindowsHookManager::isMouseHookEnabled() const
{ return impl_->mouseHookEnabled; }

void
WindowsHookManager::setMouseHookEnabled(bool enabled)
{ impl_->mouseHookEnabled = enabled; }

#endif // USE_MOUSE_HOOK


#ifdef USE_KEYBOARD_HOOK
bool
WindowsHookManager::isKeyboardHookEnabled() const
{ return impl_->mouseHookEnabled; }

void
WindowsHookManager::setKeyboardHookEnabled(bool enabled)
{ impl_->mouseHookEnabled = enabled; }

#endif // USE_KEYBOARD_HOOK

bool
WindowsHookManager::isThreadsFilterEnabled() const
{ return impl_->threadsFilterEnabled; }

void
WindowsHookManager::setThreadsFilterEnabled(bool enabled)
{ impl_->threadsFilterEnabled = enabled; }

bool
WindowsHookManager::isWindowsFilterEnabled() const
{ return impl_->windowsFilterEnabled; }

void
WindowsHookManager::setWindowsFilterEnabled(bool enabled)
{ impl_->windowsFilterEnabled = enabled; }

bool
WindowsHookManager::isContextMenuEventEnabled() const
{ return impl_->contextMenuEventEnabled; }

void
WindowsHookManager::setContextMenuEventEnabled(bool enabled)
{ impl_->contextMenuEventEnabled = enabled; }

bool
WindowsHookManager::isDoubleClickEventEnabled() const
{ return impl_->doubleClickEventEnabled; }

void
WindowsHookManager::setDoubleClickEventEnabled(bool enabled)
{ impl_->doubleClickEventEnabled = enabled; }

bool
WindowsHookManager::isMouseMoveEventEscaped() const
{ return impl_->mouseMoveEventEscaped; }

void
WindowsHookManager::setMouseMoveEventEscaped(bool enabled)
{ impl_->mouseMoveEventEscaped = enabled; }

// + WindowsHookManager.Global +
void
WindowsHookManager::start()
{
  Q_ASSERT(!isActive());
  if (isActive())
    return;

  if (isThreadsFilterEnabled()) { // Thread mode
    foreach (DWORD dwThreadId, impl_->threadHooks.keys()) {
#ifdef USE_MOUSE_HOOK
      impl_->threadHooks[dwThreadId][HOOK_PROC_MOUSE] =
        ::SetWindowsHookEx(WH_MOUSE, ::MouseProc, 0, dwThreadId);
#endif // USE_MOUSE_HOOK

#ifdef USE_KEYBOARD_HOOK
      impl_->threadHooks[dwThreadId][HOOK_PROC_KEYBOARD] =
        ::SetWindowsHookEx(WH_KEYBOARD, ::KeyboardProc, 0, dwThreadId);
#endif // USE_KEYBOARD_HOOK
    }

  } else { // Global mode

    DOUT("DLL name:" << HOOK_MODULE_NAME);
    HINSTANCE hInstance = ::GetModuleHandle(HOOK_MODULE_NAME);
    Q_ASSERT(hInstance);
    if (!hInstance) // This mostly happens when dll is not available.
      return;

#ifdef USE_MOUSE_HOOK
    impl_->globalHooks[HOOK_PROC_MOUSE] =
        ::SetWindowsHookEx(WH_MOUSE_LL, ::LowLevelMouseProc, hInstance, 0);
#endif // USE_MOUSE_HOOK

#ifdef USE_KEYBOARD_HOOK
    impl_->globalHooks[HOOK_PROC_KEYBOARD] =
        ::SetWindowsHookEx(WH_KEYBOARD, ::KeyboardProc, hInstance, 0);
#endif // USE_KEYBOARD_HOOK
  }

  impl_->active = true;
}

bool
WindowsHookManager::isActive()
{ return impl_->active; }

void
WindowsHookManager::stop()
{
  if (!impl_->globalHooks.empty()) {
    foreach (HHOOK hHook, impl_->globalHooks)
      if (hHook)
        ::UnhookWindowsHookEx(hHook);
    impl_->globalHooks.fill(0);
  }

  if (!impl_->threadHooks.empty())
    foreach (DWORD tid, impl_->threadHooks.keys()) {
      foreach (HHOOK hHook, impl_->threadHooks[tid])
        if (hHook)
          ::UnhookWindowsHookEx(hHook);
      impl_->threadHooks[tid].fill(0);
    }

  impl_->active = false;
}

// + WindowsHookManager.Threads +
void
WindowsHookManager::addThreadId(DWORD dwThreadId)
{
  if (impl_->threadHooks.contains(dwThreadId))
    return;

  impl_->threadHooks[dwThreadId].resize(HOOK_PROC_COUNT);
  impl_->threadHooks[dwThreadId].fill(0);

  if (isActive() && isThreadsFilterEnabled()) { // Thread mode
#ifdef USE_MOUSE_HOOK
    impl_->threadHooks[dwThreadId][HOOK_PROC_MOUSE] =
        ::SetWindowsHookEx(WH_MOUSE, ::MouseProc, 0, dwThreadId);
#endif // USE_MOUSE_HOOK

#ifdef USE_KEYBOARD_HOOK
    impl_->threadHooks[dwThreadId][HOOK_PROC_KEYBOARD] =
        ::SetWindowsHookEx(WH_KEYBOARD, ::KeyboardProc, 0, dwThreadId);
#endif USE_KEYOARD_HOOK
  }
}

void
WindowsHookManager::removeThreadId(DWORD dwThreadId)
{
  if (!impl_->threadHooks.contains(dwThreadId))
    return;

  foreach (HHOOK hHook, impl_->threadHooks[dwThreadId])
    if (hHook)
      ::UnhookWindowsHookEx(hHook);
  impl_->threadHooks.remove(dwThreadId);
}

void
WindowsHookManager::addCurrentThread()
{ addThreadId(::GetCurrentThreadId()); }

void
WindowsHookManager::addAllThreadsInCurrentProcess()
{
  auto threads = QtWin::getThreadIdsByProcessId(::GetCurrentProcessId());
  if (!threads.empty())
    foreach (DWORD tid, threads)
      addThreadId(tid);
}

void
WindowsHookManager::removeCurrentThread()
{ removeThreadId(::GetCurrentThreadId()); }

void
WindowsHookManager::removeAllThreads()
{
  if (!impl_->threadHooks.empty()) {
    foreach (DWORD tid, impl_->threadHooks.keys())
      foreach (HHOOK hHook, impl_->threadHooks[tid])
        if (hHook)
          ::UnhookWindowsHookEx((HHOOK)hHook);
    impl_->threadHooks.clear();
  }
}

bool
WindowsHookManager::containsThreadId(ulong tid) const
{ return impl_->threadHooks.contains(tid); }

bool
WindowsHookManager::hasThreads() const
{ return !impl_->threadHooks.empty(); }

// + WindowsHookManager.Hooks +

void
WindowsHookManager::removeAllHooks()
{
  removeAllThreads();

  foreach (HHOOK hHook, impl_->globalHooks)
    if (hHook)
      ::UnhookWindowsHookEx(hHook);
  impl_->globalHooks.fill(0);
}

void*
WindowsHookManager::hookHandleForThreadId(DWORD dwThreadId, int procId) const
{
  if (procId < 0 || procId >= HOOK_PROC_COUNT)
    return 0;

  auto found = impl_->threadHooks.find(dwThreadId);
  if (found != impl_->threadHooks.end())
    return (*found)[procId];
  else
    return 0;
}

// + WindowsHookManager.Windows +
void
WindowsHookManager::addWinId(WId hwnd)
{
  if (hwnd && impl_->windows.indexOf(hwnd) < 0)
    impl_->windows.append(hwnd);
}

void
WindowsHookManager::removeWinId(WId hwnd)
{
  if (hwnd && impl_->windows.indexOf(hwnd) >= 0)
    impl_->windows.removeAll(hwnd);
}

void
WindowsHookManager::removeAllWindows()
{ impl_->windows.clear(); }

bool
WindowsHookManager::containsWinId(WId hwnd) const
{ return impl_->windows.indexOf(hwnd) >= 0; }

bool
WindowsHookManager::hasWindows() const
{ return !impl_->windows.empty(); }

// + WindowsHookManager.Listeners +

void
WindowsHookManager::addListener(WindowsHookListener *listener)
{
  if (listener && impl_->listeners.indexOf(listener) < 0)
    impl_->listeners.append(listener);
}

void
WindowsHookManager::removeListener(WindowsHookListener *listener)
{
  if (listener && impl_->listeners.indexOf(listener) >= 0)
    impl_->listeners.removeAll(listener);
}

void
WindowsHookManager::removeAllListeners()
{ impl_->listeners.clear(); }

bool
WindowsHookManager::containsListener(WindowsHookListener *listener) const
{ return impl_->listeners.indexOf(listener) >= 0; }

bool
WindowsHookManager::hasListeners() const
{ return !impl_->listeners.empty(); }

// + WindowsHookManager.Events +

//#define WH_EVENT_CALLBACK(_callback, _event) \
//  void \
//  WindowsHookManager::_callback(_event *event) \
//  { \
//    if (!listeners_.empty()) \
//      foreach (auto listener, listeners_) \
//        listener->_callback(event); \
//  }
//
//WH_EVENT_CALLBACK(mouseEvent, QMouseEvent)
//WH_EVENT_CALLBACK(wheelEvent, QWheelEvent)

bool
WindowsHookManager::event(QEvent *event)
{
  Q_ASSERT(event);
  switch (event->type()) {
  case QEvent::MouseMove:
  case QEvent::MouseButtonPress:
  case QEvent::MouseButtonRelease:
  case QEvent::MouseButtonDblClick:
  case QEvent::Wheel:
  case QEvent::ContextMenu:
    if (!impl_->listeners.empty())
      foreach (auto listener, impl_->listeners)
        listener->event(event);
    return true;
  }

  return false;
}

// EOF

// - Windows Hook Listener and Adaptor -

/*
bool
WindowsHookAdaptor::event(QEvent *event)
{
  Q_ASSERT(event);

  switch (event->type()) {
  case QEvent::MouseMove:           mouseMoveEvent(static_cast<QMouseEvent*>(event)); break;
  case QEvent::MouseButtonPress:    mousePressEvent(static_cast<QMouseEvent*>(event)); break;
  case QEvent::MouseButtonRelease:  mouseReleaseEvent(static_cast<QMouseEvent*>(event)); break;
  case QEvent::MouseButtonDblClick: mouseDoubleClickEvent(static_cast<QMouseEvent*>(event)); break;
  case QEvent::Wheel:               wheelEvent(static_cast<QWheelEvent*>(event)); break;
  case QEvent::ContextMenu:         contextMenuEvent(static_cast<QContextMenuEvent*>(event)); break;
  default: return false;
  }

  return true;
}
*/
