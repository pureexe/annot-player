#ifndef QTVLC_H
#define QTVLC_H

// qtvlc.h
// 7/30/2011

#include "qtstep/qtstep.h"
#include <QPoint>
#include <QRect>

// - Types -

template <typename To, typename From>
inline To
vlcobject_cast(From x)
{ return __undefined_cast__(x); }

#define VLCOBJECT_TYPE_REGISTER(_type, _obj) \
  template <> \
  inline _obj *vlcobject_cast<_obj*>(_type *handle) \
  { return reinterpret_cast<objc_object*>(handle); } \
  template <> \
  inline _type *vlcobject_cast<_type*>(_obj *obj) \
  { return reinterpret_cast<_type*>(obj); } \
  template <> \
  inline _type *vlcobject_cast<_type*>(objc_object *obj) \
  { return reinterpret_cast<_type*>(obj); }

struct vout_thread_t; ///< livlc native type
typedef vout_thread_t vlcvout_t;

struct vlcvideoview_t : public nsview_t { }; ///< VLCVidewView
struct vlcglview_t : public nsview_t { }; ///< VLCOpenGLVoutView

enum vlcbutton {
  vlcbutton_left = 1,
  vlcbutton_right = 1 << 1,
  vlcbutton_mid = 1 << 2
};

// - VLCVideoView -

vlcvideoview_t *vlcvideoview_new();
void vlcvideoview_release(vlcvideoview_t *view);

vlcglview_t *vlcvideoview_glview(vlcvideoview_t *view);

// - VLCOpenGLVoutView -

vlcvout_t *vlcglview_vout(vlcglview_t *view);

/**
 *  \param  vout  VLC vout thread
 *  \param  globalPos  global pos to map
 *  \param  voutRect  VLC vout window global rect
 *  \return  pos relative to vout window
 */
QPoint vlcvout_map_from_global(vlcvout_t *vout, const QPoint &globalPos, const QRect &voutRect);

///  \param pos is relative to rendered window rather than globalPos.
void vlcvout_mouse_down(vlcvout_t *vout, const QPoint &pos, vlcbutton button = vlcbutton_left);
void vlcvout_mouse_up(vlcvout_t *vout, const QPoint &pos, vlcbutton button = vlcbutton_left);
void vlcvout_mouse_moved(vlcvout_t *vout, const QPoint &pos);

#endif // QTVLC_H
