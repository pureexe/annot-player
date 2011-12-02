#pragma once

// ith.h  10/15/2011

#include <QtGlobal>
#include <QString>

namespace Ith {

  // - Initializations -
  //void startService(); ///< Must be invoked at the beginning of the app
  //void stopService(); ///< Automatically stopped on exit

  void init();
  void destroy();
  void setWindowHandle(void *hwnd);

  // - Injections -
  bool attachProcess(ulong pid);
  bool detachProcess(ulong pid);

  // - Helpers -
  bool isElevated();
  ulong getProcessIdByName(const QString &name);

} // namespace Ith

// EOF
