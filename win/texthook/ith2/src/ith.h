#pragma once

// ith.h  10/15/2011

#include <QtGui/QWidget>
#include <QtCore/QString>

namespace Ith {

  // - Initializations -
  //void startService(); ///< Must be invoked at the beginning of the app
  //void stopService(); ///< Automatically stopped on exit

  void init();
  void destroy();

  // - Properties -

  WId parentWindow();
  void setParentWindow(WId hwnd);

  int messageInterval();
  void setMessageInterval(int msecs);

  // - Injections -
  bool attachProcess(ulong pid);
  bool detachProcess(ulong pid);

  // - Queries -
  QString getHookNameById(ulong hookId);

} // namespace Ith

// EOF
