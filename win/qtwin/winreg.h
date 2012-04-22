#pragma once

// winreg.h
// 4/21/2012
// See: http://en.wikipedia.org/wiki/Windows_Registry

#define REG_HKCR    "HKEY_CLASSES_ROOT"
#define REG_HKCU    "HKEY_CURRENT_USER"
#define REG_HKCC    "HKEY_CURRENT_CONFIG"
#define REG_HKLM    "HKEY_LOCAL_MACHINE"
#define REG_HKU     "HKEY_USERS"

#define REG_HKCU_SOFTWARE           REG_HKCU "\\Software"
#define REG_HKCU_SOFTWARE_CLASSES   REG_HKCU_SOFTWARE "\\Classes"

#define REG_HKLM_SOFTWARE           REG_HKLM "\\Software"
#define REG_HKLM_SOFTWARE_CLASSES   REG_HKLM_SOFTWARE "\\Classes"
#define REG_HKLM_SYSTEM             REG_HKLM "\\System"

// EOF
