// core/cloud/alias.cc
// 10/29/2011

#include "core/cloud/alias.h"
#include <QtCore>

using namespace Core::Cloud;

namespace { // anonymous
  struct init_ { init_() {
    qRegisterMetaType<Alias>("Alias");
    qRegisterMetaType<AliasList>("AliasList");
  } };
  init_ static_init_;
} // anonymois namespace

// EOF
