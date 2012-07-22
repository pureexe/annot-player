// vlccocoa.mm
// 7/30/2011
//
// Assume that global NSAutoreleasePool is already defined in qtcocoa module.

#include "vlccocoa/vlccocoa.h"
#import "vlccocoa/VLCOpenGLVoutView_public.h"
#import "VLCKit/VLCKit.h"
#include "module/vlccore/video.h"
//#include <objc/runtime.h>

//#define DEBUG "qtvlc"
#include "module/debug/debug.h"

// - Type cast -

VLCOBJECT_TYPE_REGISTER(vlcvideoview_t, VLCVideoView)
VLCOBJECT_TYPE_REGISTER(vlcglview_t, VLCOpenGLVoutView)

// - VLCVideoView -

vlcvideoview_t*
vlcvideoview_new()
{ return vlcobject_cast<vlcvideoview_t *>([VLCVideoView new]); }

void
vlcvideoview_release(vlcvideoview_t *handle)
{ [vlcobject_cast<VLCVideoView *>(handle) release]; }

vlcglview_t*
vlcvideoview_glview(vlcvideoview_t *handle)
{
  vlcglview_t *ret = 0;
  VLCVideoView *view = vlcobject_cast<VLCVideoView *>(handle);
  if (view.subviews.count) {
    VLCOpenGLVoutView *glview = (VLCOpenGLVoutView *)view.subviews.lastObject;
    ret = vlcobject_cast<vlcglview_t*>(glview);
  }
  return ret;
}

vlcvout_t*
vlcglview_vout(vlcglview_t *handle)
{
  VLCOpenGLVoutView *view = vlcobject_cast<VLCOpenGLVoutView *>(handle);
  VLCOpenGLVoutView_public *glview = reinterpret_cast<VLCOpenGLVoutView_public *>(view);

  //Q_ASSERT(glview == glview->p_vout->p_sys->o_glview);
  return glview->p_vout;
}

// EOF
