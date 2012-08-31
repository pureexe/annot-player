// ihf_p.cc
// 10/15/2011

//#define XXX "/HW4@6E8F0!B5346E41" // tukumonotuki
#define XXX "/HSN-4@B14B4!F20AE0D2" // koichoko
#include <QDebug>

#include "ihf_p.h"
#include "ith_p.h"
#include "textthread_p.h"
#include <ITH/IHF.h>
//#include <ntdll.h>

//#define ITH_RUNNING_EVENT L"ITH_PIPE_EXIST"
//#define ITH_RUNNING_MUTEX L"ITH_RUNNING"
//#define ITH_MUTEX_NAME  L"ITH_MAIN_RUNNING"

//#define DEBUG "ihf"
#include "module/debug/debug.h"

// - Construction -

HookManager *Ihf::hookManager_;
qint64 Ihf::messageInterval_ = 200; // 0.2 secs by default
WId Ihf::parentWindow_;

QHash<TextThread *, TextThreadDelegate *> Ihf::threadDelegates_;
QHash<QString, ulong> Ihf::hookAddresses_;

// See also: HelloITH/main.cpp
bool
Ihf::load()
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
Ihf::unload()
{
  DOUT("enter: hook manager =" << hookManager_);
  if (hookManager_) {
    hookManager_->RegisterThreadCreateCallback(0);
    hookManager_->RegisterThreadRemoveCallback(0);
    IHF_Cleanup();
  }
  DOUT("exit");
}

// - Callbacks -

// See: HelloITH/main.cpp
// See: ThreadCreate in ITH/window.cpp
DWORD
Ihf::threadCreateCallback(TextThread *t)
{
  DOUT("enter: pid =" << t->PID());
  Q_ASSERT(hookManager_);
  Q_ASSERT(t);

  // Propagate UNICODE
  // See: ThreadCreate in ITH/window.cpp
  //if (ProcessRecord *pr = hookManager_->GetProcessRecord(t->PID())) {
  //  NtWaitForSingleObject(pr->hookman_mutex, 0, 0);
  //  Hook *hk = static_cast<Hook *>(pr->hookman_map);
  //  Q_ASSERT(!hk&&!MAX_HOOK || hk&&MAX_HOOK);
  //  for (int i = 0; i < MAX_HOOK; i++) {
  //    if (hk[i].Address() == t->Addr()) {
  //      if (hk[i].Type() & USING_UNICODE)
  //        t->Status() |= USING_UNICODE;
  //      break;
  //    }
  //  }
  //  NtReleaseMutant(pr->hookman_mutex, 0);
  //}

  TextThreadDelegate *d = new TextThreadDelegate(t, messageInterval_, parentWindow_);
  threadDelegates_[t] = d;
  t->RegisterOutputCallBack(threadOutputCallback, d);
  DOUT("exit");
  return 0;
}

// See also: HelloITH/main.cpp
DWORD
Ihf::threadRemoveCallback(TextThread *t)
{
  DOUT("enter");
  Q_ASSERT(t);
  auto p = threadDelegates_.find(t);
  if (p != threadDelegates_.end()) {
    delete p.value();
    threadDelegates_.erase(p);
  }
  DOUT("exit");
  return 0;
}

// See: HelloITH/main.cpp
DWORD
Ihf::threadOutputCallback(TextThread *t, BYTE *data,DWORD dataLength, DWORD newLine, PVOID pUserData)
{
  DOUT("enter: newLine =" << newLine << ", dataLength =" << dataLength);
  Q_UNUSED(t)
  Q_ASSERT(data);

  auto d = static_cast<TextThreadDelegate *>(pUserData);
  Q_ASSERT(d);
  if (newLine)
    d->flush();

  d->append(reinterpret_cast<char *>(data), dataLength);

  //QString text = QString::fromLocal8Bit(reinterpret_cast<LPCSTR>(data), len);
  DOUT("exit");
  return dataLength;
}

// - Injection -

// See: IHF_InjectByPID in IHF/main.cpp
// See: InjectThread in ITH/profile.cpp
bool
Ihf::attachProcess(DWORD pid)
{
  DOUT("enter: pid =" << pid);
  DWORD module = IHF_InjectByPID(pid, 0); // 0 means the default engine is used

  // CHECKPOINT XXX
  //enum { InsertDelay = 500 }; // in msec
  //::Sleep(InsertDelay);
  //
  //wchar_t name[] = L"fuck";
  //HookParam hp;
  //qDebug()<<1111<<Ith::parseHookCode(XXX, &hp);
  //qDebug()<<2222<<IHF_InsertHook(pid, &hp, name);

  DOUT("exit: module =" << module);
  return module +1;
}

// See: IHF_ActiveDetachProcess in IHF/main.cpp
bool
Ihf::detachProcess(DWORD pid)
{
  DOUT("enter");
  DWORD ok = IHF_ActiveDetachProcess(pid);
  DOUT("exit: ret =" << ok);
  return ok;
}

// - Hook -

// See: IHF_ModifyHook in IHF/main.cpp
bool
Ihf::updateHook(ulong pid, const QString &code)
{
  DOUT("enter: pid =" << pid << ", code =" << code);
  Q_ASSERT(pid);
  HookParam hp;
  if (!Ith::parseHookCode(code, &hp)) {
    DOUT("exit: failed to parse hook code");
    return false;
  }

  DWORD hh = IHF_ModifyHook(pid, &hp);
  bool ret = hh +1;
  DOUT("exit: ok =" << ret);
  return ret;
}

// See: IHF_InsertHook in IHF/main.cpp
bool
Ihf::addHook(ulong pid, const QString &code, const QString &name)
{
  DOUT("enter: pid =" << pid << ", name =" << name << ", code =" << code);
  Q_ASSERT(pid);
  if (hookAddresses_.contains(code)) {
    DOUT("exit: already added");
    return false;
  }

  HookParam hp;
  if (!Ith::parseHookCode(code, &hp)) {
    DOUT("exit: failed to parse hook code");
    return false;
  }

  wchar_t *nameBuf = 0;
  if (!name.isEmpty()) {
    size_t nameBufSize = name.size() + 1;
    nameBuf = new wchar_t[nameBufSize];
    nameBuf[nameBufSize -1] = 0;
    name.toWCharArray(nameBuf);
  }
  DWORD hh = IHF_InsertHook(pid, &hp, nameBuf);
  bool ret = hh +1;
  if (ret && hp.addr) {
    DOUT("hook address =" << hp.addr);
    hookAddresses_[code] = hp.addr;
  }
  if (nameBuf)
    delete nameBuf;
  DOUT("exit: ok =" << ret);
  return ret;
}

// See: IHF_RemoveHook in IHF/main.cpp
bool
Ihf::removeHook(ulong pid, const QString &code)
{
  DOUT("enter: pid =" << pid << ", code =" << code);
  Q_ASSERT(pid);
  auto p = hookAddresses_.find(code);
  if (p == hookAddresses_.end()) {
    DOUT("exit: hook not added");
    return false;
  }
  DWORD addr = p.value();
  Q_ASSERT(addr);
  hookAddresses_.erase(p);

  DWORD hh = IHF_RemoveHook(pid, addr);
  bool ret = hh +1;
  DOUT("exit: ok =" << ret);
  return ret;
}

// EOF

//QString
//Ihf::getHookNameById(ulong hookId)
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
