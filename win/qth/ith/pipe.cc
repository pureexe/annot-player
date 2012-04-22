// ith/pipe.cc
// 10/14/2011
// Derived from ith2.
// See also: ith3/ihf_dll/pipe.cpp
#include "ith/pipe.h"
#include "ith/main.h"
#include "ith/hookman.h"

//#define DEBUG "ith:pipe"
#include "module/debug/debug.h"

#define NAMED_PIPE_DISCONNECT   1
#define NAMED_PIPE_CONNECT      2

#define ITH_TEXT_PIPE           L"\\??\\pipe\\ITH_PIPE"
#define ITH_COMMAND_PIPE        L"\\??\\pipe\\ITH_COMMAND"

#define HOOKMAN         HookManager::globalInstance()

// - Static variables -


namespace { // anonymous

  CRITICAL_SECTION csDetach_; // defined in ith.cc
  HANDLE hDetachEvent_;

  void createPipe_();

} // anonymous

// - Public API -

void
OpenPipe()
{
  ::InitializeCriticalSection(&::csDetach_);
  createPipe_();
}

// - Implementation -

namespace { // anonymous

  inline PBYTE
  filter_(PBYTE str, int len)
  {
    while (true) {
      WORD s = *reinterpret_cast<PWORD>(str);
      if (len >= 2) {
        if (s == 0x4081 || s == 0x3000 || s <= 0x20) { str += 2; len -= 2; }
        else break;
      }
      else if (str[0]<=0x20) { str++; len--; }
      else break;
    }
    return str;
  }

  inline void
  connectPipe_(HANDLE hPipe, IO_STATUS_BLOCK *ios)
  {
    ULONG ioctl = CTL_CODE(FILE_DEVICE_NAMED_PIPE, NAMED_PIPE_CONNECT, 0, 0);
    ::NtFsControlFile(hPipe, 0, 0, 0, ios, ioctl, 0, 0, 0, 0);
  }

  inline void
  disconnectPipe_(HANDLE hPipe, IO_STATUS_BLOCK *ios)
  {
    HANDLE hEvent = ::IthCreateEvent(0);
    ULONG ioctl = CTL_CODE(FILE_DEVICE_NAMED_PIPE, NAMED_PIPE_DISCONNECT, 0, 0);
    NTSTATUS status = ::NtFsControlFile(hPipe, hEvent, 0, 0, ios, ioctl, 0, 0, 0, 0);
    if (status == STATUS_PENDING)
      ::NtWaitForSingleObject(hEvent,0,0);
    ::NtClose(hEvent);
  }

  inline void
  synchronizedDetachFromProcess_(DWORD pid)
  {
    WCHAR mt[0x20];
    swprintf(mt, L"ITH_DETACH_%d", pid);
    HANDLE hMutex = ::IthOpenMutex(mt);
    if (hMutex!=INVALID_HANDLE_VALUE) {
      ::NtWaitForSingleObject(hMutex,0,0);
      ::NtReleaseMutant(hMutex,0);
      ::NtClose(hMutex);
    }
    ::NtSetEvent(hDetachEvent_,0);
    ::NtSetEvent(::hPipeExist,0);
  }

