#ifdef WITH_VLCCORE

// vlccore.h
// 11/26/2011

#ifdef _MSC_VER
  #pragma warning (disable:4819)       // C4819: The file contains a character that cannot be represented in the current code page.
  #pragma warning (disable:4996)       // C4996: MS' deprecated std functions orz.
#endif // _MSC_VER

#include "vlccore.h"
#include <QtCore>

extern "C" {
  #ifndef MODULE_STRING
    #define MODULE_STRING "main"  // needed by VLC
  #endif
  #include <inttypes.h>
  #include <vlc/plugins/vlc_input.h>
  #include <vlc/plugins/vlc_vout.h>
  //#include <vlc/plugins/vlc_vout_display.h>
  #include <vlc/vlc.h>
} // extern "C"

// - Internal types -

// Must be consistent with the one defined in:
// <vlc/src/control/media_player_internal.h>
struct libvlc_media_player_t
{
  VLC_COMMON_MEMBERS

  int i_refcount;
  vlc_mutex_t object_lock;

  struct {
    input_thread_t *p_thread;
    input_resource_t *p_resource;
    vlc_mutex_t lock;
  } input;

  struct libvlc_instance_t *p_libvlc_instance; // parent instance
  libvlc_media_t *p_md; // current media descriptor
  libvlc_event_manager_t *p_event_manager;
  libvlc_state_t state;
};

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

#endif // WITH_VLCCORE

// EOF
