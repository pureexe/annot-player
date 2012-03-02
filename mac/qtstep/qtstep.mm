// qtstep.mm
// 7/30/2011

#include "qtstep.h"
#include <QtGlobal>
#import <AppKit/AppKit.h>
#ifdef USE_MAC_CARBON
  #import <CarbonEvents.h>
#endif // USE_MAC_CARBON
//#import <objc/runtime.h>

#ifndef USE_MAC_CARBON
  #define GetCurrentEventTime()   nil
#endif // USE_MAC_CARBON

//#define DEBUG "qtstep"
//#include "module/debug/debug.h"

// - Type cast -

NSOBJECT_TYPE_REGISTER(nsobject_t, NSObject)
NSOBJECT_TYPE_REGISTER(nsresponder_t, NSResponder)
NSOBJECT_TYPE_REGISTER(nswindow_t, NSWindow)
NSOBJECT_TYPE_REGISTER(nsview_t, NSView)

// - Static initialization -

namespace { // anonymous, static
  class qtstep_global_ {
    NSAutoreleasePool *pool_;
  public:
    qtstep_global_()  { pool_ = [NSAutoreleasePool new]; }
    ~qtstep_global_() { [pool_ drain]; }
  };
  qtstep_global_ global_;
} // anonymous namespace

// - NSObject -

nsobject_t*
nsobject_new()
{
  NSObject *obj = [NSObject new];
  return nsobject_cast<nsobject_t *>(obj);
}

void
nsobject_release(nsobject_t *handle)
{
  NSObject *obj = nsobject_cast<NSObject *>(handle);
  [obj release] ;
}

// - NSResponder -

nsresponder_t*
nsresponder_new()
{
  NSResponder *obj = [NSResponder new];
  return nsobject_cast<nsresponder_t *>(obj);
}

void
nsresonder_release(nsresponder_t *handle)
{
  NSResponder *obj = nsobject_cast<NSResponder *>(handle);
  [obj release] ;
}

// - NSWindow -

nswindow_t*
nswindow_new()
{
  NSWindow *obj = [NSWindow new];
  return nsobject_cast<nswindow_t *>(obj);
}

void
nswindow_release(nswindow_t *handle)
{
  NSWindow *obj = nsobject_cast<NSWindow *>(handle);
  [obj release] ;
}

bool
nswindow_accepts_mouse_moved_events(const nswindow_t *handle)
{
  NSWindow *w = nsobject_cast<NSWindow *>(handle);
  return [w acceptsMouseMovedEvents] ;
}

void
nswindow_set_accepts_mouse_moved_events(nswindow_t *handle, bool t)
{
  NSWindow *w = nsobject_cast<NSWindow *>(handle);
  [w setAcceptsMouseMovedEvents:t] ;
}

// - NSView -

nsview_t*
nsview_new()
{
  NSView *obj = [NSView new];
  return nsobject_cast<nsview_t *>(obj);
}

void
nsview_release(nsview_t *handle)
{
  NSView *obj = nsobject_cast<NSView *>(handle);
  [obj release] ;
}

bool
nsview_is_hidden(const nsview_t *handle)
{
  NSView *view = nsobject_cast<NSView *>(handle);
  return [view isHidden] ;
}

void
nsview_set_hidden(nsview_t *handle, bool hidden)
{
  NSView *view = nsobject_cast<NSView *>(handle);
  [view setHidden:hidden] ;
}

nswindow_t*
nsview_window(nsview_t *handle)
{
  NSView *view = nsobject_cast<NSView *>(handle);
  NSWindow *window = [view window];
  return nsobject_cast<nswindow_t *>(window);
}

const nswindow_t*
nsview_window(const nsview_t *handle)
{
  NSView *view = nsobject_cast<NSView *>(handle);
  NSWindow *window = [view window];
  return nsobject_cast<nswindow_t *>(window);
}

// - QtStep -

// See: http://www.cocoadev.com/index.pl?NSEvent
// See: http://stackoverflow.com/questions/3873688/send-a-mouseevent-to-a-nsview-webview-from-code
void
QtStep::mouseClickEvent(nsview_t *handle, const QPoint &pos)
{
  NSView *view = nsobject_cast<NSView *>(handle);

  NSEvent *mouseDownEvent = [NSEvent
    mouseEventWithType:NSLeftMouseDown
    location:NSMakePoint(pos.x(), pos.y())
    modifierFlags:nil
    timestamp:GetCurrentEventTime()
    windowNumber:[[view window] windowNumber]
    context:nil
    eventNumber:nil
    clickCount:1
    pressure:nil
  ];
  [view mouseDown:mouseDownEvent];

  NSEvent *mouseUpEvent = [NSEvent
    mouseEventWithType:NSLeftMouseUp
    location:NSMakePoint(pos.x(), pos.y())
    modifierFlags:nil
    timestamp:GetCurrentEventTime()
    windowNumber:[[view window] windowNumber]
    context:nil
    eventNumber:nil
    clickCount:1
    pressure:nil
  ];
  [view mouseUp:mouseUpEvent];
}

// EOF
