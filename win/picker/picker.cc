// picker.cc
// 10/30/2011

#include "picker.h"
#include "qtwin/qtwin.h"
#include <qt_windows.h>
#include <QtCore>

#define DEBUG "Picker"
//#define USE_MODE_DEBUG
#include "module/debug/debug.h"

#ifndef PICKER_DLL_NAME
  #error "Please specify target DLL name."
#endif
#define PICKER_MODULE_NAME        PICKER_DLL_NAME

#define PICKER  WindowPicker::globalInstance()

// - Hook implementation -
namespace { // anonymous, picker callbacks

  LRESULT CALLBACK
  LowLevelMouseProc(__in int nCode, __in WPARAM wparam, __in LPARAM lparam)
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
    HWND hwnd = lpMouseEvent->hwnd;
    if (!hwnd)
      hwnd = ::WindowFromPoint(lpMouseEvent->pt);
    if (hwnd)
      PICKER->pickWindow(hwnd);

    return NEXT;
    #undef NEXT
  }

} // anonymouss namespace

// - Constructions -

class WindowPickerImpl
{
public:
  HHOOK hook;
  bool singleShot;

public:
  WindowPickerImpl() : hook(0), singleShot(false) { }
};

WindowPicker*
WindowPicker::globalInstance()
{ static Self g; return &g; }

WindowPicker::WindowPicker(QObject *parent)
  : Base(parent) { impl_ = new Impl; }

WindowPicker::~WindowPicker()
{
  if (isActive())
    stop();

  Q_ASSERT(impl_);
  delete impl_;
}

// - Properties -

void*
WindowPicker::hook() const
{ return impl_->hook; }

bool
WindowPicker::isSingleShot() const
{ return impl_->singleShot; }

void
WindowPicker::setSingleShot(bool t)
{ impl_->singleShot = t; }

// - Slots -

void
WindowPicker::pickWindow(WId hwnd)
{
  DOUT("WindowPicker:pickWindow:enter: WId =" << hwnd);
  if (isSingleShot())
    stop();

  emit windowPicked(hwnd);
  DOUT("WindowPicker:pickWindow:exit");
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

  if (impl_->hook)
    return;

  if (!hInstance) {
    DOUT("WindowPicker:start: DLL name:" << PICKER_MODULE_NAME);
    hInstance = ::GetModuleHandle(PICKER_MODULE_NAME);

    Q_ASSERT(hInstance);
    if (!hInstance) // This mostly happens when dll is not available.
      return;
  }

  impl_->hook = ::SetWindowsHookEx(WH_MOUSE_LL, ::LowLevelMouseProc, hInstance, 0);
  DOUT("WindowPicker:start: started");
}

void
WindowPicker::stop()
{
  if (impl_->hook) {
    ::UnhookWindowsHookEx((HHOOK)impl_->hook);
    impl_->hook = 0;
    DOUT("WindowPicker:stop: stoppped");
  }
}

// EOF

//#include <afxdllx.h>

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

 /*
#pragma data_seg("mydata")
     HWND glhPrevTarWnd=NULL; //上次鼠标所指的窗口句柄
     HWND glhDisplayWnd=NULL; //显示目标窗口标题编辑框的句柄
         HPICKER glhPicker=NULL; //安装的鼠标勾子句柄
         HINSTANCE glhInstance=NULL; //DLL实例句柄
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
    glhInstance=hInstance; //插入保存DLL实例句柄
  }
  else if (dwReason == DLL_PROCESS_DETACH)
  {
    TRACE0("MOUSEPICKER.DLL Terminating!\n");
    // Terminate the library before destructors are called
    AfxTermExtensionModule(MousepickerDLL);
  }
  return 1;   // ok
}

//钩子函数的实现：
LRESULT WINAPI MouseProc(int nCode,WPARAM wparam,LPARAM lparam)
{
  LPMOUSEPICKERSTRUCT pMousePicker=(MOUSEPICKERSTRUCT FAR *)lparam;
  if (nCode>=0)
  {
    //============================================================================
    // 如下代码是得到父窗口标题文本的做法
    // HWND glhTargetWnd=pMousePicker->hwnd;     //取目标窗口句柄
    // HWND ParentWnd=glhTargetWnd;
    // while (ParentWnd !=NULL)
    // {
    //   glhTargetWnd=ParentWnd;
    //   ParentWnd=GetParent(glhTargetWnd);   //取应用程序主窗口句柄
    // }
    // ===========================================================================
    HWND glhTargetWnd=XYZWindowFromPoint(NULL,pMousePicker->pt);    //用上面的一段注释掉的代码替换此行是得到父窗口标题文本的做法
    if(glhTargetWnd!=glhPrevTarWnd)
    {
      char szCaption[256];
      GetWindowText(glhTargetWnd,szCaption,100);
      //取目标窗口标题
      if(IsWindow(glhDisplayWnd))
        SendMessage(glhDisplayWnd,WM_SETTEXT,0,(LPARAM)(LPCTSTR)szCaption);
      glhPrevTarWnd=glhTargetWnd;
      //保存目标窗口
    }
  }
  return CallNextPickerEx(glhPicker,nCode,wparam,lparam);
  //继续传递消息
}
*/

