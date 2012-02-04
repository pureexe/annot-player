// annotcloud/alias.cc
// 10/29/2011

#include "annotcloud/alias.h"
#include <QtCore>

using namespace AnnotCloud;

namespace { // anonymous
  struct init_ { init_() {
    qRegisterMetaType<Alias>("Alias");
    qRegisterMetaType<AliasList>("AliasList");
  } };
  init_ static_init_;
} // anonymois namespace

// EOF
