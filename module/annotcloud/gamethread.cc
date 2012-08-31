// gamethread.cc
// 8/29/2012

#include "module/annotcloud/gamethread.h"

using namespace AnnotCloud;

// - Meta type -

namespace { // anonymous
  struct init_ { init_() {
    qRegisterMetaType<GameThread>("GameThread");
    qRegisterMetaType<GameThreadList>("GameThreadList");
  } };
  init_ static_init_;
} // anonymous namespace

// EOF
