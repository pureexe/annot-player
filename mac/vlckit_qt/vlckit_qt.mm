// vlckit_qt.mm
// Since 7/30/2011

#include "vlckit_qt.h"
#import "VLCKit/VLCKit.h"
#include <QtGlobal>

// -- Helpers --
namespace { // anonymous, cast

  template <typename To, typename From>
  To vlcobject_cast(From x)
  { return __undefined_cast__(x); }

  template <>
  VLCVideoView*
  vlcobject_cast<VLCVideoView*>(vlcvideoview_t *handle)
  { return reinterpret_cast<objc_object*>(handle); }

  template <>
  vlcvideoview_t*
  vlcobject_cast<vlcvideoview_t*>(VLCVideoView *obj)
  { return reinterpret_cast<vlcvideoview_t*>(obj); }

  template <>
  vlcvideoview_t*
  vlcobject_cast<vlcvideoview_t*>(objc_object *obj)
  { return reinterpret_cast<vlcvideoview_t*>(obj); }

} // anonymous namespace

// -- Public API --
vlcvideoview_t*
vlcvideoview_new()
{
  VLCVideoView *obj = [VLCVideoView new];
  return vlcobject_cast<vlcvideoview_t*>(obj);
}

void
vlcvideoview_release(vlcvideoview_t *handle)
{
  Q_ASSERT(handle);
  VLCVideoView *obj = vlcobject_cast<VLCVideoView*>(handle);
  [obj release] ;
}


// EOF
