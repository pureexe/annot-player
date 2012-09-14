#ifndef VLCCOCOA_H
#define VLCCOCOA_H

// vlccocoa.h
// 7/30/2011

#ifdef WITH_MAC_QTCOCOA
# include "mac/qtcocoa/qtcocoa.h"
#else
# error "require lib qtcocoa"
#endif // WITH_MAC_QTCOCOA
#include <QtCore/QPoint>
#include <QtCore/QRect>

// - Types -

template <typename To, typename From>
inline To
vlcobject_cast(From x)
{ return __undefined_cast__(x); }

#define VLCOBJECT_TYPE_REGISTER(_type, _obj) \
  template <> \
  inline _obj *vlcobject_cast<_obj *>(_type *handle) \
  { return reinterpret_cast<_obj *>(handle); } \
  template <> \
  inline _type *vlcobject_cast<_type *>(_obj *obj) \
  { return reinterpret_cast<_type *>(obj); } \
  template <> \
  inline _type *vlcobject_cast<_type *>(objc_object *obj) \
  { return reinterpret_cast<_type *>(obj); }

struct vout_thread_t; ///< livlc native type
typedef vout_thread_t vlcvout_t;

struct vlcvideoview_t : public nsview_t { }; ///< VLCVidewView
struct vlcglview_t : public nsview_t { }; ///< VLCOpenGLVoutView

// - VLCVideoView -

vlcvideoview_t *vlcvideoview_new();
void vlcvideoview_release(vlcvideoview_t *view);

vlcglview_t *vlcvideoview_glview(vlcvideoview_t *view);

// - VLCOpenGLVoutView -

vlcvout_t *vlcglview_vout(vlcglview_t *view);

#endif // VLCCOCOA_H
