#pragma once

// dwm.h
// 7/10/2011
// Enable native aero support for windows.

#include "dwm_config.h"
#include <QWidget>

///  Access desktop window manager through dwmapi.dll. Resolve this at runtime to fit old windows XP.
namespace Dwm {

  ///  Optional init procedure responsible for load caches to speed up function calls.
  void warmUp();

  bool isCompositionEnabled();

  ///  Window notifier is not used.
  bool enableBlurBehindWindow(WId hwnd, bool enable = true);
  ///  Window notifier is used.
  bool enableBlurBehindWindow(QWidget *widget, bool enable = true);

  ///  Window notifier is not used.
  bool extendFrameIntoClientArea(WId hwnd, int left = -1, int top = -1, int right = -1, int bottom = -1);
  ///  Window notifier is used.
  bool extendFrameIntoClientArea(QWidget *widget, int left = -1, int top = -1, int right = -1, int bottom = -1);

  QColor colorizatinColor();

} // namespace Dwm

// EOF
