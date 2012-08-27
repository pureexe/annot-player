#pragma once

// ith_p.h
// 10/15/2011

#include <QtCore/QString>

struct HookParam; // opaque, declared in ITH/common.h

namespace Ith {

  bool parseHookCode(const QString &code, HookParam *hp);

} // namespace Ith

// EOF