  DWORD WINAPI
  recvThread_(LPVOID lpThreadParameter)
  {

    HANDLE hTextPipe = reinterpret_cast<HANDLE>(lpThreadParameter);
    IO_STATUS_BLOCK ios;
    connectPipe_(hTextPipe, &ios);

    if (!::running || !::texts)
      return 0;

    BYTE *buf = new BYTE[0x1000];
    ::NtReadFile(hTextPipe, 0, 0, 0, &ios, buf, 16, 0, 0);
    DWORD pid = *(DWORD*)buf;
    {
       DWORD hookman = *(DWORD*)(buf+0x4);
       DWORD module = *(DWORD*)(buf+0x8);
       DWORD engine = *(DWORD*)(buf+0xC);
       HOOKMAN->RegisterProcess(pid,hookman,module,engine);
    }

    createPipe_();

    while (::running) {
      NTSTATUS status = ::NtReadFile(hTextPipe,0,0,0,&ios,buf,0x1000,0,0);
      if (!NT_SUCCESS(status))
        break;

      DWORD recvLen = ios.uInformation;
      if (recvLen < 0xC)
        break;

      DWORD hook = *(DWORD*)buf;

      union{ DWORD retn; DWORD cmd_type; };
      retn = *((DWORD*)buf+1);

      union{ DWORD split; DWORD new_engine_addr; };
      split = *((DWORD*)buf+2);

      buf[recvLen] = 0;
      buf[recvLen + 1] = 0;

      BYTE *it = filter_(buf+0xC,recvLen-0xC);
      recvLen -= it - buf;

      if (recvLen>>31)
        recvLen = 0;

      if (hook + 1 != 0) { // hook !== INVALID_HANDLE_VALUE
        HOOKMAN->DispatchText(pid, it, hook, retn, split, recvLen);
      //else
      //  switch (cmd_type)
      //  {
      //  case 2:
      //    HOOKMAN->GetProcessPath(pid,(LPWSTR)(buf+0xC));
      //    HOOKMAN->SetProcessEngineType(pid,*(DWORD*)(buf+0x8));
      //    //pfman->SetProfileEngine((LPWSTR)(buf+0xC),*(DWORD*)(buf+8));
      //    break;
      //  case 1:
      //    HOOKMAN->GetProcessPath(pid,(LPWSTR)buf);
      //    //pfman->RefreshProfileAddr(pid,(LPWSTR)buf);
      //    break;
      //  case 0:
      //    //entry_table->RegisterNewHook(new_engine_addr,(LPWSTR)(buf+0xC),pid);
      //    break;
      //  case -1:
      //    swprintf((LPWSTR)buf,L"%.4d:",pid);
      //    buf[0xA]=0x20;
      //    //ConsoleOutput((LPWSTR)buf);
      //    break;
      //  }
      }
    }

    // Close pipe

    ::EnterCriticalSection(&csDetach_);

    disconnectPipe_(hTextPipe, &ios);

    HANDLE hCmdPipe = HOOKMAN->GetCmdHandleByPID(pid);
    disconnectPipe_(hCmdPipe, &ios);

    synchronizedDetachFromProcess_(pid);

    HOOKMAN->UnRegisterProcess(pid);

    ::NtClearEvent(hDetachEvent_);

    ::LeaveCriticalSection(&csDetach_);

    delete buf;
    return 0;
  }

  void
  createPipe_()
  {
    DWORD acl[7] = { 0x1C0002, 1, 0x140000, GENERIC_READ|GENERIC_WRITE|SYNCHRONIZE, 0x101, 0x1000000, 0};
    SECURITY_DESCRIPTOR sd = { 1, 0, 4, 0, 0, 0, (PACL)acl};

    LARGE_INTEGER time = { -500000, -1};
    IO_STATUS_BLOCK ios;

    HANDLE hTextPipe; {
      UNICODE_STRING us = RTL_CONSTANT_STRING(ITH_TEXT_PIPE);
      OBJECT_ATTRIBUTES oa={sizeof(oa),0,&us,OBJ_CASE_INSENSITIVE,&sd,0};
      NTSTATUS status = ::NtCreateNamedPipeFile(&hTextPipe, GENERIC_READ|SYNCHRONIZE, &oa, &ios,
          FILE_SHARE_WRITE,FILE_OPEN_IF,FILE_SYNCHRONOUS_IO_NONALERT,1,1,0,-1,0x1000,0x1000,&time);
      if (!NT_SUCCESS(status)) {
        DOUT("failed to create text pipe ");
        return;
      }
    }

    HANDLE hCmdPipe; {
      UNICODE_STRING us = RTL_CONSTANT_STRING(ITH_COMMAND_PIPE);
      OBJECT_ATTRIBUTES oa={sizeof(oa),0,&us,OBJ_CASE_INSENSITIVE,&sd,0};
      NTSTATUS status = ::NtCreateNamedPipeFile(&hCmdPipe, GENERIC_WRITE|SYNCHRONIZE, &oa, &ios,
          FILE_SHARE_READ,FILE_OPEN_IF,FILE_SYNCHRONOUS_IO_NONALERT,1,1,0,-1,0x1000,0x1000,&time);
      if (!NT_SUCCESS(status)) {
        DOUT("failed to create cmd pipe");
        ::NtClose(hTextPipe);
        return;
      }
    }

    HANDLE hThread = ::IthCreateThread(::recvThread_, (DWORD)hTextPipe);
    man->RegisterPipe(hTextPipe, hCmdPipe, hThread);
  }

} // anonymous namespace

