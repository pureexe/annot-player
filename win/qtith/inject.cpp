#include "main.h"
#include <QtCore>
static WCHAR EngineName[]=L"ITH_engine.dll";
static WCHAR DllName[]=L"ITH.dll";
extern WCHAR file_path[];
extern LPWSTR current_dir;
DWORD Inject(HANDLE hProc)
{
  LPVOID lpvAllocAddr = 0;
  DWORD dwWrite = 0x1000;
  HANDLE hTH;
  //qDebug()<<"jichi:inject:Inject:BEGIN";
  if (!IthCheckFile(EngineName)) return -1;
  if (!IthCheckFile(DllName)) return -1;

  //qDebug()<<"jichi:inject:Inject:END";

  NtAllocateVirtualMemory(hProc, &lpvAllocAddr, 0, &dwWrite, MEM_COMMIT, PAGE_READWRITE);
  if (lpvAllocAddr == 0) return -1;
  wcscpy(current_dir, DllName);
  CheckThreadStart();
  NtWriteVirtualMemory(hProc, lpvAllocAddr, file_path + 4, 2 * MAX_PATH, &dwWrite);
  qDebug()<<QString::fromWCharArray(file_path);
  hTH=IthCreateThread(LoadLibrary, (DWORD)lpvAllocAddr, hProc);
  if (hTH==0||hTH == INVALID_HANDLE_VALUE)
  {
    ConsoleOutput(ErrorRemoteThread);
    return -1;
  }
  NtWaitForSingleObject(hTH, 0, 0);
  THREAD_BASIC_INFORMATION info;
  NtQueryInformationThread(hTH, ThreadBasicInformation, &info, sizeof(info), &dwWrite);
  NtClose(hTH);
  wcscpy(current_dir, EngineName);
  NtWriteVirtualMemory(hProc, lpvAllocAddr, file_path + 4, 2 * MAX_PATH, &dwWrite);
  hTH = IthCreateThread(LoadLibrary, (DWORD)lpvAllocAddr, hProc);
  if (hTH == 0 ||
    hTH == INVALID_HANDLE_VALUE)
  {
    ConsoleOutput(ErrorRemoteThread);
    return -1;
  }
  NtWaitForSingleObject(hTH, 0, 0);
  NtClose(hTH);
  dwWrite = 0;
  NtFreeVirtualMemory(hProc, &lpvAllocAddr, &dwWrite, MEM_RELEASE);
  qDebug() << "jichi:inject: ret ="<<info.ExitStatus;
  return info.ExitStatus;
}
DWORD PIDByName(LPWSTR pwcTarget)
{
  DWORD dwSize = 0x20000;
  BYTE *pbBuffer;
  SYSTEM_PROCESS_INFORMATION *spiProcessInfo;
  DWORD dwPid = 0;
  DWORD dwStatus;
  while (1)
  {
    pbBuffer = new BYTE[dwSize];
    dwStatus = NtQuerySystemInformation(SystemProcessInformation, pbBuffer, dwSize, 0);
    if (dwStatus == 0) break;
    delete pbBuffer;
    if (dwStatus != STATUS_INFO_LENGTH_MISMATCH) return 0;
    dwSize <<= 1;
  }

  for (spiProcessInfo = (SYSTEM_PROCESS_INFORMATION*)pbBuffer; spiProcessInfo->dNext;)
  {
    spiProcessInfo = (SYSTEM_PROCESS_INFORMATION*)
      ((DWORD)spiProcessInfo + spiProcessInfo->dNext);
    if (_wcsicmp(pwcTarget, spiProcessInfo->usName.Buffer) == 0)
    {
      dwPid = spiProcessInfo->dUniqueProcessId;
      break;
    }
  }
  if (dwPid == 0)
    ConsoleOutput(ErrorNoProcess);
  delete pbBuffer;
  return dwPid;
}
DWORD InjectByPID(DWORD pid)
{
  WCHAR str[0x80];
  DWORD s;
  if (pid == current_process_id)
  {
    ConsoleOutput(SelfAttach);
    return -1;
  }
  if (GetModuleByPID(pid))
  {
    ConsoleOutput(AlreadyAttach);
    return -1;
  }
  swprintf(str, L"ITH_HOOKMAN_%.4d", pid);
  NtClose(IthCreateMutex(str, 0, &s));
  if (s) return -1;
  CLIENT_ID id;
  OBJECT_ATTRIBUTES oa = {};
  HANDLE hProc;
  id.UniqueProcess = pid;
  id.UniqueThread = 0;
  oa.uLength=sizeof(oa);
  if (!NT_SUCCESS(NtOpenProcess(&hProc,
    PROCESS_QUERY_INFORMATION|
    PROCESS_CREATE_THREAD|
    PROCESS_VM_OPERATION|
    PROCESS_VM_READ|
    PROCESS_VM_WRITE,
    &oa, &id)))
  {
    ConsoleOutput(ErrorOpenProcess);
    return -1;
  }
  DWORD module = Inject(hProc);

  NtClose(hProc);
  if (module == -1) return -1;

  swprintf(str, FormatInject, pid, module);
  ConsoleOutput(str);
  return module;
}
