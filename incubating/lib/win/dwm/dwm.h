#pragma once

// dwm.h
// 7/10/2011 jichi
// Enable native aero support for windows.

#include <QtGui/qwindowdefs.h>  // for WId
#include <QtGui/QColor>         // for QRgb
QT_FORWARD_DECLARE_CLASS(QWidget)
QT_FORWARD_DECLARE_CLASS(QWindow)

///  Access desktop window manager through dwmapi.dll. Resolve this at runtime to fit old windows XP.
namespace Dwm {

  bool isCompositionEnabled();

  ///  Window notifier is not used.
  bool enableBlurBehindWindow(WId hwnd, bool enable = true);
  ///  Window notifier is used. The window must be persistant.
  bool enableBlurBehindWindow(QWindow *window, bool enable = true);
#ifdef QT_WIDGETS_LIB
  ///  Window notifier is used. The widget must be persistant.
  bool enableBlurBehindWindow(QWidget *widget, bool enable = true);
#endif // QT_WIDGETS_LIB

  ///  Window notifier is not used.
  bool extendFrameIntoClientArea(WId hwnd, int left = -1, int top = -1, int right = -1, int bottom = -1);
  ///  Window notifier is used. The window must be persistant.
  bool extendFrameIntoClientArea(QWindow *window, int left = -1, int top = -1, int right = -1, int bottom = -1);

#ifdef QT_WIDGETS_LIB
  ///  Window notifier is used. The widget must be persistant.
  bool extendFrameIntoClientArea(QWidget *widget, int left = -1, int top = -1, int right = -1, int bottom = -1);
#endif // QT_WIDGETS_LIB

  QRgb colorizationColor();

  // Must be consistent with DWM_COLORIZATIONPARAMS
  struct ColorizationParameters
  {
    QRgb color; // same as colorizationColor
    QRgb afterglow;
    uint colorBalance;
    uint afterglowBalance;
    uint blurBalance;
    uint glassReflectionIntensity;
    uint opaqueBlend; // BOOL
  };
#ifndef WITH_DWM_STATIC
  bool getColorizationParameters(ColorizationParameters *pparams);
  bool setColorizationParameters(const ColorizationParameters &params);
#endif // !WITH_DWM_STATIC
} // namespace Dwm

// EOF
