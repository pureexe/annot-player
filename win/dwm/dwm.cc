// dwm.cc
// 7/10/2011

#include "dwm/dwm.h"
#include "dwm/dwmapi.h"
#ifdef WITH_DWM_NOTIFIER
#  include "dwm/dwmnotifier.h"
#endif // WITH_DWM_NOTIFIER

void
Dwm::warmUp()
{
  // Cache functions.
  isCompositionEnabled();
}

bool
Dwm::isCompositionEnabled()
{
  static int ret = -1;
  if (ret < 0) {
    HRESULT hr = S_OK;

    BOOL enabled;
    hr = DWMAPI::DwmIsCompositionEnabled(&enabled);

    ret = SUCCEEDED(hr) && enabled ? 1 : 0;
  }
  return ret;
}

// http://msdn.microsoft.com/en-us/library/aa969537(v=vs.85).aspx
bool
Dwm::enableBlurBehindWindow(WId hwnd, bool enable)
{
  DWM_BLURBEHIND bb = {0};
  bb.fEnable = enable;
  bb.dwFlags = DWM_BB_ENABLE;
  bb.hRgnBlur = NULL;

  HRESULT hr = DWMAPI::DwmEnableBlurBehindWindow(hwnd, &bb);
  return SUCCEEDED(hr);
}

bool
Dwm::enableBlurBehindWindow(QWidget *w, bool enable)
{
  Q_ASSERT(w);
  if (!w)
    return false;

  //w->setAttribute(Qt::WA_TranslucentBackground, enable);
  //w->setAttribute(Qt::WA_NoSystemBackground, enable);

  bool ok = enableBlurBehindWindow(w->winId(), enable);
#ifdef WITH_DWM_NOTIFIER
  if (!enable)
    DWM_NOTIFIER->removeWidget(w);
  else if (ok)
    DWM_NOTIFIER->addWidget(w);
#endif // WITH_DWM_NOTIFIER
  return ok;
}

// http://msdn.microsoft.com/en-us/library/aa969537(v=vs.85).aspx bool
bool
Dwm::extendFrameIntoClientArea(WId hwnd, int left, int top, int right, int bottom)
{
  MARGINS m = { left, top, right, bottom };
  HRESULT hr = DWMAPI::DwmExtendFrameIntoClientArea(hwnd, &m);
  return SUCCEEDED(hr);
}

bool
Dwm::extendFrameIntoClientArea(QWidget *w, int left, int top, int right, int bottom)
{
  Q_ASSERT(w);
  if (!w)
    return false;

  bool ok = extendFrameIntoClientArea(w->winId(), left, top, right, bottom);
  //w->setAttribute(Qt::WA_TranslucentBackground, enable);

#ifdef WITH_DWM_NOTIFIER
  // If the margin is 0, disable the frame.
  bool enable = left || top || right || bottom;
  if (!enable)
    DWM_NOTIFIER->removeWidget(w);
  else if (ok)
    DWM_NOTIFIER->addWidget(w);
#endif // WITH_DWM_NOTIFIER
  return ok;
}

QColor
Dwm::colorizationColor()
{
  DWORD color = 0;
  BOOL opaque = FALSE; // unused

  HRESULT hr = DWMAPI::DwmGetColorizationColor(&color, &opaque);
  return SUCCEEDED(hr) ? QColor(color) :
         QApplication::palette().window().color();
}

// EOF
