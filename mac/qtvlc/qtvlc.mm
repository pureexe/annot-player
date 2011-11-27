// qtvlc.mm
// 7/30/2011

#include "qtvlc.h"
#import "VLCKit/VLCKit.h"

#import "minimal_macosx/voutgl.h" // declaration for vout_sys_t
extern "C" {
  #ifndef MODULE_STRING
    #define MODULE_STRING "main" // VLC module string, defined in VLC's Makefile
  #endif // MODULE_STRING
  #include <vlc/plugins/vlc_vout.h> // declaration of vout_thread_t
} // extern "C"

#include <QtGlobal>
#include <objc/runtime.h>

#define DEBUG "qtvlc"
#include "module/debug/debug.h"

// - Helpers: Type cast -

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

  template <>
  VLCOpenGLVoutView*
  vlcobject_cast<VLCOpenGLVoutView*>(vlcglview_t *handle)
  { return reinterpret_cast<objc_object*>(handle); }

  template <>
  vlcglview_t*
  vlcobject_cast<vlcglview_t*>(VLCOpenGLVoutView *obj)
  { return reinterpret_cast<vlcglview_t*>(obj); }

  template <>
  vlcglview_t*
  vlcobject_cast<vlcglview_t*>(objc_object *obj)
  { return reinterpret_cast<vlcglview_t*>(obj); }

} // anonymous namespace

// - VLCVideoView -

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

vlcglview_t*
vlcvideoview_glview(vlcvideoview_t *handle)
{
  Q_ASSERT(handle);
  vlcglview_t *ret = 0;
  VLCVideoView *view = vlcobject_cast<VLCVideoView*>(handle);
  if (view.subviews.count) {
    VLCOpenGLVoutView *glview = (VLCOpenGLVoutView *)view.subviews.lastObject;
    ret = vlcobject_cast<vlcglview_t*>(glview);
  }
  return ret;
}

// - VLCOpenGLVoutView -

extern "C" { // Objective C

// See: minimal_macosx/VLCOpenGLVoutView.h
// Used for hacking VLCOpenGLVoutView
@interface VLCOpenGLVoutView_public : NSOpenGLView
{
@public
  id <VLCOpenGLVoutEmbedding> container;
  vout_thread_t * p_vout;
  NSLock *objectLock;
}
@end

@implementation VLCOpenGLVoutView_public
@end

} // extern "C"

vlcvout_t*
vlcglview_vout(vlcglview_t *handle)
{
  Q_ASSERT(handle);
  VLCOpenGLVoutView *view = vlcobject_cast<VLCOpenGLVoutView*>(handle);
  VLCOpenGLVoutView_public *glview = reinterpret_cast<VLCOpenGLVoutView_public*>(view);

  Q_ASSERT(glview == glview->p_vout->p_sys->o_glview);
  return reinterpret_cast<vlcvout_t*>(glview->p_vout);
}

QPoint
vlcvout_map_from_global(vlcvout_t *vout, const QPoint &globalPos, const QRect &voutRect)
{
  Q_ASSERT(vout);
  QPoint ret;
  if (!vout ||
      !voutRect.width() || !voutRect.height())
    return ret;

  // jichi 11/26/2011 FIXME: All coordinates in p_sys are zero.
  // p_sys is not properly initialized.
  //
  //uint i_height = vout->p_sys->i_height,
  //     i_width  = vout->p_sys->i_width;
  //uint i_x, i_y;
  //::vout_PlacePicture(vout, i_width, i_height,
  //                    &i_x, &i_y, &i_width, &i_height);
  //int x = (pos.x() - i_x) * vout->render.i_width / i_width,
  //    y = (pos.y() - i_y) * vout->render.i_height / i_height;

  // offset
  ret = globalPos - voutRect.topLeft();

  // scale
  ret.rx() *= (double)vout->render.i_width / voutRect.width();
  ret.ry() *= (double)vout->render.i_height / voutRect.height();

  return ret;
}

// See: minimal_macosx/voutagl.m and maxosx/vout.m
void
vlcvout_mouse_down(vlcvout_t *vout, const QPoint &pos, vlcbutton button)
{
#ifndef WITH_VLCCORE
  Q_UNUSED(vout);
#else
  DOUT("vlcvout_mouse_down:enter");
  Q_ASSERT(vout);

  //if (!pos.isNull())
  vlcvout_mouse_moved(vout, pos);

  vlc_value_t val;
  ::var_Get(vout, "mouse-button-down", &val);
  val.i_int |= button;
  ::var_Set(vout, "mouse-button-down", val);

  DOUT("vlcvout_mouse_down:exit");
#endif // WITH_VLCCORE
}

void
vlcvout_mouse_up(vlcvout_t *vout, const QPoint &pos, vlcbutton button)
{
#ifndef WITH_VLCCORE
  Q_UNUSED(vout);
#else
  DOUT("vlcvout_mouse_up:enter");
  Q_ASSERT(vout);

  //if (!pos.isNull())
  vlcvout_mouse_moved(vout, pos);

  int x, y;
  ::var_GetCoords(vout, "mouse-moved", &x, &y);
  ::var_SetCoords(vout, "mouse-clicked", x, y);

  vlc_value_t val;
  ::var_Get(vout, "mouse-button-down", &val);
  val.i_int &= ~button;
  ::var_Set(vout, "mouse-button-down", val);

  DOUT("vlcvout_mouse_up:exit");
#endif // WITH_VLCCORE
}

void
vlcvout_mouse_moved(vlcvout_t *vout, const QPoint &pos)
{
#ifndef WITH_VLCCORE
  Q_UNUSED(vout);
  Q_UNUSED(pos);
#else
  DOUT("vlcvout_mouse_moved:enter: pos =" << pos);
  Q_ASSERT(vout);
  if (vout)
    ::var_SetCoords(vout, "mouse-moved", pos.x(), pos.y());

  DOUT("vlcvout_mouse_moved:exit");
#endif // WITH_VLCCORE
}

// EOF
