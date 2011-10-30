#pragma once
#ifndef QTWIN_H
#define QTWIN_H

// qtwin.h
// 7/21/2011

#include <QList>
#include <QWidget>
#include <QString>

namespace QtWin {

  // Direct access to windows DLL module
  namespace Module { }

  // Direct access windows DLL api
  namespace Api { }

  // - Main module -

  ///  Return main module path as LPWSTR
  inline wchar_t *getModulePath()
  { __asm {
    mov eax,fs:[0x30]
    mov eax,[eax+0xC]
    mov eax,[eax+0xC]
    mov eax,[eax+0x28]
  } }

  ///  Return main module handle as HINSTANCE
  inline void *getModuleBase()
  { __asm {
    mov eax,fs:[0x30]
    mov eax,[eax+0x8]
  } }

  // - Processes and thread -
  QList<ulong> getThreadIdsByProcessId(ulong dwOwnerPID);
  QList<ulong> getProcessIdsByParentProcessId(ulong dwOwnerPID);

  bool createProcessWithExecutablePath(const QString &path);

  bool isProcessActiveWithId(ulong dwProcessId);

  QList<ulong> getProcessIdsByName(const QString &processName);
  ulong getProcessIdByName(const QString &processName);

  struct ProcessInfo {
    ulong id;  ///< default is 0
    QString name; ///< name of the executable by default, unless dynamically created

    ProcessInfo() : id(0) { }
  };
  QList<ProcessInfo> getProcessesInfo();

  QString getProcessPathById(ulong dwProcessId);

  struct ModuleInfo {
    ulong moduleId;   ///< default is 0
    ulong processId;  ///< default is 0
    QString moduleName; ///< name of the executable by default, unless dynamically created
    QString modulePath; ///< full path

    ModuleInfo() : moduleId(0), processId(0) { }
  };
  QList<ModuleInfo> getModulesInfo();

  // - Widgets -

  ///  Set focus using a native way. If failed, return false but still do it in Qt way.
  bool setFocus(QWidget *w);
  bool setFocus(WId hwnd); // Same as windows version.

  WId getChildWindow(WId hwnd);

  // - Mouse and keyboard -
  void sendMouseClick(const QPoint& globalPos, Qt::MouseButton button);

  // - Environments -
  QString getWinDir();

  // - Files -

  ///  Create a lnk file at \param lnkPath pointed to \param targetPath.
  bool createLink(const QString &lnkPath, const QString &targetPath, const QString &description = QString());

  ///  Prompt using hwnd if target to return is not valid.
  QString resolveLink(const QString &lnkPath, WId winId = 0);

} // namespace QtWin

#endif // QTWIN_H
