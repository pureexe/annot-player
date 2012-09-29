#pragma once

// dwmapi_p.h
// 7/10/2011 jichi
//
// See: http://labs.qt.nokia.com/2009/09/15/using-blur-behind-on-windows/
// See:
// http://sic.no-ammo.org/git/index.cgi/Client.git/blob_plain/HEAD:/modules/qtwin/qtwin.h
// http://sic.no-ammo.org/git/index.cgi/Client.git/blob_plain/HEAD:/modules/qtwin/qtwin.cpp

#include <QtCore/QLibrary>
#include <qt_windows.h>

// - DWM Internal API -

// See: http://www.prog.org.ru/topic_18109_0.html
// See: OllyDbg disasm
#define DWM_ENTRYPOINT_GETCOLORIZATIONPARAMS    reinterpret_cast<const char *>(127) // #127
#define DWM_ENTRYPOINT_SETCOLORIZATIONPARAMS    reinterpret_cast<const char *>(131) // #131

// See: http://stackoverflow.com/questions/3560890/vista-7-how-to-get-glass-color
// See: http://www.withinwindows.com/2010/07/01/retrieving-aero-glass-base-color-for-opaque-surface-rendering/
// See: http://stackoverflow.com/questions/1487919/how-does-windows-change-aero-glass-color
typedef struct _DWM_COLORIZATIONPARAMS
{
  DWORD ColorizationColor;
  DWORD ColorizationAfterglow;
  DWORD ColorizationColorBalance;
  DWORD ColorizationAfterglowBalance;
  DWORD ColorizationBlurBalance;
  DWORD ColorizationGlassReflectionIntensity;
  DWORD ColorizationOpaqueBlend;
} DWM_COLORIZATIONPARAMS, *PDWM_COLORIZATIONPARAMS;

// - DWM API -
// Blur behind data structures, must be consistent with Windows 7 SDK
// http://msdn.microsoft.com/en-us/library/aa969533(v=vs.85).aspx
#ifdef WITH_DWM_STATIC
# include <dwmapi.h>
# define DWMAPI
#else
# define DWMAPI dwmapi

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

#define DWM_DLL "dwmapi"
class DwmApi
{
  QLibrary dwmLib_;
public:
  // http://msdn.microsoft.com/en-us/library/aa969512(v=vs.85).aspx
  typedef HRESULT (WINAPI *PtrDwmIsCompositionEnabled)(BOOL *pfEnabled);
  typedef HRESULT (WINAPI *PtrDwmExtendFrameIntoClientArea)(HWND hWnd, const MARGINS *pMarInset);
  typedef HRESULT (WINAPI *PtrDwmEnableBlurBehindWindow)(HWND hWnd, const DWM_BLURBEHIND *pBlurBehind);
  typedef HRESULT (WINAPI *PtrDwmGetColorizationColor)(DWORD *pcrColorization, BOOL *pfOpaqueBlend);
  typedef HRESULT (WINAPI *PtrDwmGetColorizationParameters)(DWM_COLORIZATIONPARAMS *pColorizationParams);
  typedef HRESULT (WINAPI *PtrDwmSetColorizationParameters)(const DWM_COLORIZATIONPARAMS *pColorizationParams, DWORD *pUnknown);

  PtrDwmIsCompositionEnabled pDwmIsCompositionEnabled;
  PtrDwmExtendFrameIntoClientArea pDwmExtendFrameIntoClientArea;
  PtrDwmEnableBlurBehindWindow pDwmEnableBlurBehindWindow;
  PtrDwmGetColorizationColor pDwmGetColorizationColor;
  PtrDwmGetColorizationParameters pDwmGetColorizationParameters;
  PtrDwmSetColorizationParameters pDwmSetColorizationParameters;

public:
  DwmApi() : dwmLib_(DWM_DLL)
  {
    #define RESOLVE(_api) p##_api = reinterpret_cast<Ptr##_api>(dwmLib_.resolve(#_api))
    RESOLVE(DwmIsCompositionEnabled);
    RESOLVE(DwmExtendFrameIntoClientArea);
    RESOLVE(DwmEnableBlurBehindWindow);
    RESOLVE(DwmGetColorizationColor);

    RESOLVE(DwmGetColorizationParameters);
    RESOLVE(DwmSetColorizationParameters);
    #undef RESOLVE

    pDwmGetColorizationParameters =
        reinterpret_cast<PtrDwmGetColorizationParameters>(
          dwmLib_.resolve(DWM_ENTRYPOINT_GETCOLORIZATIONPARAMS));
    pDwmSetColorizationParameters =
        reinterpret_cast<PtrDwmSetColorizationParameters>(
          dwmLib_.resolve(DWM_ENTRYPOINT_SETCOLORIZATIONPARAMS));
  }

  bool bad() const { return dwmLib_.isLoaded(); }
};

namespace dwmapi {
  inline DwmApi *api() { static DwmApi g; return &g; }

  inline HRESULT WINAPI
  DwmIsCompositionEnabled(__out BOOL* pfEnabled)
  {
    Q_ASSERT(api()->pDwmIsCompositionEnabled);
    return !api()->pDwmIsCompositionEnabled ? S_FALSE :
            api()->pDwmIsCompositionEnabled(pfEnabled);
  }

  inline HRESULT WINAPI
  DwmExtendFrameIntoClientArea(HWND hWnd, __in const MARGINS *pMarInset)
  {
    Q_ASSERT(api()->pDwmExtendFrameIntoClientArea);
    return !api()->pDwmExtendFrameIntoClientArea ? S_FALSE :
            api()->pDwmExtendFrameIntoClientArea(hWnd, pMarInset);
  }

  inline HRESULT WINAPI
  DwmEnableBlurBehindWindow(HWND hWnd, __in const DWM_BLURBEHIND *pBlurBehind)
  {
    Q_ASSERT(api()->pDwmEnableBlurBehindWindow);
    return !api()->pDwmEnableBlurBehindWindow ? S_FALSE :
            api()->pDwmEnableBlurBehindWindow(hWnd, pBlurBehind);
  }

  inline HRESULT WINAPI
  DwmGetColorizationColor(__out DWORD *pcrColorization, __out BOOL *pfOpaqueBlend)
  {
    Q_ASSERT(api()->pDwmGetColorizationColor);
    return !api()->pDwmGetColorizationColor ? S_FALSE :
            api()->pDwmGetColorizationColor(pcrColorization, pfOpaqueBlend);
  }

  inline HRESULT WINAPI
  DwmGetColorizationParameters(__out DWM_COLORIZATIONPARAMS *pColorizationParams)
  {
    Q_ASSERT(api()->pDwmGetColorizationParameters);
    return !api()->pDwmGetColorizationParameters ? S_FALSE :
            api()->pDwmGetColorizationParameters(pColorizationParams);
  }

  // pUnknown: optional, always set to zero is specified.
  inline HRESULT WINAPI
  DwmSetColorizationParameters(__in const DWM_COLORIZATIONPARAMS *pColorizationParams, __out_opt DWORD *pUnknown)
  {
    Q_ASSERT(api()->pDwmSetColorizationParameters);
    return !api()->pDwmSetColorizationParameters ? S_FALSE :
            api()->pDwmSetColorizationParameters(pColorizationParams, pUnknown);
  }

} // anonymous namespace dwmapi

#endif // WITH_DWM_STATIC

// EOF
