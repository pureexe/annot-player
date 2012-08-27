// ith_p.cc
// 10/15/2011

#include "ith_p.h"
#include <ITH/common.h>
#include <string>

//#define DEBUG "ith"
#include "module/debug/debug.h"

// - Implementation Details -

namespace { namespace detail {

// ITH ORIGINAL CODE BEGIN

  // See: ITH/ITH.h
  // Revision: 133
  inline DWORD Hash(LPWSTR module, int length = -1)
  {
    bool flag=(length==-1);
    DWORD hash=0;
    for (;*module&&(flag||length--);module++)
    {
    hash = _rotr(hash,7) + *module;
    //hash=((hash>>7)|(hash<<25))+(*module);
    }
    return hash;
  }

  // See: ITH/command.cpp
  // Revision: 133
  int Convert(LPWSTR str, DWORD *num, LPWSTR delim)
  {
    if (num == 0) return -1;
    WCHAR t = *str,tc = *(str+0xF);
    WCHAR temp[0x10]={};
    LPWSTR it = temp,istr = str,id = temp;
    if (delim)
    {
      id=wcschr(delim, t);
      str[0xF] = delim[0];
    }
    else str[0xF] = 0;
    while (id == 0 && t)
    {
      *it = t;
      it++; istr++;
      t = *istr;
      if (delim) id = wcschr(delim, t);
    }
    swscanf(temp, L"%x", num);
    str[0xF] = tc;
    if (id == 0 || istr - str == 0xF) return -1;
    if (t == 0) return istr - str;
    else return id - delim;
  }

  // See: ITH/command.cpp
  // Revision: 133
  bool Parse(LPWSTR cmd, HookParam& hp)
  {
    int t;
    bool accept = false;
    memset(&hp, 0, sizeof(hp));
    DWORD *data = &hp.off;
    LPWSTR offset = cmd + 1;
    LPWSTR delim_str = L":*@!";
    LPWSTR delim = delim_str;
    if (*offset == L'n' || *offset == 'N')
    {
      offset++;
      hp.type |= NO_CONTEXT;
    }
    while (!accept)
    {
      t = Convert(offset, data, delim);
      if (t < 0)
      {
  _error:
        //ConsoleOutput(L"Syntax error.");
        return false;
      }
      offset = wcschr(offset , delim[t]);
      if (offset) offset++;
      else goto _error;
      switch (delim[t])
      {
      case L':':
        data = &hp.split;
        delim = delim_str+1;
        hp.type |= USING_SPLIT;
        break;
      case L'*':
        if (hp.split)
        {
          data = &hp.split_ind;
          delim = delim_str+2;
          hp.type |= SPLIT_INDIRECT;
        }
        else
        {
          hp.type |= DATA_INDIRECT;
          data = &hp.ind;
        }
        break;
      case L'@':
        accept = true;
        break;
      }
    }
    t = Convert(offset, &hp.addr, delim_str);
    if (t < 0) return false;
    if (hp.off & 0x80000000) hp.off -= 4;
    if (hp.split & 0x80000000) hp.split -= 4;
    LPWSTR temp = offset;
    offset = wcschr(offset, L':');
    if (offset)
    {
      hp.type |= MODULE_OFFSET;
      offset++;
      delim = wcschr(offset, L':');

      if (delim)
      {
        *delim = 0;
        delim++;
        _wcslwr(offset);
        hp.function = Hash(delim);
        hp.module = Hash(offset, delim - offset - 1);
        hp.type |= FUNCTION_OFFSET;
      }
      else
      {
        hp.module = Hash(_wcslwr(offset));
      }
    }
    else
    {
      offset = wcschr(temp, L'!');
      if (offset)
      {
        hp.type |= MODULE_OFFSET;
        swscanf(offset + 1, L"%x", &hp.module);
        offset = wcschr(offset + 1, L'!');
        if (offset)
        {
          hp.type |= FUNCTION_OFFSET;
          swscanf(offset + 1, L"%x", &hp.function);
        }
      }
    }
    switch (*cmd)
    {
    case L's':
    case L'S':
      hp.type |= USING_STRING;
      break;
    case L'e':
    case L'E':
      hp.type |= STRING_LAST_CHAR;
    case L'a':
    case L'A':
      hp.type |= BIG_ENDIAN;
      hp.length_offset = 1;
      break;
    case L'b':
    case L'B':
      hp.length_offset = 1;
      break;
    case L'h':
    case L'H':
      hp.type |= PRINT_DWORD;
    case L'q':
    case L'Q':
      hp.type |= USING_STRING | USING_UNICODE;
      break;
    case L'l':
    case L'L':
      hp.type |= STRING_LAST_CHAR;
    case L'w':
    case L'W':
      hp.type |= USING_UNICODE;
      hp.length_offset = 1;
      break;
    default:
      break;
    }
    //ConsoleOutput(L"Try to insert additional hook.");
    return true;
  }

// ITH ORIGINAL CODE END

} } // anonymous namespace detail

// - ITH API -

// Sample code: L"/HS-4:-14@4383C0" (WHITE ALBUM 2)
bool
Ith::parseHookCode(const QString &code, HookParam *hp)
{
#define HOOK_CODE_PREFIX  "/H"
  if (!hp || !code.startsWith(HOOK_CODE_PREFIX))
    return false;
  DOUT("enter: code =" << code);

  size_t bufsize = code.size() + 1;
  wchar_t *buf = new wchar_t[bufsize];
  code.toWCharArray(buf);
  buf[bufsize -1] = 0;

  bool ret = detail::Parse(buf + sizeof(HOOK_CODE_PREFIX), *hp);
  delete buf;
  DOUT("exit: ret =" << ret);
  return ret;
#undef HOOK_CODE_PREFIX
}

// EOF
