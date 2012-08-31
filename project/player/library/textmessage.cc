// textmessage.cc
// 8/27/2012

#include "textmessage.h"
#include <QtCore/QMetaType>

namespace { // anonymous
  struct init_ { init_() {
    qRegisterMetaType<TextMessage>("TextMessage");
  } };
  init_ static_init_;
} // anonymois namespace

// EOF
