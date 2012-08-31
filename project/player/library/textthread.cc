// textthread.cc
// 8/27/2012

#include "textthread.h"

namespace { // anonymous
  struct init_ { init_() {
    qRegisterMetaType<TextThread>("TextThread");
    qRegisterMetaType<TextThreadList>("TextThreadList");
  } };
  init_ static_init_;
} // anonymois namespace

// EOF
