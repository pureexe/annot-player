#pragma once
#include <windows.h>
extern "C" {
extern WORD* NlsAnsiCodePage;
int disasm(BYTE* opcode0);
int FillRange(LPWSTR name,DWORD* lower, DWORD* upper);
int MB_WC(char* mb, wchar_t* wc);
int MB_WC_count(char* mb, int mb_length);
int WC_MB(wchar_t *wc, char* mb);
DWORD SearchPattern(DWORD base, DWORD base_length, LPVOID search, DWORD search_length); //KMP
void IthInitSystemService();
void IthCloseSystemService();
DWORD IthGetMemoryRange(LPVOID mem, DWORD* base, DWORD* size);
BOOL IthCheckFile(LPWSTR file);
BOOL IthFindFile(LPWSTR file);
BOOL IthGetFileInfo(LPWSTR file, LPVOID info);
BOOL IthCheckFileFullPath(LPWSTR file);
HANDLE IthCreateFile(LPWSTR name, DWORD option, DWORD share, DWORD disposition);
HANDLE IthCreateDirectory(LPWSTR name);
HANDLE IthCreateFileFullPath(LPWSTR full_path, DWORD option, DWORD share, DWORD disposition);
HANDLE IthPromptCreateFile(DWORD option, DWORD share, DWORD disposition);
HANDLE IthCreateSection(LPWSTR name, DWORD size, DWORD right);
HANDLE IthCreateEvent(LPWSTR name, DWORD auto_reset=0, DWORD init_state=0);
HANDLE IthOpenEvent(LPWSTR name);
void IthSetEvent(HANDLE hEvent);
void IthResetEvent(HANDLE hEvent);
HANDLE IthCreateMutex(LPWSTR name, BOOL InitialOwner, DWORD* exist=0);
HANDLE IthOpenMutex(LPWSTR name);
BOOL IthReleaseMutex(HANDLE hMutex);
DWORD IthWaitForSingleObject(HANDLE hObject, DWORD dwTime);
HANDLE IthCreateThread(LPVOID start_addr, DWORD param, HANDLE hProc=(HANDLE)-1);
DWORD GetExportAddress(DWORD hModule,DWORD hash);
void IthSleep(int time);
void IthSystemTimeToLocalTime(LARGE_INTEGER* ptime);
void FreeThreadStart(HANDLE hProc);
void CheckThreadStart();
}
extern HANDLE hHeap;
extern DWORD current_process_id,nt_flag;
extern BYTE LeadByteTable[];
extern LPVOID page;
extern BYTE launch_time[];
inline DWORD GetHash(LPSTR str)
{
  DWORD hash=0;
  for (;*str;str++)
  {
    hash=((hash>>7)|(hash<<25))+(*str);
  }
  return hash;
}
inline DWORD GetHash(LPWSTR str)
{
  DWORD hash=0;
  for (;*str;str++)
  {
    hash=((hash>>7)|(hash<<25))+(*str);
  }
  return hash;
}
