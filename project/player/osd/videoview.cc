// videoview.cc
// 7/10/2011

#include "videoview.h"
#include "global.h"
#include <QtGui>

// - Hooks -
#ifdef USE_WIN_HOOK
#include "win/hook/hook.h"

#ifdef USE_WIN_QTWIN
#include "win/qtwin/qtwin.h"
#endif // USE_WIN_QTWIN

#define DEBUG "VideoView"
#include "module/debug/debug.h"

void
VideoView::addToWindowsHook()
{
  #define VLC_CHILDREN_COUNT  2
  #define VLC_PLAYING_TIMEOUT   100     // in msecs
  enum { max_retries = 30 };
  static int retries = 0; // retries count

  DOUT("addToWindowsHook:enter");
  if (children_.size() >= VLC_CHILDREN_COUNT) {
    DOUT("addToWindowsHook:exit: hooks already added");
    return;
  }

#ifdef USE_WIN_QTWIN
  WId hwnd = winId();
  while (hwnd = QtWin::getChildWindow(hwnd))
    if (history_.indexOf(hwnd) < 0) { // if not existed
      history_.push_back(hwnd);
      children_.push_back(hwnd);
      HOOK->addWinId(hwnd);
      DOUT("add hwnd to hook:" << hwnd);
    }

  // jichi 8/10/2011: VLC player usually added two child windows.

  if (children_.size() < VLC_CHILDREN_COUNT) {
    DOUT("not hook enough child windows, try again soon: retries =" << retries);
    if (retries < max_retries)  {
      QTimer::singleShot(VLC_PLAYING_TIMEOUT, this, SLOT(addToWindowsHook()));
      retries++;
    } else {
      DOUT("max number of retries reached, give up hooking child windows");
      retries = 0;
    }
  } else {
    DOUT("successfully hooked enough child windows");
    retries = 0;
  }
#endif // USE_WIN_QTWIN

  DOUT("addToWindowsHook:exit");
}

void
VideoView::removeFromWindowsHook()
{
  DOUT("removeFromWindowsHook:enter");
  if (!children_.empty()) {
    foreach (WId hwnd, children_)
      HOOK->removeWinId(hwnd);
    children_.clear();
    DOUT("hooked hwnd removed");
  }
  //history_.clear(); // history is never cleared
  DOUT("removeFromWindowsHook:exit");
}

#endif // USE_WIN_HOOK

// - VLCKit -
#if USE_MAC_VLCKIT
  #include "mac/vlckit_qt/vlckit_qt.h"
  #include "mac/qtstep/qtstep.h"
#endif // USE_MAC_VLCKIT

#ifdef Q_WS_MAC
  #define BASE(_parent)   Base(0, _parent)
#else
  #define BASE(_parent)   Base(_parent)
#endif // Q_WS_MAC

// - Constructions -
VideoView::VideoView(QWidget *parent)
  : BASE(parent)
{
  setContentsMargins(0, 0, 0, 0);
  //setAttribute(Qt::WA_TransparentForMouseEvents);

#ifdef USE_MAC_VLCKIT
  view_ = ::vlcvideoview_new();
  setCocoaView(view_);
#endif // USE_MAC_VLCKIT
}
#undef BASE

VideoView::~VideoView()
{
#ifdef Q_WS_MAC
  if (view_)
    ::vlcvideoview_release((vlcvideoview_t*)view_);
#endif // Q_WS_MAC
}

#ifdef Q_WS_MAC

void*
VideoView::view() const
{ return view_; }

bool
VideoView::isViewVisible() const
{
  //nsview_t *view = reinterpret_cast<nsview_t*>(cocoaView());
  //return view && !::nsview_is_hidden(view);
  return cocoaView();
}

void
VideoView::setViewVisible(bool visible)
{
  //nsview_t *view = reinterpret_cast<nsview_t*>(cocoaView());
  //if (view)
  //  ::nsview_set_hidden(view, !visible);
  if (visible) {
    if (!cocoaView())
      setCocoaView(view_);
  } else {
    if (cocoaView())
      setCocoaView(0);
  }
}

void
VideoView::showView()
{ setViewVisible(true); }

void
VideoView::hideView()
{ setViewVisible(false); }

#endif // Q_WS_MAC

/*
void
VideoView::mousePressEvent(QMouseEvent *event)
{
  if (event && event->button() == Qt::LeftButton) {
    dragPos_ = event->globalPos() - frameGeometry().topLeft();
    event->accept();
  }
}

void
VideoView::mouseMoveEvent(QMouseEvent *event)
{
  if (event && event->buttons() & Qt::LeftButton) {
    QPoint newPos = event->globalPos() - dragPos_;
    move(newPos);
    event->accept();
  }
}

void
VideoView::mouseReleaseEvent(QMouseEvent *event)
{ Q_UNUSED(event); }

void
VideoView::mouseDoubleClickEvent(QMouseEvent *event)
{
  if (event)
    event->accept();
}
*/

// EOF
