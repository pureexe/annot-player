#pragma once

// dwmapi.h
// 7/10/2011
//
// See: http://labs.qt.nokia.com/2009/09/15/using-blur-behind-on-windows/
// Source:
// http://sic.no-ammo.org/git/index.cgi/Client.git/blob_plain/HEAD:/modules/qtwin/qtwin.h
// http://sic.no-ammo.org/git/index.cgi/Client.git/blob_plain/HEAD:/modules/qtwin/qtwin.cpp

#include <QtGui>
#include <qt_windows.h>

// - DWM API -
// Blur behind data structures, must be consistent with Windows 7 SDK
// http://msdn.microsoft.com/en-us/library/aa969533(v=vs.85).aspx
#ifdef WITH_DWM_STATIC
# include <dwmapi.h>
# define DWMAPI
#else
# define DWMAPI dwmapi_

#define DWM_BB_ENABLE                 0x00000001  // fEnable has been specified
#define DWM_BB_BLURREGION             0x00000002  // hRgnBlur has been specified
#define DWM_BB_TRANSITIONONMAXIMIZED  0x00000004  // fTransitionOnMaximized has been specified
#define WM_DWMCOMPOSITIONCHANGED      0x031E      // Composition changed window message

typedef struct _DWM_BLURBEHIND {
  DWORD dwFlags;
  BOOL fEnable;
  HRGN hRgnBlur;
  BOOL fTransitionOnMaximized;
} DWM_BLURBEHIND, *PDWM_BLURBEHIND;

typedef struct _MARGINS {
  int cxLeftWidth;
  int cxRightWidth;
  int cyTopHeight;
  int cyBottomHeight;
} MARGINS, *PMARGINS;

class DwmApi
{
  QLibrary dwmLib_;
public:
  // http://msdn.microsoft.com/en-us/library/aa969512(v=vs.85).aspx
  typedef HRESULT (WINAPI *PtrDwmIsCompositionEnabled)(BOOL *pfEnabled);
  typedef HRESULT (WINAPI *PtrDwmExtendFrameIntoClientArea)(HWND hWnd, const MARGINS *pMarInset);
  typedef HRESULT (WINAPI *PtrDwmEnableBlurBehindWindow)(HWND hWnd, const DWM_BLURBEHIND *pBlurBehind);
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

namespace dwmapi_ {
  inline DwmApi *api() { static DwmApi g; return &g; }

  inline HRESULT WINAPI
  DwmIsCompositionEnabled(__out BOOL* pfEnabled)
  {
    Q_ASSERT(api()->pDwmIsCompositionEnabled);
    if (!api()->pDwmIsCompositionEnabled)
      return S_FALSE;
    else
      return api()->pDwmIsCompositionEnabled(pfEnabled);
  }

  inline HRESULT WINAPI
  DwmExtendFrameIntoClientArea(HWND hWnd, __in const MARGINS *pMarInset)
  {
    Q_ASSERT(api()->pDwmExtendFrameIntoClientArea);
    if (!api()->pDwmExtendFrameIntoClientArea)
      return S_FALSE;
    else
      return api()->pDwmExtendFrameIntoClientArea(hWnd, pMarInset);
  }

  inline HRESULT WINAPI
  DwmEnableBlurBehindWindow(HWND hWnd, __in const DWM_BLURBEHIND *pBlurBehind)
  {
    Q_ASSERT(api()->pDwmEnableBlurBehindWindow);
    if (!api()->pDwmEnableBlurBehindWindow)
      return S_FALSE;
    else
      return api()->pDwmEnableBlurBehindWindow(hWnd, pBlurBehind);
  }

  inline HRESULT WINAPI
  DwmGetColorizationColor(__out DWORD *pcrColorization, __out BOOL *pfOpaqueBlend)
  {
    Q_ASSERT(api()->pDwmGetColorizationColor);
    if (!api()->pDwmGetColorizationColor)
      return S_FALSE;
    else
      return api()->pDwmGetColorizationColor(pcrColorization, pfOpaqueBlend);
  }

} // anonymous namespace dwmapi_

#endif // WITH_DWM_STATIC

// EOF
