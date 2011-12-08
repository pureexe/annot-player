#pragma once

// hook.h
// 7/10/2011
//
// Windows typedef: http://msdn.microsoft.com/en-us/library/aa383751(v=vs.85).aspx
// - HHOOK = HANDLE = void*
// - DWORD = usigned long = ulong (qt)
// Some Qt SDK types are inconsistent with with the official documentation.
// - Qt::HANDLE -> void* (qnamespace.h, Qt SDK 4.7.1)
// - Qt::HANDLE -> DWORD (<qt> documentation)
//
// NOTE: Current implementation is NOT thread safe!!!
// NOTE: It is required that one dll only allow one HookProc with SetWindowsHookEx!

#include "hook_config.h"
#include <QWidget>

QT_FORWARD_DECLARE_CLASS(QMouseEvent)

///  Hook event callbacks
class HOOKAPI WindowsHookListener
{
public:
  /**
   *  \brief  If the event is from hook listener.
   *
   *  This is a replacement for eventFilter, which requires the caller to be in the same thread
   *  which create the hook object.
   *
   *  Supported mouse event type:
   *  - QEvent::MouseMove
   *  - QEvent::MouseButtonPressed
   *  - QEvent::MouseButtonReleased
   *  - QEvent::MouseButtonDblClicked (left button only)
   *  - QEvent::Wheel
   *  - QEvent::ContextMenu (when enabled and will take place of right click)
   *  - QEvent::KeyPress
   *  - QEvent::KeyRelease
   *
   *  Limitations:
   *  - Keyboard events
   *    - modifiers  not implemented
   *    - repeat  not implemented
   *    - text  not implemented
   *  - Mouse events
   *    - Thread hook
   *      - type  MouseMove
   *      - button  NoButton
   *      - pos  supported
   *      - globalPos  supported
   *      - modifiers  not implemented
   *    - Global hook
   *      - type  MouseMove, MouseButtonPress, MouseButtonRelease, MouseButtonDblClick
   *      - button  NoButton, LeftButton, RightButton
   *      - pos  supported
   *      - globalPos  supported
   *      - modifiers  not implemented
   *
   *  FIXME: Double-click event always comes with press/release events.
   */
  virtual bool event(QEvent *event) = 0;
  virtual ~WindowsHookListener() { }
};

/*
class HOOKAPI WindowsHookAdaptor : public WindowsHookListener
{
protected:
  virtual bool event(QEvent *event);

  virtual void mouseMoveEvent(QMouseEvent *) { }
  virtual void mousePressEvent(QMouseEvent *) { }
  virtual void mouseReleaseEvent(QMouseEvent *) { }
  virtual void mouseDoubleClickEvent(QMouseEvent *) { }
  virtual void wheelEvent(QWheelEvent *) { }
  virtual void contextMenuEvent(QContextMenuEvent *) { }
};
*/

class WindowsHookManagerImpl; ///< \internal

///  This class should be used as a singleton.
class WindowsHookManager : public QObject
{
  Q_OBJECT
  typedef WindowsHookManager Self;
  typedef QObject Base;
  typedef WindowsHookManagerImpl Impl;

  Impl *impl_;

  // - Singleton -
public:
  HOOKAPI static Self *globalInstance();

  // - Options - TODO: it will be better to put these options into flags/setFlags methods.
public:

#ifdef USE_MOUSE_HOOK
  ///  This determines whether mouse events are available.
  HOOKAPI bool isMouseHookEnabled() const;
  ///  Enabled by default.
  HOOKAPI void setMouseHookEnabled(bool enabled = true);
#endif // USE_MOUSE_HOOK

#ifdef USE_KEYBOARD_HOOK
  ///  This determines whether keyboard events are available.
  HOOKAPI bool isKeyboardHookEnabled() const;
  ///  Enabled by default.
  HOOKAPI void setKeyboardHookEnabled(bool enabled = true);
#endif // USE_KEYBOARD_HOOK

  ///  Switch to global mode if false, and ignore added threads.
  HOOKAPI bool isThreadsFilterEnabled() const;
  ///  Enabled by default. Please invoke me before \fn start.
  HOOKAPI void setThreadsFilterEnabled(bool enabled = true);

  ///  Enabled by default. When enabled, only specfied windows would be hooked.
  HOOKAPI bool isWindowsFilterEnabled() const;
  ///  Please invoke me before \fn start.
  HOOKAPI void setWindowsFilterEnabled(bool enabled = true);

  ///  When enabled, QContextMenuEvent will take place QMouseEvent for right click.
  HOOKAPI bool isContextMenuEventEnabled() const;
  ///  Enabled by default.
  HOOKAPI void setContextMenuEventEnabled(bool enabled = true);

  ///  When enabled, MouseButtonDblClick type is generated when double clicked.
  HOOKAPI bool isDoubleClickEventEnabled() const;
  ///  Enabled by default.
  HOOKAPI void setDoubleClickEventEnabled(bool enabled = true);

  ///  When enabled, MouseMoveEvent is always forwarded if mouse hook is enabled, disgarding any filters.
  HOOKAPI bool isMouseMoveEventEscaped() const;
  ///  Enabled by default.
  HOOKAPI void setMouseMoveEventEscaped(bool enabled = true);

  HOOKAPI bool isActive();
public slots:
  HOOKAPI void start(); // hook all thread - TODO need a better name
  HOOKAPI void stop();

  // - Threads -
public:
  ///  Return true if thread was not been added before and was successfully added.
  HOOKAPI void addThreadId(ulong tid);
  ///  Return true if thread was added before and was successfully removed.
  HOOKAPI void removeThreadId(ulong tid);
  HOOKAPI bool containsThreadId(ulong tid) const;
  HOOKAPI bool hasThreads() const;
public slots:
  HOOKAPI void addCurrentThread();
  HOOKAPI void addAllThreadsInCurrentProcess();
  HOOKAPI void removeCurrentThread();
  HOOKAPI void removeAllThreads();

  // - Windows -
public:
  HOOKAPI void addWinId(WId hwnd);
  HOOKAPI void removeWinId(WId hwnd);
  HOOKAPI bool containsWinId(WId hwnd) const;
  HOOKAPI bool hasWindows() const;
public slots:
  HOOKAPI void removeAllWindows();

  // - Listeners -
public:
  HOOKAPI void addListener(WindowsHookListener *listener);
  HOOKAPI void removeListener(WindowsHookListener *listener);
  HOOKAPI bool containsListener(WindowsHookListener *listener) const;
  HOOKAPI bool hasListeners() const;
public slots:
  HOOKAPI void removeAllListeners();

public:
  //HOOKAPI bool empty() const; ///< No threads and no listeners
  HOOKAPI void clear();       ///< removeAllThreads and removeAllListneres

  // - Implementation -
protected:
  explicit WindowsHookManager(QObject *parent = 0);   ///< \internal
  ~WindowsHookManager();  ///< \internal
protected:
  virtual bool event(QEvent *e); ///< \internal

public:
  ///  \internal  Return hook for thread or 0 if not found
  void *hookHandleForThreadId(ulong tid, int procId) const;

  ///  \internal  Global hook handle.
  void *globalHookHandle(int procId) const;

  void removeAllHooks();  ///< \internal
};

// EOF
