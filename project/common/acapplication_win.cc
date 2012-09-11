// acapplication_win.cc
// 6/6/2012
#include "project/common/acapplication.h"
#include <QtCore/QFileInfo>
#include <qt_windows.h>

#define DEBUG "acapplication_win"
#include "module/debug/debug.h"

LONG WINAPI
AcApplication::exceptionFilter(PEXCEPTION_POINTERS e)
{
  DOUT("enter");
  Q_ASSERT(e);

#ifdef DEBUG
  QString msg = QString("exception code = %1, address = %2")
      .arg(QString::number(e->ExceptionRecord->ExceptionCode, 16))
      .arg(QString::number(e->ContextRecord->Eip, 16));
  DOUT(msg);
#endif // DEBUG

  //MEMORY_BASIC_INFORMATION info;
  //NT_STATUS status = ::NtQueryVirtualMemory(
  //  NtCurrentProcess(),
  //  (PVOID)e->ContextRecord->Eip,
  //  MemoryBasicInformation, &info,sizeof(info),0
  //);
  //if (NT_SUCCESS(status) )
  //  msg += QString("\nException offset: %2").arg(
  //    QString::number(ExceptionInfo->ContextRecord->Eip - (DWORD)info.AllocationBase, 16)
  //  );

  //NtTerminateProcess(NtCurrentProcess(),0);
  // Use applicationFilePath_ instead of applicationFilePath(), which is not available after qApp is destructed
  QString app = QFileInfo(applicationFilePath_).baseName();
  DOUT("WARNING: kill all running processes:" << app);

  // app must not contains spaces or quotes
  QString cmd = "tskill " + app;
  ::WinExec(cmd.toAscii().constData(), SW_HIDE);
  //exit(-1);
  DOUT("exit");
  return 0;
}

void
AcApplication::ignoreWindowsExcetions()
{
  DOUT("enter");
  ::SetUnhandledExceptionFilter(exceptionFilter);
  DOUT("exit");
}

// EOF
