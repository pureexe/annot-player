// dwm.cc
// 7/10/2011
//
// Source:
// http://sic.no-ammo.org/git/index.cgi/Client.git/blob_plain/HEAD:/modules/qtwin/qtwin.h
// http://sic.no-ammo.org/git/index.cgi/Client.git/blob_plain/HEAD:/modules/qtwin/qtwin.cpp

#include "dwm.h"
#include <QtGui>
#include <qt_windows.h>

// - DWM API -
// Blur behind data structures, must be consistent with Windows 7 SDK
// http://msdn.microsoft.com/en-us/library/aa969533(v=vs.85).aspx
#ifdef USE_STATIC_LIB
  #include <dwmapi.h>
#else
  #define DWM_BB_ENABLE                 0x00000001  // fEnable has been specified
  #define DWM_BB_BLURREGION             0x00000002  // hRgnBlur has been specified
  #define DWM_BB_TRANSITIONONMAXIMIZED  0x00000004  // fTransitionOnMaximized has been specified
  #define WM_DWMCOMPOSITIONCHANGED      0x031E      // Composition changed window message

  typedef struct _DWM_BLURBEHIND
  {
    DWORD dwFlags;
    BOOL fEnable;
    HRGN hRgnBlur;
    BOOL fTransitionOnMaximized;
  } DWM_BLURBEHIND, *PDWM_BLURBEHIND;

  typedef struct _MARGINS
  {
    int cxLeftWidth;
    int cxRightWidth;
    int cyTopHeight;
    int cyBottomHeight;
  } MARGINS, *PMARGINS;

#endif // USE_STATIC_LIB

#ifndef USE_STATIC_LIB
namespace { // anonymous, DwmApi
  class DwmApi
  {
    QLibrary dwmLib_;
  public:
    // http://msdn.microsoft.com/en-us/library/aa969512(v=vs.85).aspx
    typedef HRESULT (WINAPI *PtrDwmIsCompositionEnabled)(BOOL* pfEnabled);
    typedef HRESULT (WINAPI *PtrDwmExtendFrameIntoClientArea)(HWND hWnd, const MARGINS* pMarInset);
    typedef HRESULT (WINAPI *PtrDwmEnableBlurBehindWindow)(HWND hWnd, const DWM_BLURBEHIND* pBlurBehind);
    typedef HRESULT (WINAPI *PtrDwmGetColorizationColor)(DWORD *pcrColorization, BOOL *pfOpaqueBlend);

    PtrDwmIsCompositionEnabled pDwmIsCompositionEnabled;
    PtrDwmExtendFrameIntoClientArea pDwmExtendFrameIntoClientArea;
    PtrDwmEnableBlurBehindWindow pDwmEnableBlurBehindWindow;
    PtrDwmGetColorizationColor pDwmGetColorizationColor;

  public:
    DwmApi() : dwmLib_("dwmapi")
    {
      #define RESOLVE(_lib, _api) p##_api = static_cast<Ptr##_api>(_lib.resolve(#_api))
      RESOLVE(dwmLib_, DwmIsCompositionEnabled);
      RESOLVE(dwmLib_, DwmExtendFrameIntoClientArea);
      RESOLVE(dwmLib_, DwmEnableBlurBehindWindow);
      RESOLVE(dwmLib_, DwmGetColorizationColor);
      #undef RESOLVE
    }

    bool bad() const { return dwmLib_.isLoaded(); }
  };

} // anonyomus
#endif // USE_STATIC_LIB

// - WindowNotifier -

#ifdef USE_WINDOW_NOTIFIER
namespace { // anoymous, WindowsNotifier

  // Inherit QWidget to access protected \fn WinEvent.
  class WindowNotifier : public QWidget
  {
    // Q_OBJECT
    typedef WindowNotifier Self;
    typedef QWidget Base;

    QWidgetList widgets_;

  public:
    WindowNotifier() { winId(); }

    inline void addWidget(QWidget *widget);
    inline void removeWidget(QWidget *widget);

  protected:
    virtual bool winEvent(MSG *message, long *result); ///> override
  };

  void
  WindowNotifier::addWidget(QWidget *widget)
  {
    Q_ASSERT(widget);
    if (widgets_.indexOf(widget) < 0)
      widgets_.append(widget);
  }

  void
  WindowNotifier::removeWidget(QWidget *widget)
  {
    Q_ASSERT(widget);
    widgets_.removeAll(widget);
  }

  bool
  WindowNotifier::winEvent(MSG *message, long *result)
  {
    if (!widgets_.empty()
        && message && message->message == WM_DWMCOMPOSITIONCHANGED) {

      bool enabled = Dwm::isCompositionEnabled();
      foreach (QWidget *widget, widgets_)
        if (widget) {
          widget->setAttribute(Qt::WA_NoSystemBackground, enabled);
          widget->update();
        }
    }

    return Base::winEvent(message, result);
  }

} // anonymous

#endif // USE_WINDOW_NOTIFIER

// - DesktopWindowManager -


#ifdef USE_STATIC_LIB
  #define DWM_API
