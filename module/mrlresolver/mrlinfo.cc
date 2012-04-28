// mrlinfo.cc
// 2/3/2012

#include "module/mrlresolver/mrlinfo.h"

namespace { // anonymous
  struct init_ { init_() {
    qRegisterMetaType<MrlInfo>("MrlInfo");
    qRegisterMetaType<MediaInfo>("MediaInfo");
  } };
  init_ static_init_;
} // anonymois namespace

// EOF
