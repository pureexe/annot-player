// ith.cc
// 10/15/2011

#include "ith.h"
#include "textthread_p.h"
#include <ITH/IHF.h>

//#define ITH_RUNNING_EVENT L"ITH_PIPE_EXIST"
//#define ITH_RUNNING_MUTEX L"ITH_RUNNING"
//#define ITH_MUTEX_NAME  L"ITH_MAIN_RUNNING"

///#define DEBUG "ith"
#include "module/debug/debug.h"

// - Construction -

HookManager *Ith::hookManager_;
QHash<TextThread *, TextThreadProperty *> Ith::threadProperties_;
qint64 Ith::messageInterval_ = 200; // 0.2 secs by default
WId Ith::parentWindow_;

bool
Ith::load()
{
  DOUT("enter");
  if (hookManager_) {
    DOUT("exit: already loaded");
    return true;
  }

  // Single instance protection
  //HANDLE hMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, ITH_MUTEX_NAME); // in kernel32.dll
  //if (hMutex != 0 || ::GetLastError() != ERROR_FILE_NOT_FOUND) {
  //  ::CloseHandle(hMutex);
  //  return false;
  //}

  if (IHF_Init()) {
    IHF_GetHookManager(&hookManager_);
    if (hookManager_) {
      //hookManager_->RegisterProcessAttachCallback(ProcessAttach);
      //hookManager_->RegisterProcessDetachCallback(ProcessDetach);
      //hookManager_->RegisterProcessNewHookCallback(ProcessNewHook);
      hookManager_->RegisterThreadCreateCallback(threadCreateCallback);
      hookManager_->RegisterThreadRemoveCallback(threadRemoveCallback);
      IHF_Start();
    }
  }
  else
    IHF_Cleanup();
  DOUT("exit: hook manager =" << hookManager_);
  return hookManager_;
}

void
Ith::unload()
{
  DOUT("enter: hook manager =" << hookManager_);
  if (hookManager_)
    IHF_Cleanup();
  DOUT("exit");
}

// - Callbacks -

DWORD
Ith::threadCreateCallback(TextThread *t)
{
  DOUT("enter: pid =" << t->PID());
  Q_ASSERT(t);

  TextThreadProperty *d = new TextThreadProperty(t, messageInterval_, parentWindow_);
  threadProperties_[t] = d;
  t->RegisterOutputCallBack(threadOutputCallback, d);
  DOUT("exit");
  return 0;
}

DWORD
Ith::threadRemoveCallback(TextThread *t)
{
  DOUT("enter");
  Q_ASSERT(t);
  auto p = threadProperties_.find(t);
  if (p != threadProperties_.end()) {
    delete p.value();
    threadProperties_.erase(p);
  }
  DOUT("exit");
  return 0;
}

DWORD
Ith::threadOutputCallback(TextThread *t, BYTE *data,DWORD dataLength, DWORD newLine, PVOID pUserData)
{
  DOUT("enter: newLine =" << newLine << ", dataLength =" << dataLength);
  Q_UNUSED(t);
  Q_ASSERT(data);

  auto d = static_cast<TextThreadProperty *>(pUserData);
  Q_ASSERT(d);
  if (newLine)
    d->flush();

  d->append(reinterpret_cast<char *>(data), dataLength);

  //QString text = QString::fromLocal8Bit(reinterpret_cast<LPCSTR>(data), len);
  DOUT("exit");
  return dataLength;
}

// - Injections -

bool
Ith::attachProcess(DWORD pid)
{
  DOUT("enter: pid =" << pid);
  //return ::InjectProcessByPid(pid) != INVALID_HANDLE_VALUE;
  DWORD module = IHF_InjectByPID(pid, 0); // 0 means the default profile is used
  DOUT("exit: module =" << module);
  return module != -1;
}

bool
Ith::detachProcess(DWORD pid)
{
  DOUT("enter");
  //return !::DetachProcessByPid(pid);
  DWORD status = IHF_ActiveDetachProcess(pid);
  DOUT("exit: status =" << status);
  return !status;
}

// EOF

//QString
//Ith::getHookNameById(ulong hookId)
//{
//  QString ret;
//  if (hookId) {
//    auto p = reinterpret_cast<TextThread *>(hookId);
//    if (p->good())
//      ret = p->name();
//  }
//  return ret;
//}

//DWORD ProcessAttach(DWORD pid)
//{
//  DOUT("process attached, pid =" << pid);
//  return 0;
//}
//DWORD ProcessDetach(DWORD pid)
//{
//  DOUT("process detached, pid =" << pid);
//  return 0;
//}
//DWORD ProcessNewHook(DWORD pid)
//{
//  DOUT("process has new hook inserted, pid =" << pid);
//  return 0;
//}
