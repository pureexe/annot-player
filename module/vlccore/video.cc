// vlccore/video.cc
// See: vlc/control/video.cc
// 11/26/2011

#ifdef _MSC_VER
  #pragma warning (disable:4819)       // C4819: The file contains a character that cannot be represented in the current code page.
  #pragma warning (disable:4996)       // C4996: MS' deprecated std functions orz.
#endif // _MSC_VER

#include "module/vlccore/video.h"
#include <cassert>

//#define DEBUG "vlccore::video"
#include "module/debug/debug.h"
#include <QDebug>

#ifdef _MSC_VER
#  pragma warning (disable:4819)       // C4819: The file contains a character that cannot be represented in the current code page.
#  pragma warning (disable:4996)       // C4996: MS' deprecated std functions orz.
#endif // _MSC_VER

#ifndef MODULE_STRING
#  define MODULE_STRING "main"  // needed by VLC
#endif // MODULE_STRING
#ifdef _MSC_VER
#  include <inttypes.h>
#endif // _MSC_VER
#include <vlc/vlc.h>
#include <vlc/plugins/vlc_input.h>
#include <vlc/plugins/vlc_vout.h>
#include <vlc/plugins/vlc_vout_display.h>
#include <vlc/lib/media_player_internal.h>
#include <vlc/src/video_output/vout_internal.h>

// - vlc -

// Must be consistent with: vlc/lib/control/media_player.c
// Revision: VLC 2.0.0

namespace { // anonymous

  // Defined in vlc/src/control/media_player.c
  inline void lock_input(libvlc_media_player_t *mp)
  { ::vlc_mutex_lock(&mp->input.lock); }

  // Defined in vlc/src/control/media_player.c
  inline void unlock_input(libvlc_media_player_t *mp)
  { ::vlc_mutex_unlock(&mp->input.lock); }

} // anonymous namespace

input_thread_t*
libvlc_get_input_thread(libvlc_media_player_t *p_mi)
{
  input_thread_t *p_input_thread;
  Q_ASSERT(p_mi);

  lock_input(p_mi);
  p_input_thread = p_mi->input.p_thread;
  if( p_input_thread )
    vlc_object_hold( p_input_thread );
  else
    libvlc_printerr( "No active input" );
  unlock_input(p_mi);

  return p_input_thread;
}

// - vlccore -

// Return a list of vout threads, save count to \param n.
// See: vlc/lib/video.c
// Revision: VLC 2.0.0
vout_thread_t**
vlccore::GetVouts(libvlc_media_player_t *p_mi, size_t *n)
{
  Q_ASSERT(p_mi);
  input_thread_t *p_input = ::libvlc_get_input_thread(p_mi);
  if(!p_input) {
    *n = 0;
    return 0;
  }

  vout_thread_t **pp_vouts; // return
  if (::input_Control(p_input, INPUT_GET_VOUTS, &pp_vouts, n)) {
    *n = 0;
    pp_vouts = 0;
  }
  ::vlc_object_release(p_input);
  return pp_vouts;
}

// Extract (width, height) from vout.
// Revision: VLC 2.0.0
QSize
vlccore::vout_dimension(vout_thread_t *vout)
{
  //return  QSize(vout->render.i_width, vout->render.i_height);
  if (!vout)
    return QSize();
  vout_thread_sys_t *sys = vout->p;
  if (!sys)
    return QSize();
  vout_display_t *vd = sys->display.vd;
  if (!vd)
    return QSize();
  const vout_display_cfg_t *cfg = vd->cfg;
  if (!cfg)
    return QSize();
  return  QSize(cfg->display.width, cfg->display.height);
}

QPoint
vlccore::vout_map_to_widget(vout_thread_t *vout, const QPoint &voutPos, const QSize &widgetSize)
{
  QSize voutSize = vout_dimension(vout);
  if (voutSize.isEmpty() || voutSize == widgetSize)
    return voutPos;

 int x = (voutPos.x() * widgetSize.width()) / voutSize.width(),
     y = (voutPos.y() * widgetSize.height()) / voutSize.height();
 return QPoint(x, y);
}

QPoint
vlccore::vout_map_from_global(vout_thread_t *vout, const QPoint &globalPos, const QRect &voutRect)
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

  // render size
  QSize sz = vout_dimension(vout);
  if (sz.isEmpty())
    return QPoint();

  // offset
  int x = globalPos.x() - voutRect.x(),
      y = globalPos.y() - voutRect.y();

  // scale
  x = (x * sz.width()) / voutRect.width();
  y = (y * sz.height()) / voutRect.height();

  return QPoint(x, y);
}

// See: vlc/modules/gui/minimal_macosx/voutagl.m
void
vlccore::vout_mouse_down(vout_thread_t *vout, const QPoint &pos, vlcbutton button)
{
  DOUT("enter");

  //if (!pos.isNull())
  vout_mouse_moved(vout, pos);

  vlc_value_t val = { };
  ::var_Get(vout, "mouse-button-down", &val);
  val.i_int |= (int)button;
  ::var_Set(vout, "mouse-button-down", val);
  DOUT("exit");
}

void
vlccore::vout_mouse_up(vout_thread_t *vout, const QPoint &pos, vlcbutton button)
{
  DOUT("enter");

  //if (!pos.isNull())
  vout_mouse_moved(vout, pos);

  int32_t x, y;
  ::var_GetCoords(vout, "mouse-moved", &x, &y);
  ::var_SetCoords(vout, "mouse-clicked", x, y);

  vlc_value_t val = { };
  ::var_Get(vout, "mouse-button-down", &val);
  val.i_int &= ~(int)button;
  ::var_Set(vout, "mouse-button-down", val);

  DOUT("exit");
}

void
vlccore::vout_mouse_moved(vout_thread_t *vout, const QPoint &pos)
{
  DOUT("enter: pos =" << pos);
  if (vout)
    ::var_SetCoords(vout, "mouse-moved", pos.x(), pos.y());

  DOUT("exit");
}

QPoint
vlccore::vout_mouse_pos(vout_thread_t *vout)
{
  Q_ASSERT(vout);
  qint32 x = 0, y = 0;
  ::var_GetCoords(vout, "mouse-moved", &x, &y);
  return QPoint(x, y);
}


vlccore::vlcbuttons
vlccore::vout_mouse_buttons(vout_thread_t *vout)
{
  vlc_value_t btval = { };
  ::var_Get(vout, "mouse-button-down", &btval);
  return btval.i_int;
}

// EOF
