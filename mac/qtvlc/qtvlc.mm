// qtvlc.mm
// 7/30/2011
//
// Assume that global NSAutoreleasePool is already defined in qtstep module.

#include "qtvlc.h"
#import "qtvlcprivate.h"
#import "VLCKit/VLCKit.h"
#include <QtGlobal>
//#include <objc/runtime.h>

//#define DEBUG "qtvlc"
#include "module/debug/debug.h"

// - Type cast -

VLCOBJECT_TYPE_REGISTER(vlcvideoview_t, VLCVideoView)
VLCOBJECT_TYPE_REGISTER(vlcglview_t, VLCOpenGLVoutView)

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
  VLCVideoView *obj = vlcobject_cast<VLCVideoView*>(handle);
  [obj release] ;
}

vlcglview_t*
vlcvideoview_glview(vlcvideoview_t *handle)
{
  vlcglview_t *ret = 0;
  VLCVideoView *view = vlcobject_cast<VLCVideoView*>(handle);
  if (view.subviews.count) {
    VLCOpenGLVoutView *glview = (VLCOpenGLVoutView *)view.subviews.lastObject;
    ret = vlcobject_cast<vlcglview_t*>(glview);
  }
  return ret;
}

vlcvout_t*
vlcglview_vout(vlcglview_t *handle)
{
  VLCOpenGLVoutView *view = vlcobject_cast<VLCOpenGLVoutView*>(handle);
  VLCOpenGLVoutView_public *glview = reinterpret_cast<VLCOpenGLVoutView_public*>(view);

  Q_ASSERT(glview == glview->p_vout->p_sys->o_glview);
  return reinterpret_cast<vlcvout_t*>(glview->p_vout);
}

QPoint
vlcvout_map_from_global(vlcvout_t *vout, const QPoint &globalPos, const QRect &voutRect)
{
  if (!vout ||
      !voutRect.width() || !voutRect.height())
    return QPoint();

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
  int x = globalPos.x() - voutRect.x(),
      y = globalPos.y() - voutRect.y();

  // scale
  x = (x * vout->render.i_width) / voutRect.width();
  y = (y * vout->render.i_height) / voutRect.height();

  return QPoint(x, y);
}

// See: minimal_macosx/voutagl.m and maxosx/vout.m
void
vlcvout_mouse_down(vlcvout_t *vout, const QPoint &pos, vlcbutton button)
{
#ifndef WITH_VLCCORE
  Q_UNUSED(vout);
#else
  DOUT("vlcvout_mouse_down:enter");

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

  //if (!pos.isNull())
  vlcvout_mouse_moved(vout, pos);

  int32_t x, y;
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
  if (vout)
    ::var_SetCoords(vout, "mouse-moved", pos.x(), pos.y());

  DOUT("vlcvout_mouse_moved:exit");
#endif // WITH_VLCCORE
}

// EOF
