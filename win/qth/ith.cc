// ith.cc  10/15/2011

#include "ith.h"
#include "ith/main.h"
#include "ith/cmdq.h"
#include "ith/hookman.h"
#include <QtCore>
#include <string>  // std::wstring
#include <cstdlib> // ::atexit

//#define DEBUG "Ith"
#include "module/debug/debug.h"

#define ITH_RUNNING_MUTEX       L"ITH_RUNNING"
#define ITH_RUNNING_EVENT       L"ITH_PIPE_EXIST"


// - Ith background service -

/*
// jichi:10/20/2011:Not needed after eliminate Rtl global heap.
// in static initializers from other modules invoked before this module.
namespace { // anonymous

  struct IthBackgroundService
  {
    IthBackgroundService()
    {
      DOUT("IthBackgroundService: initializing system service (fn:IthInitSystemService) ...");
      ::IthInitSystemService(); // So that Ith- work normally.
    }

    ~IthBackgroundService()
    {
      DOUT("~IthBackgroundService: closing system service (fn:IthCloseSystemService) ..." );
      ::IthCloseSystemService();
    }
  };
  IthBackgroundService IthDaemon_;

} // anonymous
*/

// - Global variables -

// + Global settings +
DWORD cyclic_remove = 0, global_filter = 0; // boolean
DWORD split_time = 200, process_time = 50; // in msecs
DWORD repeat_count = 0;

// + Global states +

bool running = true;
BYTE* static_large_buffer; // used in utility.h

HANDLE hPipeExist;

// + Global windows +

HWND hMainWnd;

// + Global singletons +

TextBuffer *texts;
HookManager *man;
//ProfileManager  *pfman;
CommandQueue *cmdq;
BitMap *pid_map;
CustomFilterMultiByte *mb_filter;
CustomFilterUnicode *uni_filter;

// - Static members -

namespace { // anonymous, handlers

  LONG WINAPI
  ExceptionFilter(EXCEPTION_POINTERS *e)
  {
    DOUT("ExceptionFiler:enter");
    //WCHAR str[0x40],name[0x100];

    //::swprintf(str, L"Exception code: 0x%.8X\r\nAddress: 0x%.8X",
    //  ExceptionInfo->ExceptionRecord->ExceptionCode,
    //  ExceptionInfo->ContextRecord->Eip);
    //::MessageBox(0,str, 0, 0);
    QString msg = QString("Exception code: %1\nAddress: %2")
        .arg(QString::number(e->ExceptionRecord->ExceptionCode, 16))
        .arg(QString::number(e->ContextRecord->Eip, 16));

    /*
    MEMORY_BASIC_INFORMATION info;
    NT_STATUS status = ::NtQueryVirtualMemory(
      NtCurrentProcess(),
      (PVOID)e->ContextRecord->Eip,
      MemoryBasicInformation, &info,sizeof(info),0
    );
    if (NT_SUCCESS(status) )
      msg += QString("\nException offset: %2").arg(
        QString::number(ExceptionInfo->ContextRecord->Eip - (DWORD)info.AllocationBase, 16)
      );
    */

    //QMessageBox::warning(0, "Exception", msg);
    //NtTerminateProcess(NtCurrentProcess(),0);
    DOUT("ExceptionFilter:" << msg);
    DOUT("ExceptionFiler:exit");
    return 0;
  }

}

// - Initializations -

/*

namespace { // anonymous, status variables
  bool serviceStarted_ = false;
} // anonymous namespace

void
Ith::startService()
{
  Q_ASSERT(!serviceStarted_);
  if (!serviceStarted_) {
    ::IthInitSystemService();
    serviceStarted_ = true;

    ::atexit(Ith::stopService);
  }
}

void
Ith::stopService()
{
  if (serviceStarted_) {
    ::IthCloseSystemService();
    serviceStarted_ = false;
  }
}
*/

void
Ith::setWindowHandle(void *hwnd)
{ ::hMainWnd = (HWND)hwnd; }

void
Ith::init()
{
  //Q_UNUSED(runAsAdmin); DOUT("init:enter");

  ::hMainWnd = 0;

  DOUT("init: initializing system service (fn:IthInitSystemService) ...");
  ::IthInitSystemService(); // So that Ith- work normally.

  DWORD dwDummy;
  ::IthCreateMutex(ITH_RUNNING_MUTEX, TRUE ,&dwDummy);
  ::hPipeExist=::IthCreateEvent(ITH_RUNNING_EVENT);
  ::NtSetEvent(::hPipeExist, NULL);

  DOUT("init: installing exception filer ...");
  ::SetUnhandledExceptionFilter(ExceptionFilter);

  //DOUT("init: creating hidden windows ...");
  //createHiddenWindows(hIns);

  ::InitializeCriticalSection(&detach_cs);

  DOUT("init: allocating mb_filter ...");
  ::mb_filter = new CustomFilterMultiByte;

  DOUT("init: allocating uni_filter ...");
  ::uni_filter = new CustomFilterUnicode;

  DOUT("init: allocating pid_map ...");
  ::pid_map = new BitMap(0x100);

  DOUT("init: allocating texts ...");
  ::texts = new TextBuffer;

  DOUT("init: allocating man ...");
  ::man = new HookManager;

  //DOUT("init: allocating pfman ...");
  //::pfman = new ProfileManager;

  DOUT("init: allocating cmdq ...");
  ::cmdq = new CommandQueue;

  DOUT("init: creating new pipe (fn:CreateNewPipe) ...");
  ::CreateNewPipe();

  DOUT("init:exit");
}

void
Ith::destroy()
{
  DOUT("destroy:enter");

  DOUT("destroy: clearing running pipe event ..." );
  ::NtClearEvent(hPipeExist);

  DOUT("destroy: deleting cmdq ..." );
  delete cmdq;

  //DOUT("destroy: deleting pfman ..." );
  //delete pfman;

  DOUT("destroy: deleting man ..." );
  delete man;

  DOUT("destroy: deleting texts ..." );
  delete texts;

  DOUT("destroy: deleting mb_filter ..." );
  delete mb_filter;

  DOUT("destroy: deleting uni_filter ..." );
  delete uni_filter;

  DOUT("destroy: deleting pid_map ..." );
  //delete pid_map;

  DOUT("destroy: deleting static_large_buffer if valid ..." );
  if (static_large_buffer)
    delete static_large_buffer;

  DOUT("destroy: closing system service (fn:IthCloseSystemService) ..." );
  ::IthCloseSystemService();

  DOUT("destroy:exit");
}

// - Injections -

bool
Ith::attachProcess(ulong pid)
{ return ::InjectByPID(pid) != -1; }

bool
Ith::detachProcess(ulong pid)
{ return ::ActiveDetachProcess(pid) == 0; }

// - Helpers -

bool
Ith::isElevated()
{
  DOUT("isElevated:enter");
  bool ret;
  HANDLE hToken;
  DWORD dwRet;
  TOKEN_PRIVILEGES priv = { 1, {0x14, 0} }; {
    priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
  }

  ::NtOpenProcessToken(NtCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken);
  ret = !::NtAdjustPrivilegesToken(hToken, FALSE, &priv, sizeof(priv), NULL, &dwRet);
  ::NtClose(hToken);

  DOUT("isElevated:exit: ret =" << ret);
  return ret;
}

ulong
Ith::getProcessIdByName(const QString &name)
{
  if (name.isEmpty())
    return 0;

  std::wstring wstr = name.toStdWString();
  LPWSTR lp = const_cast<LPWSTR>(wstr.c_str());
  Q_ASSERT(lp);
  return ::PIDByName(lp);
}

// EOF
