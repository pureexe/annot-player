// gamehook.cc
// 9/2/2012

#include "module/annotcloud/gamehook.h"

using namespace AnnotCloud;

// - Meta type -

namespace { // anonymous
  struct init_ { init_() {
    qRegisterMetaType<GameHook>("GameHook");
    qRegisterMetaType<GameHookList>("GameHookList");
  } };
  init_ static_init_;
} // anonymous namespace

// EOF