/*
//安装钩子并设定接收显示窗口句柄
BOOL CMousePicker::StartPicker(HWND hWnd)
{
  BOOL bResult=FALSE;

  glhPicker=SetWindowsPickerEx(WH_MOUSE,MouseProc,glhInstance,0);
  // =================================================================================
  //   HPICKER SetWindowsPickerEx( int idPicker,PICKERPROC lpfn, INSTANCE hMod,DWORD dwThreadId )
　　// 参数idPicker表示钩子类型，它是和钩子函数类型一一对应的。
  //   比如，WH_KEYBOARD表示安装的是键盘钩子，WH_MOUSE表示是鼠标钩子等等。
//
　　// Lpfn是钩子函数的地址。
//
　　// HMod是钩子函数所在的实例的句柄。对于线程钩子，该参数为NULL；
  //   对于系统钩子，该参数为钩子函数所在的DLL句柄。
//
　　// dwThreadId 指定钩子所监视的线程的线程号。对于全局钩子，该参数为NULL。
//
　　// SetWindowsPickerEx返回所安装的钩子句柄。
  //   值得注意的是线程钩子和系统钩子的钩子函数的位置有很大的差别。
  //       线程钩子一般在当前线程或者当前线程派生的线程内，
  //       而系统钩子必须放在独立的动态链接库中，实现起来要麻烦一些。
  //       ==================================================================================
        if(glhPicker!=NULL)
                bResult=TRUE;
        glhDisplayWnd=hWnd;
        //设置显示目标窗口标题编辑框的句柄
        return bResult;
}

//卸载钩子
BOOL CMousePicker::StopPicker()
{
  BOOL bResult=FALSE;
  if(glhPicker)
  {
    bResult= UnpickerWindowsPickerEx(glhPicker);
    if(bResult)
    {
      glhPrevTarWnd=NULL;
      glhDisplayWnd=NULL;//清变量
      glhPicker=NULL;
    }
  }
  return bResult;
}
*/

 /*
//返回光标(point)所在点的子窗口句柄
HWND XYZWindowFromPoint(HWND hwndParent,   // handle to parent window
                        POINT point,       // structure with point coordinates
                        UINT uFlags        // skip options
)
{
  if(hwndParent != NULL)
    return ::ChildWindowFromPointEx(hwndParent, point, uFlags);
  //返回光标(point)所在点的子窗口句柄

  // Find the smallest "window" still containing the point
  // Doing this prevents us from stopping at the first window containing the point
  RECT rect, rectSearch;
  HWND pWnd, hWnd, hSearchWnd;

  hWnd = ::WindowFromPoint(point);//得到光标(point)所在点的窗口句柄
  if(hWnd != NULL)
  {
    // Get the size and parent for compare later
    ::GetWindowRect(hWnd, &rect);  //得到整个窗口在屏幕上的矩形框位置
    pWnd = ::GetParent(hWnd);       //得到父窗口句柄

    // We only search further if the window has a parent
    if(pWnd != NULL)
    {
      // Search from the window down in the Z-Order
      hSearchWnd = hWnd;
      do{
        hSearchWnd = ::GetWindow(hSearchWnd, GW_HWNDNEXT);//如果再也找不到这样的窗口，该函数就会返回NULL
        //GetWindow得到和句柄为hSearchWnd(即首次循环为hWnd)的窗口相关的窗口，其关系由GW_HWNDNEXT决定，这里是寻找兄弟窗口

        // Does the search window also contain the point, have the same parent, and is visible?
        ::GetWindowRect(hSearchWnd, &rectSearch);
        if(::PtInRect(&rectSearch, point) && ::GetParent(hSearchWnd) == pWnd && ::IsWindowVisible(hSearchWnd))
        {
          // It does, but is it smaller?比较看谁的面积最小
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
