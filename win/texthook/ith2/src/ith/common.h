#pragma once
// ith/common.h
// 10/14/2011
// TODO: clean up this file
#include "ntdll.h"
#include <string>
//extern "C" {
//int swprintf(wchar_t * _String, const wchar_t * _Format, ...);
//int sprintf(char * _String, const char * _Format, ...);
//int swscanf(const wchar_t * _Src,  const wchar_t * _Format, ...);
//int sscanf(const char * _String, const char * _Format, ...);
//}

#ifdef _MSC_VER
# pragma warning(disable:4100) // C4100: unreferenced formal parameter
# pragma warning(disable:4996) // C4996: unsafe function or variable used such as swprintf, wcscpy; alternatively use __CRT_SECURE_NO_WARNINGS
# pragma warning(disable:4146) // C4146: unary minus operator applied to unsigned type, result still unsigned
#endif // _MSC_VER

#define ENGINE_KIRIKIRI 1
#define ENGINE_BGI 2
#define ENGINE_REALLIVE 3
#define ENGINE_MAJIRO 4
#define ENGINE_CMVS 5
#define ENGINE_RUGP 6
#define ENGINE_LUCIFEN 7
#define ENGINE_SYS40 8
#define ENGINE_ATELIER 9
#define ENGINE_CIRCUS 10
#define ENGINE_SHINA 11
#define ENGINE_LUNE 12
#define ENGINE_TINKER 13
#define ENGINE_WHIRLPOOL 14
#define ENGINE_COTOPHA 15
#define ENGINE_MALIE 16
#define ENGINE_SOFTHOUSE 17
#define ENGINE_CATSYSTEM 18
#define ENGINE_IGS 19
#define ENGINE_WAFFLE 20
#define ENGINE_NITROPLUS 21
#define ENGINE_DOTNET1 22
#define ENGINE_RETOUCH 23
#define ENGINE_SIGLUS 24
#define ENGINE_ABEL 25
#define ENGINE_LIVE 26
#define ENGINE_FRONTWING 27
#define ENGINE_BRUNS 28
#define ENGINE_CANDY 29
#define ENGINE_APRICOT 30
#define ENGINE_CARAMEL 31


#define USING_STRING  0x1
#define USING_UNICODE  0x2
#define BIG_ENDIAN  0x4
#define DATA_INDIRECT  0x8
#define USING_SPLIT  0x10
#define SPLIT_INDIRECT  0x20
#define MODULE_OFFSET  0x40
#define FUNCTION_OFFSET  0x80
#define PRINT_DWORD  0x100
#define STRING_LAST_CHAR 0x200
#define NO_CONTEXT  0x400
#define EXTERN_HOOK  0x800
#define CURRENT_SELECT  0x1000
#define REPEAT_NUMBER_DECIDED  0x2000
#define BUFF_NEWLINE 0x4000
#define CYCLIC_REPEAT 0x8000
#define HOOK_ENGINE 0x4000
#define HOOK_ADDITIONAL 0x8000

#define MAX_HOOK 32

struct HookParam // 0x24
{
  DWORD addr;
  DWORD off,ind,split,split_ind;
  DWORD module,function;
  DWORD extern_fun,type;
  WORD length_offset;
  BYTE hook_len,recover_len;
};

struct SendParam
{
  DWORD type;
  HookParam hp;
};

///  Readonly
class Hook // 0x80
{
  HookParam hp;
  LPWSTR hook_name;
  int name_length;
  BYTE recover[0x68 - sizeof(HookParam)];
  BYTE original[0x10];

public:
  DWORD Address() const { return hp.addr; }
  DWORD Type() const    { return hp.type; }
  WORD Length() const   { return hp.hook_len; }
  LPWSTR Name() const   { return hook_name; }
  int NameLength() const{ return name_length; }
};

//extern HANDLE hHeap;

// jichi: 10/15/2011: FIXME: This overload will infect the entire program,
// even source files that exclude this header, which is unexpected.
// No idea what is the trade off of this behavior on performance and liability.
// Lots of Qt stuff doesn't work such as QString::toStdString.
// I have to use dynamic linkage to avoid being polluted by this module.
//
// original author: HEAP_ZERO_MEMORY flag is critical. All new object are assumed with zero initialized.
// jichi: 10/20/2011: I think the only reason to use Rtl heap here is to ensure HEAP_ZERO_MEMORY,
// which is really a bad programming style and incur unstability on heap memory allocation.
// ::RtlFreeHeap crash on DLL debug mode. Replace it with standard malloc/free.
// ::hHeap handle is also removed from ith/sys.c.cc
/*
inline void * __cdecl operator new(size_t lSize)
{ return ::RtlAllocateHeap(::hHeap, HEAP_ZERO_MEMORY, lSize); }

inline void * __cdecl operator new[](size_t lSize)
{ return ::RtlAllocateHeap(::hHeap, HEAP_ZERO_MEMORY, lSize); }

inline void __cdecl operator delete(void *pBlock)
{ ::RtlFreeHeap(::hHeap, 0, pBlock); }

inline void __cdecl operator delete[](void* pBlock)
{ ::RtlFreeHeap(::hHeap, 0, pBlock); }
*/
#include <cstdlib>
inline void * __cdecl operator new(size_t size) throw()
{
  if (!size)    // When the value of the expression in a direct-new-declarator is zero,
    size = 4;   // the allocation function is called to allocatean array with no elements.(ISO)

  void *p = malloc(size);
  if (p)
    memset(p, 0, size);
  return p;
}

inline void * __cdecl operator new[](size_t size) throw()
{
  if (!size)    // When the value of the expression in a direct-new-declarator is zero,
    size = 4;   // the allocation function is called to allocatean array with no elements.(ISO)

  void *p = malloc(size);
  if (p)
    memset(p, 0, size);
  return p;
}

inline void __cdecl operator delete(void *p) throw() { free(p); }

inline void __cdecl operator delete[](void *p) throw() { free(p); }

// EOF
