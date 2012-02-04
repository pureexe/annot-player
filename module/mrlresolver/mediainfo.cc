// mediainfo.cc
// 2/3/2012

#include "mediainfo.h"

namespace { // anonymous
  struct init_ { init_() {
    qRegisterMetaType<MediaInfo>("MediaInfo");
  } };
  init_ static_init_;
} // anonymois namespace

// EOF
