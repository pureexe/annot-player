#pragma once

#include <qt_windows.h>

// qtwinapi.h   10/14/2011

namespace QtWin { namespace Api {

// ntdll
namespace ntdll {

} // namespace ntdll

// <dwmapi.h>
namespace dwmapi {
#ifndef _DWMAPI_H_

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

// http://msdn.microsoft.com/en-us/library/aa969512(v=vs.85).aspx
typedef HRESULT (WINAPI *FpDwmIsCompositionEnabled)(BOOL* pfEnabled);
typedef HRESULT (WINAPI *FpDwmExtendFrameIntoClientArea)(HWND hWnd, const MARGINS* pMarInset);
typedef HRESULT (WINAPI *FpDwmEnableBlurBehindWindow)(HWND hWnd, const DWM_BLURBEHIND* pBlurBehind);
typedef HRESULT (WINAPI *FpDwmGetColorizationColor)(DWORD *pcrColorization, BOOL *pfOpaqueBlend);

#endif // _DWMAPI_H_
} // namespace dwmapi

} } // namespace Api, QtWin

// EOF
