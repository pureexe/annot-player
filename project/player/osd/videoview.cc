// videoview.cc
// 7/10/2011

#include "videoview.h"
#include "global.h"
#include <QtGui>

// - Hooks -
#ifdef USE_WIN_HOOK
#include "win/hook/hook.h"

#ifdef Q_WS_WIN
  #include "win/qtwin/qtwin.h"
#endif // Q_WS_WIN

//#define DEBUG "VideoView"
#include "module/debug/debug.h"

// This function is re-entrant, and \var tries is not synchornized
void
VideoView::addToWindowsHook()
{
  enum { VLC_CHILDREN_COUNT = 2 };
  enum { VLC_PLAYING_TIMEOUT = 200 }; // in msecs
  enum { MAX_TRIES = 50 }; // in total 10 secs
  static int tries = 0; // retries count

  DOUT("addToWindowsHook:enter: tries =" << tries);
  //if (children_.size() >= VLC_CHILDREN_COUNT) {
  if (tries > MAX_TRIES || children_.size() >= 2 * VLC_CHILDREN_COUNT) {
    // This should never happen.
    //Q_ASSERT(0);
    DOUT("addToWindowsHook:exit: max tries/children already reached");
    tries = 0;
    return;
  }

  WId hwnd = winId();
  while (hwnd = QtWin::getChildWindow(hwnd))
    if (children_.indexOf(hwnd) < 0) { // if not existed
      children_.append(hwnd);
      HOOK->addWinId(hwnd);
      DOUT("add hwnd to hook:" << hwnd << ", tries =" << tries);
    }

  // jichi 8/10/2011: VLC player usually added two child windows.

  //if (children_.size() < VLC_CHILDREN_COUNT) {
  //DOUT("not hook enough child windows, try again soon: tries =" << tries);
  if (tries < MAX_TRIES &&
      children_.size() < 2 * VLC_CHILDREN_COUNT)  {
    tries++;
    QTimer::singleShot(VLC_PLAYING_TIMEOUT, this, SLOT(addToWindowsHook()));
  } else {
    tries = 0;
    DOUT("max number of tries/children reached, stop watching child windows");
  }

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
  DOUT("removeFromWindowsHook:exit");
}

bool
VideoView::containsWindow(WId hwnd) const
{ return children_.contains(hwnd); }

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
  //setWindowOpacity(1.0); // opaque

#ifdef USE_MAC_VLCKIT
  view_ = ::vlcvideoview_new();
  //setCocoaView(view_);
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

// EOF

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

