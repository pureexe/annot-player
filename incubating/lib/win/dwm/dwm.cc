// dwm.cc
// 7/10/2011 jichi

#include "dwm/dwm.h"
#include "dwm/dwmapi_p.h"
#ifdef WITH_DWM_NOTIFIER
# include "dwm/dwmnotifier_p.h"
#endif // WITH_DWM_NOTIFIER
#include <QtGui/QWindow>
#include <QtWidgets/QWidget>

// - Helper -

namespace { namespace detail_ {

  inline WId HWND2WId(HWND hwnd)
  { return reinterpret_cast<WId>(hwnd); }

  inline HWND WId2HWND(WId winId)
  { return reinterpret_cast<HWND>(winId); }

} } // anonymous namespace detail

// - API -

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
Dwm::enableBlurBehindWindow(WId winId, bool enable)
{
  DWM_BLURBEHIND bb = {0};
  bb.fEnable = enable;
  bb.dwFlags = DWM_BB_ENABLE;
  bb.hRgnBlur = NULL;

  HRESULT hr = DWMAPI::DwmEnableBlurBehindWindow(detail_::WId2HWND(winId), &bb);
  return SUCCEEDED(hr);
}

bool
Dwm::enableBlurBehindWindow(QWindow *w, bool enable)
{
  Q_ASSERT(w);
  if (!w)
    return false;

  bool ok = enableBlurBehindWindow(w->winId(), enable);
#ifdef WITH_DWM_NOTIFIER
  if (!enable)
    DWM_NOTIFIER->removeWindow(w);
  else if (ok)
    DWM_NOTIFIER->addWindow(w);
#endif // WITH_DWM_NOTIFIER
  return ok;
}

#ifdef QT_WIDGETS_LIB
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

#endif // QT_WIDGETS_LIB

// http://msdn.microsoft.com/en-us/library/aa969537(v=vs.85).aspx bool
bool
Dwm::extendFrameIntoClientArea(WId winId, int left, int top, int right, int bottom)
{
  MARGINS m = { left, top, right, bottom };
  HRESULT hr = DWMAPI::DwmExtendFrameIntoClientArea(detail_::WId2HWND(winId), &m);
  return SUCCEEDED(hr);
}

bool
Dwm::extendFrameIntoClientArea(QWindow *w, int left, int top, int right, int bottom)
{
  Q_ASSERT(w);
  if (!w)
    return false;

  bool ok = extendFrameIntoClientArea(w->winId(), left, top, right, bottom);

#ifdef WITH_DWM_NOTIFIER
  // If the margin is 0, disable the frame.
  bool enable = left || top || right || bottom;
  if (!enable)
    DWM_NOTIFIER->removeWindow(w);
  else if (ok)
    DWM_NOTIFIER->addWindow(w);
#endif // WITH_DWM_NOTIFIER
  return ok;
}

#ifdef QT_WIDGETS_LIB
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

#endif // QT_WIDGETS_LIB

QRgb
Dwm::colorizationColor()
{
  DWORD color = 0;
  BOOL opaque = FALSE; // unused

  HRESULT hr = DWMAPI::DwmGetColorizationColor(&color, &opaque);
  return SUCCEEDED(hr) ? color : 0;
         //QApplication::palette().window().color();
}

#ifndef WITH_DWM_STATIC

bool
Dwm::getColorizationParameters(ColorizationParameters *pparams)
{
  Q_ASSERT(pparams);
  if (!pparams)
    return false;
  HRESULT hr = DWMAPI::DwmGetColorizationParameters(
        reinterpret_cast<PDWM_COLORIZATIONPARAMS>(pparams));
  return SUCCEEDED(hr);
}
#include <QDebug>
bool
Dwm::setColorizationParameters(const ColorizationParameters &params)
{
  HRESULT hr = DWMAPI::DwmSetColorizationParameters(
        reinterpret_cast<const DWM_COLORIZATIONPARAMS *>(&params), 0);
  return SUCCEEDED(hr);
}

#endif // !WITH_DWM_STATIC

// EOF
