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

// -- Helpers --
namespace { // anonymous, cast

  template <typename To, typename From>
  To nsobject_cast(From x)
  { return __undefined_cast__(x); }

  template <>
  NSView*
  nsobject_cast<NSView*>(nsview_t *handle)
  { return reinterpret_cast<objc_object*>(handle); }

  template <>
  nsview_t*
  nsobject_cast<nsview_t*>(NSView *obj)
  { return reinterpret_cast<nsview_t*>(obj); }

  template <>
  nsview_t*
  nsobject_cast<nsview_t*>(objc_object *obj)
  { return reinterpret_cast<nsview_t*>(obj); }

  template <>
  const NSView*
  nsobject_cast<const NSView*>(const nsview_t *handle)
  { return reinterpret_cast<const objc_object*>(handle); }

  template <>
  const nsview_t*
  nsobject_cast<const nsview_t*>(const NSView *obj)
  { return reinterpret_cast<const nsview_t*>(obj); }

  template <>
  const nsview_t*
  nsobject_cast<const nsview_t*>(const objc_object *obj)
  { return reinterpret_cast<const nsview_t*>(obj); }

} // anonymous namespace

// -- Static initialization/destruction --
namespace { // anonymous, static

  class qtstep_global_
  {
    NSAutoreleasePool *pool;

  public:
    qtstep_global_()  { pool = [NSAutoreleasePool new]; }
    ~qtstep_global_() { [pool drain]; }

  };
  qtstep_global_ global_;

} // anonymous namespace

// -- Public API --
nsview_t*
nsview_new()
{
  NSView *view = [NSView new];
  return nsobject_cast<nsview_t*>(view);
}

void
nsview_release(nsview_t *handle)
{
  Q_ASSERT(handle);
  NSView *view = nsobject_cast<NSView*>(handle);
  [view release] ;
}

bool
nsview_is_hidden(const nsview_t *handle)
{
  Q_ASSERT(handle);
  const NSView *view = nsobject_cast<const NSView*>(handle);
  return [view isHidden] ;
}

void
nsview_set_hidden(nsview_t *handle, bool hidden)
{
  Q_ASSERT(handle);
  NSView *view = nsobject_cast<NSView*>(handle);
  [view setHidden:hidden] ;
}

// See: http://www.cocoadev.com/index.pl?NSEvent
// See: http://stackoverflow.com/questions/3873688/send-a-mouseevent-to-a-nsview-webview-from-code
void
QtStep::mouseClickEvent(nsview_t *handle, const QPoint &pos)
{
  Q_ASSERT(handle);
  NSView *view = nsobject_cast<NSView*>(handle);

  id mouseDownEvent = [NSEvent
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

  id mouseUpEvent = [NSEvent
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
