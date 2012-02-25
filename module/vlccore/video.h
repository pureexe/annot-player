#ifndef _VLCCORE_VIDEO_H
#define _VLCCORE_VIDEO_H
// vlccore/video.h
// 11/26/2011

#include <cstdlib>

struct libvlc_media_player_t;
struct input_thread_t;
struct vout_thread_t;

namespace vlccore {

  // Declared in vlc/src/control/media_player_internal.h
  // Defined in vlc/src/control/media_player.c
  input_thread_t *libvlc_get_input_thread(libvlc_media_player_t *p_mi);

  // Return a list of vout threads, save count to \param n.
  // See: vlc/src/control/video.c
  vout_thread_t **GetVouts(libvlc_media_player_t *p_mi, size_t *n);

} // namespace vlccore

#endif // _VLCCORE_VIDEO_H