// EOF

/*
enum ThreadOperation { Suspend, Resume, Terminate, OutputInformation };

void OutputDWORD(DWORD d)
{
  WCHAR str[0x20];
  swprintf(str,L"%.8X",d);
  ConsoleOutput(str);
}

DWORD WINAPI CmdThread(LPVOID lpThreadParameter)
{
  CommandQueue *q = (CommandQueue*)lpThreadParameter;
  while (running)
    q->SendCommand();
  return 0;
}

CommandQueue::CommandQueue():used(0),current(1)
{
  DOUT("enter");
  InitializeCriticalSection(&rw);
  NtCreateSemaphore(&hSemaphore,SEMAPHORE_ALL_ACCESS,0,0,QUEUE_MAX);
  hThread=IthCreateThread(CmdThread,(DWORD)this);
  DOUT("exit");
}
CommandQueue::~CommandQueue()
{
  DOUT("enter");
  NtReleaseSemaphore(hSemaphore,1,0);

  DOUT("FIXME: termating hanging hThread ...");
  // jichi:10/15/2011: hanged orz
  //NtWaitForSingleObject(hThread,0,0);
  NtTerminateThread(hThread, 0);

  NtClose(hSemaphore);
  NtClose(hThread);
  DeleteCriticalSection(&rw);
  DOUT("exit");
}
void CommandQueue::AddRequest(const SendParam& sp, DWORD pid)
{
  if (current==used) {
    //ConsoleOutput(ErrorCmdQueueFull);
    DOUT("command queue is full");
  }
  EnterCriticalSection(&rw);
  queue[current]=sp;
  if (pid)
    pid_associate[current++]=pid;
  else
  {
    pid=man->GetCurrentPID();
    if (pid)
      pid_associate[current++]=pid;
    else
    {
      //ConsoleOutput(ErrorNoAttach);
      DOUT("error: not attached ");
      goto _request_exit;
    }
  }
  current&=(QUEUE_MAX-1);
  NtReleaseSemaphore(hSemaphore,1,0);
_request_exit:
  LeaveCriticalSection(&rw);
}
void CommandQueue::SendCommand()
{
  NtWaitForSingleObject(hSemaphore,0,0);
  if (!running) return;
  EnterCriticalSection(&rw);
  SendParam sp;
  DWORD pid;
  HANDLE pipe;
  used=(used+1)&(QUEUE_MAX-1);
  sp=queue[used];
  pid=pid_associate[used];
  pipe=man->GetCmdHandleByPID(pid);
  if (pipe)
  {
    IO_STATUS_BLOCK ios;
    NtWriteFile(pipe,0,0,0,&ios,&sp,sizeof(SendParam),0,0);
  }
  LeaveCriticalSection(&rw);
}
bool CommandQueue::Empty()
{
  return ((used+1)&(QUEUE_MAX-1))==current;
}
void CommandQueue::Register(DWORD pid, DWORD hookman, DWORD module, DWORD engine)
{
  man->RegisterProcess(pid,hookman,module,engine);
}
*/