#else
  namespace { namespace dwmapi {
    inline DwmApi *api() { static DwmApi g; return &g; }

    inline HRESULT WINAPI DwmIsCompositionEnabled(__out BOOL* pfEnabled)
    {
      Q_ASSERT(api()->pDwmIsCompositionEnabled);
      if (!api()->pDwmIsCompositionEnabled)
        return S_FALSE;
      else
        return api()->pDwmIsCompositionEnabled(pfEnabled);
    }

    inline HRESULT WINAPI DwmExtendFrameIntoClientArea(HWND hWnd, __in const MARGINS* pMarInset)
    {
      Q_ASSERT(api()->pDwmExtendFrameIntoClientArea);
      if (!api()->pDwmExtendFrameIntoClientArea)
        return S_FALSE;
      else
        return api()->pDwmExtendFrameIntoClientArea(hWnd, pMarInset);
    }

    inline HRESULT WINAPI DwmEnableBlurBehindWindow(HWND hWnd, __in const DWM_BLURBEHIND* pBlurBehind)
    {
      Q_ASSERT(api()->pDwmEnableBlurBehindWindow);
      if (!api()->pDwmEnableBlurBehindWindow)
        return S_FALSE;
      else
        return api()->pDwmEnableBlurBehindWindow(hWnd, pBlurBehind);
    }

    inline HRESULT WINAPI DwmGetColorizationColor(__out DWORD *pcrColorization, __out BOOL *pfOpaqueBlend)
    {
      Q_ASSERT(api()->pDwmGetColorizationColor);
      if (!api()->pDwmGetColorizationColor)
        return S_FALSE;
      else
        return api()->pDwmGetColorizationColor(pcrColorization, pfOpaqueBlend);
    }

  }}
  #define DWM_API dwmapi
#endif

#ifdef USE_WINDOW_NOTIFIER
  namespace { namespace static_ {
    inline WindowNotifier *windowNotifier() { static WindowNotifier g; return &g; }
  }}

  #define WINDOW_NOTIFIER       static_::windowNotifier()
#endif // USE_WINDOW_NOTIFIER

#ifdef USE_WINDOW_NOTIFIER
#endif // USE_WINDOW_NOTIFIER

bool
Dwm::isCompositionEnabled()
{
  HRESULT hr = S_OK;

  BOOL enabled;
  hr = DWM_API::DwmIsCompositionEnabled(&enabled);

  return SUCCEEDED(hr) && enabled;
}

// http://msdn.microsoft.com/en-us/library/aa969537(v=vs.85).aspx
bool
Dwm::enableBlurBehindWindow(WId hwnd, bool enable)
{
  DWM_BLURBEHIND bb = {0};
  bb.fEnable = enable;
  bb.dwFlags = DWM_BB_ENABLE;
  bb.hRgnBlur = NULL;

  HRESULT hr = DWM_API::DwmEnableBlurBehindWindow(hwnd, &bb);
  return SUCCEEDED(hr);
}

bool
Dwm::enableBlurBehindWindow(QWidget *widget, bool enable)
{
  Q_ASSERT(widget);
  if (!widget)
    return false;

  //widget->setAttribute(Qt::WA_TranslucentBackground, enable);
  //widget->setAttribute(Qt::WA_NoSystemBackground, enable);

  bool succeed = enableBlurBehindWindow(widget->winId(), enable);
  #ifdef USE_WINDOW_NOTIFIER
  if (enable && succeed)
    WINDOW_NOTIFIER->addWidget(widget);
  if (!enable)
    WINDOW_NOTIFIER->removeWidget(widget);
  #endif // USE_WINDOW_NOTIFIER
  return succeed;
}

// http://msdn.microsoft.com/en-us/library/aa969537(v=vs.85).aspx bool
bool
Dwm::extendFrameIntoClientArea(WId hwnd, int left, int top, int right, int bottom)
{
  MARGINS m = {left, top, right, bottom};
  HRESULT hr = DWM_API::DwmExtendFrameIntoClientArea(hwnd, &m);
  return SUCCEEDED(hr);
}

bool
Dwm::extendFrameIntoClientArea(QWidget *widget, int left, int top, int right, int bottom)
{
  Q_ASSERT(widget);
  if (!widget)
    return false;

  bool succeed = extendFrameIntoClientArea(widget->winId(), left, top, right, bottom);
  //widget->setAttribute(Qt::WA_TranslucentBackground, enable);

#ifdef USE_WINDOW_NOTIFIER
  // If the margin is 0, disable the frame.
  bool enable = left || top || right || bottom;
  if (enable && succeed)
    WINDOW_NOTIFIER->addWidget(widget);
  if (!enable)
    WINDOW_NOTIFIER->removeWidget(widget);
#endif // USE_WINDOW_NOTIFIER
  return succeed;
}

QColor
Dwm::colorizatinColor()
{
  QColor ret = QApplication::palette().window().color();
  DWORD color = 0;
  BOOL opaque = FALSE;

  HRESULT hr = DWM_API::DwmGetColorizationColor(&color, &opaque);
  if (!SUCCEEDED(hr))
    return ret;
  else
    return QColor(color);
}

// EOF
