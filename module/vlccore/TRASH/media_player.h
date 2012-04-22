#ifndef _VLCCORE_MEDIA_PLAYER_H
#define _VLCCORE_MEDIA_PLAYER_H
// vlccore/media_player.h
// See: vlc/src/control/media_player_internal.h
// 11/26/2011

#ifdef _MSC_VER
  #pragma warning (disable:4819)       // C4819: The file contains a character that cannot be represented in the current code page.
  #pragma warning (disable:4996)       // C4996: MS' deprecated std functions orz.
#endif // _MSC_VER

#ifndef MODULE_STRING
#  define MODULE_STRING "main"  // needed by VLC
#endif // MODULE_STRING
#ifdef _MSC_VER
#  include <inttypes.h>
#endif // _MSC_VER
#include <vlc/plugins/vlc_input.h>
#include <vlc/plugins/vlc_vout.h>
//#include <vlc/plugins/vlc_vout_display.h>
#include <vlc/vlc.h>

// - Internal types -

// Must be consistent with the one defined in: media_player_internal.h
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

#endif // _VLCCORE_MEDIA_PLAYER_H
