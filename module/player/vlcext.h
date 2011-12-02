#ifndef VLCEXT_H
#define VLCEXT_H
// vlcext.h
// 11/26/2011

#include <QtGlobal>
#include <boost/tuple/tuple.hpp>

// - Constants -

#define VLC_MAX_VOLUME 100.0

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

// - Helper functions -

// http://mailman.videolan.org/pipermail/vlc-devel/2010-April/074742.html
// Adapt container window type
#ifdef Q_WS_WIN
  #define libvlc_media_player_set_drawable(_mp, _wid)   libvlc_media_player_set_hwnd(_mp, _wid)
#elif defined(Q_WS_MAC)
  //#define libvlc_media_player_set_drawable(_mp, _wid)   libvlc_media_player_set_agl(_mp, _wid)
  #define libvlc_media_player_set_drawable(_mp, _wid)   libvlc_media_player_set_nsobject(_mp, _wid)
#else // Linux
  #define libvlc_media_player_set_drawable(_mp, _wid)   libvlc_media_player_set_xwindow(_mp, _wid)
#endif // Q_WS_

#ifdef Q_WS_WIN
  #define libvlc_media_player_get_drawable(_mp)         libvlc_media_player_get_hwnd(_mp)
#elif defined(Q_WS_MAC)
  //#define libvlc_media_player_get_drawable(_mp)         libvlc_media_player_get_agl(_mp)
  #define libvlc_media_player_get_drawable(_mp)         libvlc_media_player_get_nsobject(_mp)
#else //Linux
  #define libvlc_media_player_get_drawable(_mp)         libvlc_media_player_get_xwindow(_mp)
#endif // Q_WS_

namespace { // anonymous

  inline boost::tuple<
    Qt::MouseButton,
    Qt::MouseButtons
  >
  qt_buttons_from_vlc_buttons(int vbs)
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
    return boost::make_tuple(bt, bts);
  }

} // anonymous namespace

#endif // VLCEXT_H
