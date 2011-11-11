#ifndef VLCKIT_QT_H
#define VLCKIT_QT_H

// vlckit_qt.h
// 7/30/2011

#include "qtstep/qtstep.h"

// - VLCVideoView -
struct vlcvideoview_t : public nsview_t { };

vlcvideoview_t *vlcvideoview_new();
void vlcvideoview_release(vlcvideoview_t *handle);

#endif // VLCKIT_QT_H
