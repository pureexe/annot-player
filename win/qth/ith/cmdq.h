#pragma once
// ith/cmdq.h  10/14/2011
#include "ith/main.h"
enum ThreadOperation
{
  Suspend,
  Resume,
  Terminate,
  OutputInformation
};
struct PipeRecord
{
  HANDLE hTextPipe, hCmdPipe, hThread;
};
void CreateNewPipe();
#define QUEUE_MAX 16 //QUEUE_MAX need to be a exponent of 2;
#define QUEUE_BUFF_SIZE 0x40
#define CMD_SIZE 0x200

class CommandQueue
{
public:
  CommandQueue();
  ~CommandQueue();
  void AddRequest(const SendParam& sp,DWORD pid=0);
  void SendCommand();
  bool Empty();
  void Register(DWORD pid, DWORD hookman, DWORD module, DWORD engine);
  DWORD ProcessCommand(LPWSTR cmd, DWORD pid);
private:
  CRITICAL_SECTION rw;
  DWORD current;
  DWORD used;
  HANDLE hSemaphore,hThread;
  SendParam queue[QUEUE_MAX];
  DWORD pid_associate[QUEUE_MAX];
};
// EOF
