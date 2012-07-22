// iht/inject.cc
// 10/14/2011
// Derived from ith2.
// See also: ith3/ihf/main.cpp
#include "ith/inject.h"
#include "ith/main.h"
#include "ith/hookman.h"

//#define DEBUG "ith:inject"
#include "module/debug/debug.h"

#define ITH_MODULE_DLL  L"ITH.dll"        // first step injection
#define ITH_ENGINE_DLL  L"ITH_engine.dll" // second step injection

// defined in sys.cc
extern WCHAR file_path[];
extern LPWSTR current_dir; // a pointer pointed to the end of file_path

#define HOOKMAN HookManager::globalInstance()

// - Inject -

HANDLE
InjectProcess(HANDLE hProc)
{
  DOUT("enter");

  HANDLE hModule = INVALID_HANDLE_VALUE; // injected module handle to return

  // jichi: Assume existent of ITH*.dll in the current dir
  //if (!IthCheckFile(ITH_MODULE_DLL)) return -1;
  //if (!IthCheckFile(ITH_ENGINE_DLL)) return -1;

  LPVOID lpvAllocAddr = nullptr;
  DWORD dwWrite = 0x1000;
  ::NtAllocateVirtualMemory(hProc, &lpvAllocAddr, 0, &dwWrite, MEM_COMMIT, PAGE_READWRITE);
  if (!lpvAllocAddr)
    return INVALID_HANDLE_VALUE;

  // STEP 1: Load ITH_MODULE_DLL
  ::wcscpy(::current_dir, ITH_MODULE_DLL);
  ::CheckThreadStart();
  ::NtWriteVirtualMemory(hProc, lpvAllocAddr, ::file_path + 4, 2 * MAX_PATH, &dwWrite);
  {
    HANDLE hThread = ::IthCreateThread(::LoadLibraryW, (DWORD)lpvAllocAddr, hProc);
    if (!hThread || hThread == INVALID_HANDLE_VALUE) {
      DOUT("ERROR: failed to create remote hook thread");
      dwWrite = 0; // region size = 0
      ::NtFreeVirtualMemory(hProc, &lpvAllocAddr, &dwWrite, MEM_RELEASE);
      return INVALID_HANDLE_VALUE;
    }
    ::NtWaitForSingleObject(hThread, 0, 0);

    THREAD_BASIC_INFORMATION info; // hooked remote thread info
    NTSTATUS status = ::NtQueryInformationThread(hThread, ThreadBasicInformation, &info, sizeof(info), 0); // 0: &dwReturn
    if (NT_SUCCESS(status))
      hModule = (HANDLE)info.ExitStatus;

    ::NtClose(hThread);
  }

  // STEP 2: Load ITH_ENGINE_DLL
  ::wcscpy(::current_dir, ITH_ENGINE_DLL);
  ::NtWriteVirtualMemory(hProc, lpvAllocAddr, ::file_path + 4, 2 * MAX_PATH, &dwWrite);
  {
    HANDLE hThread = ::IthCreateThread(::LoadLibraryW, (DWORD)lpvAllocAddr, hProc);
    if (!hThread || hThread == INVALID_HANDLE_VALUE) {
      DOUT("ERROR: failed to create remote hook thread");
      dwWrite = 0; // region size = 0
      ::NtFreeVirtualMemory(hProc, &lpvAllocAddr, &dwWrite, MEM_RELEASE);
      return INVALID_HANDLE_VALUE;
    }
    ::NtWaitForSingleObject(hThread, 0, 0);
    ::NtClose(hThread);
  }

  dwWrite = 0; // region size = 0
  ::NtFreeVirtualMemory(hProc, &lpvAllocAddr, &dwWrite, MEM_RELEASE);

  DOUT("exit: ret =" << hModule);
  return hModule;
}

HANDLE
InjectProcessByPid(DWORD pid)
{
  if (pid == ::current_process_id) {
    //ConsoleOutput(SelfAttach);
    DOUT("skip attach to my process");
    return INVALID_HANDLE_VALUE;
  }

  if (HOOKMAN->GetModuleByPID(pid)) {
    //ConsoleOutput(AlreadyAttach);
    DOUT("process already attached");
    return INVALID_HANDLE_VALUE;
  }

  DWORD dwReturn; // mutex return code
  WCHAR wszMutex[0x40];
  HookManager::setMutexNameForPid(wszMutex, pid);
  ::NtClose(::IthCreateMutex(wszMutex, 0, &dwReturn));
  if (dwReturn)
    return INVALID_HANDLE_VALUE;

  CLIENT_ID id; {
    id.UniqueProcess = pid;
    id.UniqueThread = 0;
  }
  OBJECT_ATTRIBUTES oa = { }; { oa.uLength=sizeof(oa); }
  HANDLE hProc;
  NTSTATUS status = ::NtOpenProcess(&hProc,
      PROCESS_QUERY_INFORMATION |
      PROCESS_CREATE_THREAD |
      PROCESS_VM_OPERATION |
      PROCESS_VM_READ |
      PROCESS_VM_WRITE,
      &oa, &id);
  if (!NT_SUCCESS(status)) {
    //ConsoleOutput(ErrorOpenProcess);
    DOUT("failed to open process");
    return INVALID_HANDLE_VALUE;
  }

  HANDLE hModule = ::InjectProcess(hProc);
  ::NtClose(hProc);
  return hModule;
}

// - Detach -

BOOL
DetachProcessByPid(DWORD pid)
{
  HANDLE hProc = HOOKMAN->GetProcessByPID(pid);

  DWORD hIthModule = HOOKMAN->GetModuleByPID(pid);
  if (!hIthModule)
    return FALSE;

  DWORD hIthEngine = HOOKMAN->GetEngineByPID(pid);
  hIthEngine &= ~0xFF;

  //SendParam sp = { }; { sp.type = 4; }
  //cmdq->AddRequest(sp, pid);

  // STEP 2: Detach engine dll
  {
    HANDLE hThread = ::IthCreateThread(::LdrUnloadDll, hIthEngine, hProc);
    if (!!hThread == 0 || hThread == INVALID_HANDLE_VALUE)
      return FALSE;
    ::NtWaitForSingleObject(hThread, 0, 0);
    ::NtClose(hThread);
  }

  // STEP 1: Detach module dll
  BOOL ret = FALSE;
  {
    HANDLE hThread = ::IthCreateThread(::LdrUnloadDll, hIthModule, hProc);
    if (!hThread || hThread == INVALID_HANDLE_VALUE)
      return FALSE;

    ::NtWaitForSingleObject(hThread, 0, 0);
    THREAD_BASIC_INFORMATION info;
    NTSTATUS status = ::NtQueryInformationThread(hThread, ThreadBasicInformation, &info, sizeof(info), 0);
    ::NtClose(hThread);
    if (NT_SUCCESS(status))
      ret = info.ExitStatus;
  }

  ::NtSetEvent(::hPipeExist, 0);
  ::FreeThreadStart(hProc);

  return ret;
}

// EOF
