#pragma once
// iht/main.h  10/14/2011
#include "ith/common.h"
#include "ith/sys.h"
#include "language.h"
#pragma comment(linker,"/manifestdependency:\"type='win32' "\
  "name='Microsoft.Windows.Common-Controls' version='6.0.0.0' "\
  "processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define GLOBAL extern
#define SHIFT_JIS 0x3A4
class TextBuffer;
class HookManager;
class CommandQueue;
class TextHook;
class BitMap;
class CustomFilterMultiByte;
class CustomFilterUnicode;
//class ProfileManager;
#define TextHook Hook
GLOBAL bool running;

//GLOBAL HINSTANCE hIns;
GLOBAL BitMap *pid_map;
GLOBAL CustomFilterMultiByte *mb_filter;
GLOBAL CustomFilterUnicode *uni_filter;
GLOBAL TextBuffer *texts;
GLOBAL HookManager *man;
//GLOBAL ProfileManager *pfman;
GLOBAL CommandQueue *cmdq;
GLOBAL HWND hMainWnd;
GLOBAL WCHAR pipe[];
GLOBAL WCHAR command[];
GLOBAL HANDLE hPipeExist;
GLOBAL DWORD split_time, process_time;// inject_delay, insert_delay;
//GLOBAL DWORD auto_inject, auto_insert;
GLOBAL DWORD cyclic_remove,global_filter;
GLOBAL CRITICAL_SECTION detach_cs;
DWORD WINAPI RecvThread(LPVOID lpThreadParameter);
DWORD WINAPI CmdThread(LPVOID lpThreadParameter);

void CopyToClipboard(void* str,bool unicode, int len);
void ConsoleOutput(LPCWSTR text);
DWORD  GetCurrentPID();
DWORD  GetPIDByHandle(HANDLE h);
DWORD  GetHookManByPID(DWORD pid);
DWORD  GetModuleByPID(DWORD pid);
DWORD  GetEngineByPID(DWORD pid);
DWORD  GetProcessIDByPath(LPWSTR str);
HANDLE  GetTextHandleByPID(DWORD pid);
HANDLE  GetCmdHandleByPID(DWORD pid);
HANDLE  GetMutexByPID(DWORD pid);
HANDLE  GetProcessByPID(DWORD pid);
DWORD  Inject(HANDLE hProc);
DWORD  InjectByPID(DWORD pid);
DWORD  PIDByName(LPWSTR target);
DWORD  Hash(LPWSTR module, int length=-1);
BOOL ActiveDetachProcess(DWORD pid);
BOOL CheckFile(LPWSTR file);
bool GetProcessPath(HANDLE hProc, LPWSTR path);
bool GetProcessPath(DWORD pid, LPWSTR path);
// EOF
