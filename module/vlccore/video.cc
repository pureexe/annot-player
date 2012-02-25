// vlccore/video.cc
// See: vlc/control/video.cc
// 11/26/2011

#ifdef _MSC_VER
  #pragma warning (disable:4819)       // C4819: The file contains a character that cannot be represented in the current code page.
  #pragma warning (disable:4996)       // C4996: MS' deprecated std functions orz.
#endif // _MSC_VER

#include "module/vlccore/video.h"
#include <QtCore>
#include "module/vlccore/types/media_player.h"

// - Helpers -

namespace { // anonymous

  // Defined in vlc/src/control/media_player.c
  inline void lock_input(libvlc_media_player_t *mp)
  { ::vlc_mutex_lock(&mp->input.lock); }

  // Defined in vlc/src/control/media_player.c
  inline void unlock_input(libvlc_media_player_t *mp)
  { ::vlc_mutex_unlock(&mp->input.lock); }

} // anonymous namespace

// - APIs -

// Declared in vlc/src/control/media_player_internal.h
// Defined in vlc/src/control/media_player.c
input_thread_t*
vlccore::libvlc_get_input_thread(libvlc_media_player_t *p_mi)
{
  input_thread_t *p_input_thread;
  Q_ASSERT(p_mi);

  ::lock_input(p_mi);
  p_input_thread = p_mi->input.p_thread;
  if( p_input_thread )
    ::vlc_object_hold(p_input_thread);
  //else
  //  libvlc_printerr( "No active input" );
  ::unlock_input(p_mi);

  return p_input_thread;
}

// Return a list of vout threads, save count to \param n.
// See: vlc/src/control/video.c
vout_thread_t**
vlccore::GetVouts(libvlc_media_player_t *p_mi, size_t *n)
{
  Q_ASSERT(p_mi);
  input_thread_t *p_input = libvlc_get_input_thread(p_mi);
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

// EOF
