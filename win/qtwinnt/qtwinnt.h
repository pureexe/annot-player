#pragma once

// qtwinnt.h
// 10/14/2011

#include <QString>

namespace QtWinnt {

  // - Process -

  bool isElevated();
  bool isProcessElevated(void *processHandle);

  ulong getProcessIdByName(const QString &name);

} // namespace QtWinnt

// EOF
