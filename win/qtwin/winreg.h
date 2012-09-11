#ifndef WINREG_H
#define WINREG_H

// winreg.h
// 4/21/2012
// See: http://en.wikipedia.org/wiki/Windows_Registry

// - Roots -

#define REG_HKCR    "HKEY_CLASSES_ROOT"
#define REG_HKCU    "HKEY_CURRENT_USER"
#define REG_HKCC    "HKEY_CURRENT_CONFIG"
#define REG_HKLM    "HKEY_LOCAL_MACHINE"
#define REG_HKU     "HKEY_USERS"

// - Common Paths -

#define REG_PATH_SOFTWARE           "\\Software"
#define REG_PATH_CLASSES            REG_PATH_SOFTWARE "\\Classes"
#define REG_PATH_MICROSOFT          REG_PATH_SOFTWARE "\\Microsoft"
#define REG_PATH_WINDOWS            REG_PATH_MICROSOFT "\\Windows"
#define REG_PATH_CURRENTVERSION     REG_PATH_WINDOWS "\\CurrentVersion"
#define REG_PATH_EXPLORER           REG_PATH_CURRENTVERSION "\\EXPLORER"
#define REG_PATH_SHELLFOLDERS       REG_PATH_EXPLORER "\\Shell Folders"

// - Full Locations -

#define REG_HKCU_SOFTWARE           REG_HKCU REG_PATH_SOFTWARE
#define REG_HKCU_CLASSES            REG_HKCU REG_PATH_CLASSES
#define REG_HKCU_MICROSOFT          REG_HKCU REG_PATH_MICROSOFT
#define REG_HKCU_WINDOWS            REG_HKCU REG_PATH_WINDOWS
#define REG_HKCU_CURRENTVERSION     REG_HKCU REG_PATH_CURRENTVERSION
#define REG_HKCU_EXPLORER           REG_HKCU REG_PATH_EXPLORER
#define REG_HKCU_SHELLFOLDERS       REG_HKCU REG_PATH_SHELLFOLDERS

#define REG_HKLM_SOFTWARE           REG_HKLM REG_PATH_SOFTWARE
#define REG_HKLM_CLASSES            REG_HKLM REG_PATH_CLASSES
#define REG_HKLM_MICROSOFT          REG_HKLM REG_PATH_MICROSOFT
#define REG_HKLM_WINDOWS            REG_HKLM REG_PATH_WINDOWS
#define REG_HKLM_CURRENTVERSION     REG_HKLM REG_PATH_CURRENTVERSION
#define REG_HKLM_EXPLORER           REG_HKLM REG_PATH_EXPLORER
#define REG_HKLM_SHELLFOLDERS       REG_HKLM REG_PATH_SHELLFOLDERS

// - Special Locations

#define REG_HKLM_SYSTEM             REG_HKLM "\\System"

// - Registry Readers -

#ifdef __cplusplus
#ifdef QT_CORE_LIB
#include <QtCore/QSettings>

namespace WindowsRegistry {
//
//  inline QVariant
//  value(const QString &path, const QString &key, const QVariant &defval = QVariant())
//  { return QSettings(path, QSettings::NativeFormat).value(key, defval); }
//
} // namespace WindowsRegistry

#endif // QT_CORE_LIB
#endif // __cplusplus


#endif // WINREG_H
