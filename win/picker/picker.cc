// picker.cc
// 10/30/2011

#ifdef _MSC_VER
# pragma warning (disable:4819)       // C4819: The file contains a character that cannot be represented in the current code page.
#endif // _MSC_VER

#include "picker.h"
#include "picker_p.h"
#include "qtwin/qtwin.h"
#include <QtCore>
#include <qt_windows.h>

#define DEBUG "picker"
#include "module/debug/debug.h"

#ifndef PICKER_DLL_NAME
# error "Please specify target DLL name."
#endif
#define PICKER_MODULE_NAME        PICKER_DLL_NAME

#define PICKER  WindowPicker::globalInstance()

// - Hook implementation -

namespace { namespace detail {

  LRESULT CALLBACK
  MouseProc(__in int nCode, __in WPARAM wparam, __in LPARAM lparam)
  {
    #define NEXT ::CallNextHookEx(hHook, nCode, wparam, lparam)
    HHOOK hHook = (HHOOK)PICKER->hook();
    Q_ASSERT(hHook);
    if (!hHook)
      return 0;

    // nCode can only be HC_ACTION(0) or HC_NOREMOVE (3)
    // See: http://msdn.microsoft.com/en-us/library/ms644988(v=vs.85).aspx
    if (nCode < 0 || !PICKER->isActive())
      return NEXT;

    switch (wparam) {
    case WM_LBUTTONUP: break;
    default: return NEXT;
    }

    LPMOUSEHOOKSTRUCT lpMouseEvent = (LPMOUSEHOOKSTRUCT)lparam;
    Q_ASSERT(lpMouseEvent);
    HWND hwnd = lpMouseEvent->hwnd;
    if (!hwnd)
      hwnd = ::WindowFromPoint(lpMouseEvent->pt);
    if (hwnd)
      PICKER->pickWindow(hwnd);

    return NEXT;
    #undef NEXT
  }

} } // anonymouss detail

// - Constructions -

WindowPicker::WindowPicker(QObject *parent)
  : Base(parent)
{ d_ = new Private; }

WindowPicker::~WindowPicker()
{
  if (isActive())
    stop();
  delete d_;
}

// - Properties -

void*
WindowPicker::hook() const
{ return d_->hook; }

bool
WindowPicker::isSingleShot() const
{ return d_->singleShot; }

void
WindowPicker::setSingleShot(bool t)
{ d_->singleShot = t; }

// - Slots -

void
WindowPicker::pickWindow(WId hwnd)
{
  DOUT("enter: WId =" << hwnd);
  if (isSingleShot())
    stop();

  emit windowPicked(hwnd);
  DOUT("exit");
}

void
WindowPicker::singleShot()
{
  if (!isSingleShot())
    setSingleShot(true);

  if (!isActive())
    start();
}

bool
WindowPicker::isActive() const
{ return hook(); }

void
WindowPicker::start()
{
  static HINSTANCE hInstance = 0;

  if (d_->hook)
    return;

  if (!hInstance) {
    DOUT("start: DLL name:" << PICKER_MODULE_NAME);
    hInstance = ::GetModuleHandle(PICKER_MODULE_NAME);

    Q_ASSERT(hInstance);
    if (!hInstance) // This mostly happens when dll is not available.
      return;
  }

  // Global mode
  d_->hook = ::SetWindowsHookEx(WH_MOUSE_LL, detail::MouseProc, hInstance, 0);
  DOUT("start: started");
}

void
WindowPicker::stop()
{
  if (d_->hook) {
    ::UnhookWindowsHookEx((HHOOK)d_->hook);
    d_->hook = 0;
    DOUT("stop: stoppped");
  }
}

