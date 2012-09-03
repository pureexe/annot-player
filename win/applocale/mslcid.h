#pragma once

// mslcid.h
// 9/1/2012
// See: http://download.microsoft.com/download/9/5/E/95EF66AF-9026-4BB0-A41D-A4F81802D92C/%5BMS-LCID%5D.pdf

#define LCID_JA_JP  0x0411
#define LCID_ZH_CN  0x0804

inline const char *lcidToString(int lcid)
{
  switch (lcid) {
  case LCID_JA_JP: return "ja-JP";
  case LCID_ZH_CN: return "zh-CN";
  default: return 0;
  }
}

// EOF
