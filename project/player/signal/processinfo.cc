// processinfo.cc
// 10/18/2011

#include "processinfo.h"

namespace { // anonymous
  struct init_ { init_() {
    qRegisterMetaType<ProcessInfo>("ProcessInfo");
  } };
  init_ static_init_;
} // anonymois namespace

// EOF
