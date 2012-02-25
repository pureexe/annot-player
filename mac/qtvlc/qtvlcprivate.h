// qtvlcprivate.h
// Objective C header.

#ifndef MODULE_STRING
  #define MODULE_STRING "main" // VLC module string, defined in VLC's Makefile
#endif // MODULE_STRING
#import <vlc/modules/gui/minimal_macosx/VLCOpenGLVoutView.h>
#import <vlc/modules/gui/minimal_macosx/voutgl.h> // declare vout_sys_t
#include <vlc/plugins/vlc_vout.h> // declare vout_thread_t

// - VLCOpenGLVoutView -

// See: minimal_macosx/VLCOpenGLVoutView.h
// Used for hacking VLCOpenGLVoutView
@interface VLCOpenGLVoutView_public : NSOpenGLView
{
@public
  id <VLCOpenGLVoutEmbedding> container;
  vout_thread_t * p_vout;
  NSLock *objectLock;
}
@end

@implementation VLCOpenGLVoutView_public
@end

// EOF