// EOF

 /*
#pragma data_seg("mydata")
     HWND glhPrevTarWnd=NULL;
     HWND glhDisplayWnd=NULL;
         HPICKER glhPicker=NULL;
         HINSTANCE glhInstance=NULL;
#pragma data_seg()

HWND XYZWindowFromPoint(HWND hwndParent,      // handle to parent window
                        POINT point,          // structure with point coordinates
                        UINT uFlags  = CWP_SKIPINVISIBLE       // skip options
                                           );
static AFX_EXTENSION_MODULE MousepickerDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
  // Remove this if you use lpReserved
  UNREFERENCED_PARAMETER(lpReserved);

  if (dwReason == DLL_PROCESS_ATTACH)
  {
    TRACE0("MOUSEPICKER.DLL Initializing!\n");

    // Extension DLL one-time initialization
    if (!AfxInitExtensionModule(MousepickerDLL, hInstance))
      return 0;

    // Insert this DLL into the resource chain
    // NOTE: If this Extension DLL is being implicitly linked to by
    //  an MFC Regular DLL (such as an ActiveX Control)
    //  instead of an MFC application, then you will want to
    //  remove this line from DllMain and put it in a separate
    //  function exported from this Extension DLL.  The Regular DLL
    //  that uses this Extension DLL should then explicitly call that
    //  function to initialize this Extension DLL.  Otherwise,
    //  the CDynLinkLibrary object will not be attached to the
    //  Regular DLL's resource chain, and serious problems will
    //  result.

    new CDynLinkLibrary(MousepickerDLL);
    glhInstance=hInstance;
  }
  else if (dwReason == DLL_PROCESS_DETACH)
  {
    TRACE0("MOUSEPICKER.DLL Terminating!\n");
    // Terminate the library before destructors are called
    AfxTermExtensionModule(MousepickerDLL);
  }
  return 1;   // ok
}

LRESULT WINAPI MouseProc(int nCode,WPARAM wparam,LPARAM lparam)
{
  LPMOUSEPICKERSTRUCT pMousePicker=(MOUSEPICKERSTRUCT FAR *)lparam;
  if (nCode>=0)
  {
    //============================================================================
    // HWND glhTargetWnd=pMousePicker->hwnd;
    // HWND ParentWnd=glhTargetWnd;
    // while (ParentWnd !=NULL)
    // {
    //   glhTargetWnd=ParentWnd;
    //   ParentWnd=GetParent(glhTargetWnd);
    // }
    // ===========================================================================
    HWND glhTargetWnd=XYZWindowFromPoint(NULL,pMousePicker->pt);
    if(glhTargetWnd!=glhPrevTarWnd)
    {
      char szCaption[256];
      GetWindowText(glhTargetWnd,szCaption,100);
      if(IsWindow(glhDisplayWnd))
        SendMessage(glhDisplayWnd,WM_SETTEXT,0,(LPARAM)(LPCTSTR)szCaption);
      glhPrevTarWnd=glhTargetWnd;
    }
  }
  return CallNextPickerEx(glhPicker,nCode,wparam,lparam);
}

BOOL CMousePicker::StartPicker(HWND hWnd)
{
  BOOL bResult=FALSE;

  glhPicker=SetWindowsPickerEx(WH_MOUSE,MouseProc,glhInstance,0);
        if(glhPicker!=NULL)
                bResult=TRUE;
        glhDisplayWnd=hWnd;
        return bResult;
}

BOOL CMousePicker::StopPicker()
{
  BOOL bResult=FALSE;
  if(glhPicker)
  {
    bResult= UnpickerWindowsPickerEx(glhPicker);
    if(bResult)
    {
      glhPrevTarWnd=NULL;
      glhDisplayWnd=NULL;
      glhPicker=NULL;
    }
  }
  return bResult;
}
HWND XYZWindowFromPoint(HWND hwndParent,   // handle to parent window
                        POINT point,       // structure with point coordinates
                        UINT uFlags        // skip options
)
{
  if(hwndParent != NULL)
    return ::ChildWindowFromPointEx(hwndParent, point, uFlags);

  // Find the smallest "window" still containing the point
  // Doing this prevents us from stopping at the first window containing the point
  RECT rect, rectSearch;
  HWND pWnd, hWnd, hSearchWnd;

  hWnd = ::WindowFromPoint(point);
  if(hWnd != NULL)
  {
    // Get the size and parent for compare later
    ::GetWindowRect(hWnd, &rect);
    pWnd = ::GetParent(hWnd);

    // We only search further if the window has a parent
    if(pWnd != NULL)
    {
      // Search from the window down in the Z-Order
      hSearchWnd = hWnd;
      do{
        hSearchWnd = ::GetWindow(hSearchWnd, GW_HWNDNEXT);

        // Does the search window also contain the point, have the same parent, and is visible?
        ::GetWindowRect(hSearchWnd, &rectSearch);
        if(::PtInRect(&rectSearch, point) && ::GetParent(hSearchWnd) == pWnd && ::IsWindowVisible(hSearchWnd))
        {
          // It does, but is it smaller?
          if(((rectSearch.right - rectSearch.left) * (rectSearch.bottom - rectSearch.top)) < ((rect.right - rect.left) * (rect.bottom - rect.top)))
          {
            // Found new smaller window, update compare window
            hWnd = hSearchWnd;
            ::GetWindowRect(hWnd, &rect);
          }
        }
      }
      while(hSearchWnd != NULL);
    }
  }
  return hWnd;
}
*/
