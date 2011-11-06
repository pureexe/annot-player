// qtstep.mm
// Since 7/30/2011

#include "qtstep.h"
#include <QtGlobal>
#import <AppKit/AppKit.h>

// -- Helpers --
namespace { // anonymous, cast

  template <typename To, typename From>
  To vlcobject_cast(From x)
  { return __undefined_cast__(x); }

  template <>
  NSView*
  vlcobject_cast<NSView*>(nsview_t *handle)
  { return reinterpret_cast<objc_object*>(handle); }

  template <>
  nsview_t*
  vlcobject_cast<nsview_t*>(NSView *obj)
  { return reinterpret_cast<nsview_t*>(obj); }

  template <>
  nsview_t*
  vlcobject_cast<nsview_t*>(objc_object *obj)
  { return reinterpret_cast<nsview_t*>(obj); }

  template <>
  const NSView*
  vlcobject_cast<const NSView*>(const nsview_t *handle)
  { return reinterpret_cast<const objc_object*>(handle); }

  template <>
  const nsview_t*
  vlcobject_cast<const nsview_t*>(const NSView *obj)
  { return reinterpret_cast<const nsview_t*>(obj); }

  template <>
  const nsview_t*
  vlcobject_cast<const nsview_t*>(const objc_object *obj)
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
  NSView *obj = [NSView new];
  return vlcobject_cast<nsview_t*>(obj);
}

void
nsview_release(nsview_t *handle)
{
  Q_ASSERT(handle);
  NSView *obj = vlcobject_cast<NSView*>(handle);
  [obj release] ;
}

bool
nsview_is_hidden(const nsview_t *handle)
{
  Q_ASSERT(handle);
  const NSView *obj = vlcobject_cast<const NSView*>(handle);
  return [obj isHidden] ;
}

void
nsview_set_hidden(nsview_t *handle, bool hidden)
{
  Q_ASSERT(handle);
  NSView *obj = vlcobject_cast<NSView*>(handle);
  [obj setHidden:hidden] ;
}

// EOF
