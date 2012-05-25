#ifndef _VLCCORE_VIDEO_H
#define _VLCCORE_VIDEO_H

// vlccore/video.h
// 11/26/2011

#include <QtCore/QSize>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <cstdlib>
#include <utility>

struct libvlc_media_player_t;
struct input_thread_t;
struct vout_thread_t;

// http://mailman.videolan.org/pipermail/vlc-devel/2010-April/074742.html
// Adapt container window type
#ifdef Q_WS_WIN
#  define libvlc_media_player_set_drawable(_mp, _wid)   libvlc_media_player_set_hwnd(_mp, _wid)
#elif defined(Q_WS_MAC)
//#  define libvlc_media_player_set_drawable(_mp, _wid)   libvlc_media_player_set_agl(_mp, _wid)
#  define libvlc_media_player_set_drawable(_mp, _wid)   libvlc_media_player_set_nsobject(_mp, _wid)
#else // Linux
#  define libvlc_media_player_set_drawable(_mp, _wid)   libvlc_media_player_set_xwindow(_mp, _wid)
#endif // Q_WS_

#ifdef Q_WS_WIN
#  define libvlc_media_player_get_drawable(_mp)         libvlc_media_player_get_hwnd(_mp)
#elif defined Q_WS_MAC
//#  define libvlc_media_player_get_drawable(_mp)         libvlc_media_player_get_agl(_mp)
#  define libvlc_media_player_get_drawable(_mp)         libvlc_media_player_get_nsobject(_mp)
#else //Linux
#  define libvlc_media_player_get_drawable(_mp)         libvlc_media_player_get_xwindow(_mp)
#endif // Q_WS_

namespace vlccore {

  // Return a list of vout threads, save count to \param n.
  // See: vlc/lib/video.c
  vout_thread_t **GetVouts(libvlc_media_player_t *p_mi, size_t *n);

  ///  Return vout rendered size.
  QSize vout_dimension(vout_thread_t *vout);

  /**
   *  \param  vout  VLC vout thread
   *  \param  globalPos  global pos to map
   *  \param  voutRect  VLC vout window global rect
   *  \return  pos relative to vout window
   */
  QPoint vout_map_from_global(vout_thread_t *vout, const QPoint &globalPos, const QRect &voutRect);
  QPoint vout_map_to_widget(vout_thread_t *vout, const QPoint &voutPos, const QSize &widgetSize);

  enum vlcbutton {
    vlcbutton_left = 1,
#ifdef Q_WS_MAC
    vlcbutton_right = 1 << 1,
    vlcbutton_mid = 1 << 2
#else // only tested on Windows
    vlcbutton_mid = 1 << 1,
    vlcbutton_right = 1 << 2
#endif // Q_WS_MAC
  };
  typedef quint8 vlcbuttons;

  inline std::pair<
    Qt::MouseButton,
    Qt::MouseButtons
  >
  vlcbuttons_to_qt(int vbs)
  {
    Qt::MouseButton bt = Qt::NoButton;
    Qt::MouseButtons bts = Qt::NoButton;
    if (vbs) {
      if (vbs & vlcbutton_left) {
        bts |= Qt::LeftButton;
        bt = Qt::LeftButton;
      } if (vbs & vlcbutton_right) {
        bts |= Qt::RightButton;
        if (!bt) bt = Qt::RightButton;
      } if (vbs & vlcbutton_mid) {
        bts |= Qt::MidButton;
        if (!bt) bt = Qt::MidButton;
      }
    }
    return std::make_pair(bt, bts);
  }

  ///  \param pos is relative to rendered window rather than globalPos.
  void vout_mouse_down(vout_thread_t *vout, const QPoint &pos, vlcbutton button = vlcbutton_left);
  void vout_mouse_up(vout_thread_t *vout, const QPoint &pos, vlcbutton button = vlcbutton_left);
  void vout_mouse_moved(vout_thread_t *vout, const QPoint &pos);

  vlcbuttons vout_mouse_buttons(vout_thread_t *vout);
  QPoint vout_mouse_pos(vout_thread_t *vout);

  // - Fourcc -
  const char *codec_name(uint i_codec);

} // namespace vlccore

#endif // _VLCCORE_VIDEO_H
