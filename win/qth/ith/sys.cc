// ith/sys.c
// 10/14/2011
// TODO: clean up this file

// TODO: 10/15/2011: clean-up this file

#include "ntdll.h"
#include "ith/sys.h"
#include <qt_windows.h>

//#define DEBUG "ith:sys"
#include "module/debug/debug.h"

// - Global variables -
// jichi: 10/15/2011: Avoid renaming these variables to ease merging future ITH updates

WCHAR file_path[MAX_PATH] = L"\\??\\"; // export to inject.cc, MAX_PATH defined in ntdll.h
LPWSTR current_dir; // export to inject.cc

DWORD current_process_id;
DWORD nt_flag;

LPVOID page;

//HANDLE hHeap;
HANDLE root_obj;
HANDLE codepage_section, thread_man_section;
HANDLE thread_man_mutex;

//BYTE launch_time[0x10];

BYTE LeadByteTable[0x100]={
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1
};


//#define SEC_BASED 0x200000

// - Local static members -

namespace { // anonymous, variables

  BYTE file_info[0x1000];

} // anonymous namespace

namespace { // anonymous, functions

  inline DWORD GetShareMemory()
  { __asm {
    mov eax,fs:[0x30]
    mov eax,[eax+0x4C]
  } }

  inline LARGE_INTEGER* GetTimeBias()
  { __asm mov eax,0x7ffe0020 }

  __declspec(naked)
  void ThreadStart()
  { __asm{
    nop
    call eax
    push eax
    push -2
    call dword ptr [NtTerminateThread]
  } }

  inline DWORD GetHash(LPSTR str)
  {
    DWORD hash = 0;
    for (;*str;str++)
      hash = ((hash>>7)|(hash<<25)) + (*str);
    return hash;
  }

  inline DWORD GetHash(LPWSTR str)
  {
    DWORD hash = 0;
    for (;*str;str++)
      hash = ((hash>>7)|(hash<<25)) + (*str);
    return hash;
  }

} // anonymous namespace

namespace { // anonymous, ThreadStartManager singleton

  class ThreadStartManager
  {
    UINT_PTR count_;
    DWORD proc_record_[1];

  public:
    LPVOID
    GetProcAddr(HANDLE hProc)
    {
      DWORD pid, addr, len;
      if (hProc == ::NtCurrentProcess())
        pid = ::current_process_id;
      else {
        PROCESS_BASIC_INFORMATION info;
        ::NtQueryInformationProcess(hProc, ProcessBasicInformation, &info, sizeof(info), &len);
        pid = info.uUniqueProcessId;
      }
      pid >>= 2;
      for (UINT_PTR i=0; i < count_; i++) {
        if ((proc_record_[i]&0xFFF)==pid) {
          addr = proc_record_[i] & ~0xFFF;
          return (LPVOID)addr;
        }
      }
      len = 0x1000;
      ::NtAllocateVirtualMemory(hProc, (PVOID*)(proc_record_ + count_), 0, &len, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
      addr = proc_record_[count_];
      proc_record_[count_] |= pid;
      BYTE buffer[0x10];

      ::memcpy(buffer, ThreadStart, 0x8);
      *((DWORD*)buffer+3) = (DWORD)::NtTerminateThread;
      *((DWORD*)buffer+2) = (DWORD)addr + 0xC;
      ::NtWriteVirtualMemory(hProc,(PVOID)addr,buffer,0x10,&len);
      count_++;
      return (LPVOID)addr;
    }

    void
    ReleaseProcessMemory(HANDLE hProc)
    {
      DWORD pid,addr,len;
      if (hProc == ::NtCurrentProcess())
        pid = ::current_process_id;
      else {
        PROCESS_BASIC_INFORMATION info;
        ::NtQueryInformationProcess(hProc,ProcessBasicInformation,&info,sizeof(info),&len);
        pid = info.uUniqueProcessId;
      }
      pid >>= 2;
      ::NtWaitForSingleObject(thread_man_mutex,0,0);
      for (UINT_PTR i=0;i<count_;i++) {
        if ((proc_record_[i] & 0xFFF) == pid) {
          addr = proc_record_[i]&~0xFFF;
          DWORD size = 0x1000;
          ::NtFreeVirtualMemory(hProc,(PVOID*)&addr, &size, MEM_RELEASE);
          count_--;
          for (UINT_PTR j = i; j<count_; j++)
            proc_record_[j] = proc_record_[j+1];
          proc_record_[count_] = 0;
          ::NtReleaseMutant(thread_man_mutex,0);
          return;
        }
      }
      ::NtReleaseMutant(thread_man_mutex,0);
    }

    void
    CheckProcessMemory()
    {
      DWORD len;
      CLIENT_ID id;
      OBJECT_ATTRIBUTES oa={0};
      HANDLE hProc;
      BYTE buffer[8];
      id.UniqueThread=0;
      oa.uLength=sizeof(oa);
      for (UINT_PTR i = 0; i<count_; i++) {
        id.UniqueProcess=(proc_record_[i]&0xFFF)<<2;
        UINT_PTR addr = proc_record_[i]&~0xFFF;
        UINT_PTR flag = 0;
        if (NT_SUCCESS(::NtOpenProcess(&hProc, PROCESS_VM_OPERATION|PROCESS_VM_READ, &oa, &id))) {
          if (NT_SUCCESS(::NtReadVirtualMemory(hProc, (PVOID)addr, buffer, 8, &len))) {
            if (::memcmp(buffer, ThreadStart, 8) == 0)
              flag = 1;
          }
          ::NtClose(hProc);
        }
        if (flag == 0)
          for (UINT_PTR j = i; j < count_; j++, i--, count_--)
            proc_record_[j] = proc_record_[j + 1];
      }
    }
  };

  ThreadStartManager *thread_man = 0; // Initialized later in ::IthInitSystemService

} // anonymous namespace

// - Global functions -

extern "C" {

int FillRange(LPWSTR name,DWORD* lower, DWORD* upper)
{
  PLDR_DATA_TABLE_ENTRY it;
  LIST_ENTRY *begin;
  __asm
  {
    mov eax,fs:[0x30]
    mov eax,[eax+0xC]
    mov eax,[eax+0xC]
    mov it,eax
    mov begin,eax
  }
  while (it->SizeOfImage)
  {
    if (_wcsicmp(it->BaseDllName.Buffer,name)==0)
    {
      *lower=(DWORD)it->DllBase;
      *upper=*lower;
      MEMORY_BASIC_INFORMATION info={0};
      DWORD l,size;
      size=0;
      do
      {
        NtQueryVirtualMemory(::NtCurrentProcess(),(LPVOID)(*upper),MemoryBasicInformation,&info,sizeof(info),&l);
        if (info.Protect&PAGE_NOACCESS)
        {
          it->SizeOfImage=size;
          break;
        }
        size+=info.RegionSize;
        *upper+=info.RegionSize;
      }while (size<it->SizeOfImage);
      return 1;
    }
    it=(PLDR_DATA_TABLE_ENTRY)it->InLoadOrderModuleList.Flink;
    if (it->InLoadOrderModuleList.Flink==begin) break;
  }
  return 0;
}
DWORD SearchPattern(DWORD base, DWORD base_length, LPVOID search, DWORD search_length) //KMP
{
  __asm
  {
    mov eax,search_length
alloc:
    push 0
    sub eax,1
    jnz alloc

    mov edi,search
    mov edx,search_length
    mov ecx,1
    xor esi,esi
build_table:
    mov al,byte ptr [edi+esi]
    cmp al,byte ptr [edi+ecx]
    sete al
    test esi,esi
    jz pre
    test al,al
    jnz pre
    mov esi,[esp+esi*4-4]
    jmp build_table
pre:
    test al,al
    jz write_table
    inc esi
write_table:
    mov [esp+ecx*4],esi

    inc ecx
    cmp ecx,edx
    jb build_table

    mov esi,base
    xor edx,edx
    mov ecx,edx
matcher:
    mov al,byte ptr [edi+ecx]
    cmp al,byte ptr [esi+edx]
    sete al
    test ecx,ecx
    jz match
    test al,al
    jnz match
    mov ecx, [esp+ecx*4-4]
    jmp matcher
match:
    test al,al
    jz pre2
    inc ecx
    cmp ecx,search_length
    je finish
pre2:
    inc edx
    cmp edx,base_length //search_length
    jb matcher
    mov edx,search_length
    dec edx
finish:
    mov ecx,search_length
    sub edx,ecx
    lea eax,[edx+1]
    lea ecx,[ecx*4]
    add esp,ecx
  }
}
DWORD IthGetMemoryRange(LPVOID mem, DWORD* base, DWORD* size)
{
  DWORD r;
  MEMORY_BASIC_INFORMATION info;
  NtQueryVirtualMemory(::NtCurrentProcess(),mem,MemoryBasicInformation,&info,sizeof(info),&r);
  if (base) *base=(DWORD)info.BaseAddress;
  if (size) *size=info.RegionSize;
  return (info.Type&PAGE_NOACCESS)==0;
}
//Get full path of current process.
LPWSTR GetModulePath()
{
  __asm
  {
    mov eax,fs:[0x30]
    mov eax,[eax+0xC]
    mov eax,[eax+0xC]
    mov eax,[eax+0x28]
  }
}
//SJIS->Unicode. 'mb' must be null-terminated. 'wc' should have enough space ( 2*strlen(mb) is safe).
int MB_WC(char* mb, wchar_t* wc)
{
  __asm
  {
    mov esi,mb
    mov edi,wc
    mov edx,page
    lea ebx,LeadByteTable
    add edx,0x220
    push 0
_mb_translate:
    movzx eax,word ptr [esi]
    test al,al
    jz _mb_fin
    movzx ecx,al
    xlat
    test al,1
    cmovnz cx, word ptr [ecx*2+edx-0x204]
    jnz _mb_next
    mov cx,word ptr [ecx*2+edx]
    mov cl,ah
    mov cx, word ptr [ecx*2+edx]
_mb_next:
    mov [edi],cx
    add edi,2
    movzx eax,al
    add esi,eax
    inc dword ptr [esp]
    jmp _mb_translate
_mb_fin:
    pop eax
  }
}

//Count characters of 'mb' string. 'mb_length' is max length.
int MB_WC_count(char* mb, int mb_length)
{
  __asm
  {
    xor eax,eax
    xor edx,edx
    mov esi,mb
    mov edi,mb_length
    lea ebx,LeadByteTable
_mbc_count:
    mov dl,byte ptr [esi]
    movzx ecx, byte ptr [ebx+edx]
    add esi,ecx
    inc eax
    sub edi,ecx
    ja _mbc_count
  }
}

//Unicode->SJIS. Analogous to MB_WC.
int WC_MB(wchar_t *wc, char* mb)
{
  __asm
  {
    mov esi,wc
    mov edi,mb
    mov edx,page
    add edx,0x7C22
    xor ebx,ebx
_wc_translate:
    movzx eax,word ptr [esi]
    test eax,eax
    jz _wc_fin
    mov cx,word ptr [eax*2+edx]
    test ch,ch
    jz _wc_single
    mov [edi+ebx],ch
    inc ebx
_wc_single:
    mov [edi+ebx],cl
    inc ebx
    add esi,2
    jmp _wc_translate
_wc_fin:
    mov eax,ebx
  }
}

void
FreeThreadStart(HANDLE hProc)
{ ::thread_man->ReleaseProcessMemory(hProc); }

void
CheckThreadStart()
{ ::thread_man->CheckProcessMemory(); }

//Initialize environment for NT native calls.
//1. Create new heap. Successive memory requests are handled buy this heap.
//Destroying this heap will completely release all dynamic allocated memory, thus prevent memory leaks on unload.
//2. Create handle to root directory of process objects (section/event/mutex/semaphore).
//NtCreate* calls will use this handle as base directory.
//3. Load SJIS code page. First check for Japanese locale. If not then load from 'C_932.nls' in system folder.
//MB_WC & WC_MB use this code page for translation.
//4. Locate current NT path (start with \??\).
//NtCreateFile requires full path or a root handle. But this handle is different from object.
//5. Map shared memory for ThreadStartManager into virtual address space.
//This will allow IthCreateThread function properly.
void
IthInitSystemService()
{
  //ULONG LowFragmentHeap;
  LPWSTR t,obj;
  UNICODE_STRING us;
  DWORD mem,size;
  OBJECT_ATTRIBUTES oa={sizeof(oa),0,&us,OBJ_CASE_INSENSITIVE,0,0};
  IO_STATUS_BLOCK ios;
  HANDLE codepage_file;
  LARGE_INTEGER sec_size={0x1000,0};
  __asm {
    mov eax,fs:[0x18]
    mov ecx,[eax+0x20]
    mov eax,[eax+0x30]
    mov eax,[eax]
    mov current_process_id,ecx
    mov nt_flag,eax
  }
  //LowFragmentHeap=2;
  //hHeap=RtlCreateHeap(0x1002,0,0,0,0,0);
  //RtlSetHeapInformation(hHeap,HeapCompatibilityInformation,&LowFragmentHeap,sizeof(LowFragmentHeap));
  mem=GetShareMemory();
  IthGetMemoryRange((LPVOID)mem,0,&size);
  t=(LPWSTR)(mem+SearchPattern(mem,size,L"system32",0x10));
  for (obj=t;*obj!=L'\\';obj++);
  RtlInitUnicodeString(&us,obj);
  NtOpenDirectoryObject(&root_obj,READ_CONTROL|0xF,&oa);
  if (*NlsAnsiCodePage==0x3A4)
  {
    __asm
    {
      mov eax,fs:[0x30]
      mov eax,[eax+0x58]
      mov page,eax
    }
    oa.hRootDirectory=root_obj;
    oa.uAttributes|=OBJ_OPENIF;
  }
  else
  {
    while (*t--!=L':');
    wcscpy(file_path+4,t);
    t=file_path;
    while(*++t);
    if (*(t-1)!=L'\\') *t++=L'\\';
    wcscpy(t,L"C_932.nls");
    RtlInitUnicodeString(&us,file_path);
    NtOpenFile(&codepage_file,FILE_READ_DATA,&oa,&ios,FILE_SHARE_READ,0);
    oa.hRootDirectory=root_obj;
    oa.uAttributes|=OBJ_OPENIF;
    RtlInitUnicodeString(&us,L"JPN_CodePage");
    NtCreateSection(&codepage_section,SECTION_MAP_READ,&oa,0,PAGE_READONLY,SEC_COMMIT,codepage_file);
    NtClose(codepage_file);
    size=0;
    NtMapViewOfSection(codepage_section,::NtCurrentProcess(),&page,0,0,0,&size,ViewUnmap,0,PAGE_READONLY);
  }

  wcscpy(file_path+4,GetModulePath());
  current_dir = wcsrchr(file_path, L'\\')+1;

  RtlInitUnicodeString(&us,L"ITH_SysSection");
  NtCreateSection(&thread_man_section,SECTION_ALL_ACCESS,&oa,&sec_size,
    PAGE_EXECUTE_READWRITE,SEC_COMMIT,0);
  size=0;
  NtMapViewOfSection(thread_man_section,::NtCurrentProcess(),
    (PVOID*)&thread_man,0,0,0,&size,ViewUnmap,0,PAGE_EXECUTE_READWRITE);
  thread_man_mutex=IthCreateMutex(L"ITH_ThreadMan",0);

}

//Release resources allocated by IthInitSystemService.
//After destroying the heap, all memory allocated by ITH module is returned to system.
void
IthCloseSystemService()
{
  if (*NlsAnsiCodePage!=0x3A4)
  {
    NtUnmapViewOfSection(::NtCurrentProcess(),page);
    NtClose(codepage_section);
  }
  NtUnmapViewOfSection(::NtCurrentProcess(),thread_man);
  //RtlDestroyHeap(hHeap);
  NtClose(root_obj);
  NtClose(thread_man_mutex);
  NtClose(thread_man_section);

}
/*
//Check for existence of a file in current folder.
//For ITH main module, it's ITH folder. For target process it's the target process's current folder.
BOOL IthCheckFile(LPWSTR file)
{
  //return IthGetFileInfo(file,file_info);
  wcscpy(current_dir,file);
  return IthCheckFileFullPath(file_path);
}
//Check for existence of files in current folder.
//Unlike IthCheckFile, this function allows wildcard character.
BOOL IthFindFile(LPWSTR file)
{
  NTSTATUS status;
  LPWSTR path=wcsrchr(file,L'\\');
  if (path)
  {
    memcpy(current_dir,file,(path-file)<<1);
    current_dir[path-file]=0;
    file=path+1;
  }
  else current_dir[0]=0;
  HANDLE h;
  UNICODE_STRING us;
  RtlInitUnicodeString(&us,file_path);
  OBJECT_ATTRIBUTES oa={sizeof(oa),0,&us,OBJ_CASE_INSENSITIVE,0,0};
  IO_STATUS_BLOCK ios;
  if (NT_SUCCESS(NtOpenFile(&h,FILE_LIST_DIRECTORY|SYNCHRONIZE,
    &oa,&ios,FILE_SHARE_READ,FILE_DIRECTORY_FILE|FILE_SYNCHRONOUS_IO_NONALERT)))
  {
    RtlInitUnicodeString(&us,file);
    status=NtQueryDirectoryFile(h,0,0,0,&ios,file_info,0x400,FileBothDirectoryInformation,TRUE,&us,0);
    NtClose(h);
    return NT_SUCCESS(status);
  }
  return FALSE;
}
//Analogous to IthFindFile, but return detail information in 'info'.
BOOL IthGetFileInfo(LPWSTR file, LPVOID info)
{
  NTSTATUS status;
  LPWSTR path=wcsrchr(file,L'\\');
  if (path)
  {
    memcpy(current_dir,file,(path-file)<<1);
    current_dir[path-file]=0;
    file=path+1;
  }
  else current_dir[0]=0;
  HANDLE h;
  UNICODE_STRING us;
  RtlInitUnicodeString(&us,file_path);
  OBJECT_ATTRIBUTES oa={sizeof(oa),0,&us,OBJ_CASE_INSENSITIVE,0,0};
  IO_STATUS_BLOCK ios;
  if (NT_SUCCESS(NtOpenFile(&h,FILE_LIST_DIRECTORY|SYNCHRONIZE,
    &oa,&ios,FILE_SHARE_READ,FILE_DIRECTORY_FILE|FILE_SYNCHRONOUS_IO_NONALERT)))
  {
    RtlInitUnicodeString(&us,file);
    status=NtQueryDirectoryFile(h,0,0,0,&ios,info,0x1000,FileBothDirectoryInformation,0,&us,0);
    status=NT_SUCCESS(status);
  }
  else
    status=FALSE;
  NtClose(h);
  return status;
}
//Check for existence of a file with full NT path(start with \??\).
BOOL IthCheckFileFullPath(LPWSTR file)
{
  UNICODE_STRING us;
  RtlInitUnicodeString(&us,file);
  OBJECT_ATTRIBUTES oa={sizeof(oa),0,&us,OBJ_CASE_INSENSITIVE,0,0};
  HANDLE hFile;
  IO_STATUS_BLOCK isb;
  if (NT_SUCCESS(NtCreateFile(&hFile,FILE_READ_DATA,&oa,&isb,0,0,FILE_SHARE_READ,FILE_OPEN,0,0,0)))
  {
    NtClose(hFile);
    return TRUE;
  }
  else return FALSE;
}
//Create or open file in current folder. Analogous to Win32 CreateFile.
//option: GENERIC_READ / GENERIC_WRITE.
//share: FILE_SHARE_READ / FILE_SHARE_WRITE / FILE_SHARE_DELETE. 0 for exclusive access.
//disposition: FILE_OPEN / FILE_OPEN_IF.
//Use FILE_OPEN instead of OPEN_EXISTING and FILE_OPEN_IF for CREATE_ALWAYS.
HANDLE IthCreateFile(LPWSTR name, DWORD option, DWORD share, DWORD disposition)
{
  wcscpy(current_dir,name);
  UNICODE_STRING us;
  RtlInitUnicodeString(&us,file_path);
  OBJECT_ATTRIBUTES oa={sizeof(oa),0,&us,OBJ_CASE_INSENSITIVE,0,0};
  HANDLE hFile;
  IO_STATUS_BLOCK isb;
  if (NT_SUCCESS(NtCreateFile(&hFile,
    option|FILE_READ_ATTRIBUTES|SYNCHRONIZE
    ,&oa,&isb,0,0,share,disposition,
    FILE_SYNCHRONOUS_IO_NONALERT|FILE_NON_DIRECTORY_FILE,0,0)))
    return hFile;
  else return INVALID_HANDLE_VALUE;
}

//Create a directory file in current folder.
HANDLE IthCreateDirectory(LPWSTR name)
{
  wcscpy(current_dir,name);
  UNICODE_STRING us;
  RtlInitUnicodeString(&us,file_path);
  OBJECT_ATTRIBUTES oa={sizeof(oa),0,&us,OBJ_CASE_INSENSITIVE,0,0};
  HANDLE hFile;
  IO_STATUS_BLOCK isb;
  if (NT_SUCCESS(NtCreateFile(&hFile,STANDARD_RIGHTS_REQUIRED,&oa,&isb,0,0,
    FILE_SHARE_READ|FILE_SHARE_WRITE,
    FILE_OPEN_IF,FILE_DIRECTORY_FILE,0,0)))
    return hFile;
  else return INVALID_HANDLE_VALUE;
}

//Analogous to IthCreateFile, but with full NT path.
HANDLE IthCreateFileFullPath(LPWSTR full_path, DWORD option, DWORD share, DWORD disposition)
{
  WCHAR path[MAX_PATH]=L"\\??\\";
  wcscpy(path+4,full_path);
  UNICODE_STRING us;
  RtlInitUnicodeString(&us,path);
  OBJECT_ATTRIBUTES oa={sizeof(oa),0,&us,OBJ_CASE_INSENSITIVE,0,0};
  HANDLE hFile;
  IO_STATUS_BLOCK isb;
  if (NT_SUCCESS(NtCreateFile(&hFile,
    option|FILE_READ_ATTRIBUTES|SYNCHRONIZE
    ,&oa,&isb,0,0,share,disposition,
    FILE_SYNCHRONOUS_IO_NONALERT|FILE_NON_DIRECTORY_FILE,0,0)))
    return hFile;
  else return INVALID_HANDLE_VALUE;
}

//Prompt for file name.
HANDLE IthPromptCreateFile(DWORD option, DWORD share, DWORD disposition)
{
  OPENFILENAME ofn={sizeof(ofn)};       // common dialog box structure
  WCHAR szFile[MAX_PATH];       // buffer for file name
  wcscpy(current_dir,L"ITH_export.txt");
  wcscpy(szFile,file_path+4);

  //szFile[0]=0;
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrFilter = L"Text\0*.txt";
  BOOL result;
  if (disposition==FILE_OPEN)
    result=GetOpenFileName(&ofn);
  else
    result=GetSaveFileName(&ofn);
  if (result)
  {
    LPWSTR s=szFile+wcslen(szFile)-4;
    if (_wcsicmp(s,L".txt")!=0) wcscpy(s+4,L".txt");
    return IthCreateFileFullPath(szFile,option,share,disposition);
  }
  else return INVALID_HANDLE_VALUE;
}
*/

//Create section object for sharing memory between processes.
//Similar to CreateFileMapping.
HANDLE IthCreateSection(LPWSTR name, DWORD size, DWORD right)
{
  HANDLE hSection;
  LARGE_INTEGER s={size,0};
  if (name)
  {

    UNICODE_STRING us;
    RtlInitUnicodeString(&us,name);
    OBJECT_ATTRIBUTES oa={sizeof(oa),root_obj,&us,OBJ_OPENIF,0,0};
    if (NT_SUCCESS(NtCreateSection(&hSection,GENERIC_ALL,&oa,&s,
      right,SEC_COMMIT,0)))
      return hSection;
    else return INVALID_HANDLE_VALUE;
  }
  else
  {
    if (NT_SUCCESS(NtCreateSection(&hSection,GENERIC_ALL,0,&s,
      right,SEC_COMMIT,0)))
      return hSection;
    else return INVALID_HANDLE_VALUE;
  }
}
//Create event object. Similar to CreateEvent.
HANDLE IthCreateEvent(LPWSTR name, DWORD auto_reset, DWORD init_state)
{
  HANDLE hEvent;
  if (name)
  {
    UNICODE_STRING us;
    RtlInitUnicodeString(&us,name);
    OBJECT_ATTRIBUTES oa={sizeof(oa),root_obj,&us,OBJ_OPENIF,0,0};
    if (NT_SUCCESS(NtCreateEvent(&hEvent,EVENT_ALL_ACCESS,&oa,auto_reset,init_state)))
      return hEvent;
  }
  else if (NT_SUCCESS(NtCreateEvent(&hEvent,EVENT_ALL_ACCESS,0,auto_reset,init_state)))
      return hEvent;
  return INVALID_HANDLE_VALUE;
}
HANDLE IthOpenEvent(LPWSTR name)
{
  UNICODE_STRING us;
  RtlInitUnicodeString(&us,name);
  OBJECT_ATTRIBUTES oa={sizeof(oa),root_obj,&us,0,0,0};
  HANDLE hEvent;
  if (NT_SUCCESS(NtOpenEvent(&hEvent,EVENT_ALL_ACCESS,&oa)))
    return hEvent;
  else return INVALID_HANDLE_VALUE;
}
void IthSetEvent(HANDLE hEvent)
{
  NtSetEvent(hEvent,0);
}
void IthResetEvent(HANDLE hEvent)
{
  NtClearEvent(hEvent);
}
//Create mutex object. Similar to CreateMutex.
//If 'exist' is not null, it will be written 1 if mutex exist.
HANDLE IthCreateMutex(LPWSTR name, BOOL InitialOwner, DWORD* exist)
{
  UNICODE_STRING us;
  HANDLE hMutex; NTSTATUS status;
  if (name)
  {
    RtlInitUnicodeString(&us,name);
    OBJECT_ATTRIBUTES oa={sizeof(oa),root_obj,&us,OBJ_OPENIF,0,0};
    status=NtCreateMutant(&hMutex,MUTEX_ALL_ACCESS,&oa,InitialOwner);
  }
  else status=NtCreateMutant(&hMutex,MUTEX_ALL_ACCESS,0,InitialOwner);
  if (NT_SUCCESS(status))
  {
    if (exist) *exist=(STATUS_OBJECT_NAME_EXISTS==status);
    return hMutex;
  }
  else
    return INVALID_HANDLE_VALUE;
}

HANDLE
IthOpenMutex(LPWSTR name)
{
  UNICODE_STRING us;
  ::RtlInitUnicodeString(&us,name);
  OBJECT_ATTRIBUTES oa = { sizeof(oa), root_obj, &us, 0, 0, 0 };
  HANDLE hMutex;
  if (NT_SUCCESS(::NtOpenMutant(&hMutex, MUTEX_ALL_ACCESS, &oa)))
    return hMutex;
  else
    return INVALID_HANDLE_VALUE;
}

BOOL
IthReleaseMutex(HANDLE hMutex)
{ return NT_SUCCESS(::NtReleaseMutant(hMutex, 0)); }

DWORD
IthWaitForSingleObject(HANDLE hObject, DWORD dwTime)
{ __asm{
  sub esp,0x8
  xor ecx,ecx
  cmp dwTime,-1
  cmove eax,ecx
  je _wait
  mov eax,0x2710
  mov ecx,dwTime
  mul ecx
  neg eax
  adc edx,0
  neg edx
  mov [esp],eax
  mov [esp+4],edx
  mov eax,esp
_wait:
  push eax
  push 0
  push hObject
  call dword ptr [NtWaitForSingleObject]
  add esp,0x8
} }

HANDLE
IthCreateThread(LPVOID lpStartAddress, DWORD dwParam, HANDLE hProc)
{
  DOUT("enter");
  HANDLE hThread; // return value

//#if _WIN32_WINNT == _WIN32_WINNT_WINXP
#if 1 // might be slower on Windows 7, but support Windows XP
  // NtCreateThread with non-admin previlege only works for Windows XP SP3
  // Use CreateRemoteThread for Windows XP SP1-SP2 instead.
  //
  // \param hProc must have following rights:
  // - PROCESS_CREATE_THREAD, PROCESS_VM_OPERATION, PROCESS_VM_READ, PROCESS_VM_WRITE.
  enum { DEFAULT_STACK_LIMIT = 0x400000 };
  enum { DEFAULT_STACK_COMMIT = 0x10000 };
  enum { PAGE_SIZE = 0x1000 };

  // Construct stack
  DWORD dwPageSize = PAGE_SIZE;
  DWORD dwStackSize = DEFAULT_STACK_LIMIT;
  DWORD dwCommitSize = DEFAULT_STACK_COMMIT;
  USER_STACK stack = { }; {
    ::NtAllocateVirtualMemory(
      hProc,          // __in HANDLE ProcessHandle,
      &stack.ExpandableStackBottom, // __inout PVOID *BaseAddress,
      0,              // __in ULONG ZeroBits
      &dwStackSize,   // __inout PULONG AllocationSize,
      MEM_RESERVE,    // __in ULONG AllocationType,
      PAGE_READWRITE  // __in ULONG Protect
    );

    stack.ExpandableStackBase = (char*)stack.ExpandableStackBottom + dwStackSize;
    stack.ExpandableStackLimit = (char*)stack.ExpandableStackBase - dwCommitSize;
    dwCommitSize += dwPageSize;

    LPVOID lpProtect = (char*)stack.ExpandableStackBase - dwCommitSize;
    ::NtAllocateVirtualMemory(
      hProc,            // __in HANDLE ProcessHandle,
      &lpProtect,         // __inout PVOID *BaseAddress,
      0,                // __in ULONG ZeroBits
      &dwCommitSize,    // __inout PULONG AllocationSize
      MEM_COMMIT,       // __in ULONG AllocationType
      PAGE_READWRITE    // __in ULONG Protect
    );

    DWORD dwDummy;
    ::NtProtectVirtualMemory(
      hProc,      // __in HANDLE ProcessHandle,
      &lpProtect, // __inout PVOID *BaseAddress,
      &dwPageSize,// __inout PULONG ProtectSize,
      PAGE_READWRITE | PAGE_GUARD, // __in ULONG NewProtect,
      &dwDummy    // __out PULONG OldProtect
    );
  }

  // Construct context
  CONTEXT ctx = { CONTEXT_FULL }; {
    ctx.SegGs = 0x0;
    ctx.SegFs = 0x38;
    ctx.SegEs = 0x20;
    ctx.SegDs = 0x20;
    ctx.SegSs = 0x20;
    ctx.SegCs = 0x18;
    ctx.EFlags= 0x3000;
    ctx.Eax = (DWORD)lpStartAddress;
    ctx.Esp = (DWORD)stack.ExpandableStackBase - 0x10;

    // __in HANDLE hObject, __in BOOL fAlertable, __in PLARGE_INTEGER pliTimeout (NULL = infinite)
    ::NtWaitForSingleObject(::thread_man_mutex, 0, 0);
    //Q_ASSERT(thread_man);
    ctx.Eip = (DWORD)::thread_man->GetProcAddr(hProc);
    ::NtReleaseMutant(::thread_man_mutex, 0); // __in HANDLE hMutex, __out PULONG Optional
  }

  CLIENT_ID cid;
  NTSTATUS status = ::NtCreateThread(
    &hThread,   // __out PHANDLE ThreadHandle,
    THREAD_ALL_ACCESS, // __in ACCESS_MASK DesiredAccess,
    0,          // __in POBJECT_ATTRIBUTES ObjectAttributes,
    hProc,      // __in HANDLE ProcessHandle,
    &cid,       // __out PCLIENT_ID ClientId,
    &ctx,       // __in PCONTEXT ThreadContext,
    &stack,     // __in PUSER_STACK UserStack,
    1           // __in BOOLEAN CreateSuspended, if true, NtResumeThread is required
  );

  if (NT_SUCCESS(status)) {
    ::NtGetContextThread(hThread, &ctx);

    ::NtWriteVirtualMemory(
      hProc,            // __in HANDLE ProcessHandle,
      (LPVOID)ctx.Esp,  // __in PVOID BaseAddress,
      &dwParam,         // __in PVOID Buffer,
      sizeof(DWORD),    // __in ULONG BufferLength, size(DWORD) == 4
      0                 // __out PULONG ReturnLength OPTIONAL
    );

    // (__in HANDLE ThreadHandle, __out PULONG PreviousSuspendCount OPTIONAL)
    status = ::NtResumeThread(hThread, 0);
  }

#else // Windows Vista/7 only
  // NtCreateThreadEx only works for Windows Vista/7
  // See: http://stackoverflow.com/questions/4826408/failing-dll-injection

  DWORD dwStackCommitSize = 0;//DEFAULT_STACK_COMMIT;
  DWORD dwStackReserveSize = 0;//DEFAULT_STACK_LIMIT;
  NtCreateThreadExBuffer ntbuffer; {
    ntbuffer.Size = sizeof(NtCreateThreadExBuffer);
    ntbuffer.Unknown1 = 0x10003;
    ntbuffer.Unknown2 = 0x8;
    ntbuffer.Unknown3 = &dwStackCommitSize;
    ntbuffer.Unknown4 = 0;
    ntbuffer.Unknown5 = 0x10004;
    ntbuffer.Unknown6 = 4;
    ntbuffer.Unknown7 = &dwStackReserveSize;
    ntbuffer.Unknown8 = 0;
  }

  NTSTATUS status = ::NtCreateThreadEx(
    &hThread,
    THREAD_ALL_ACCESS,
    0, // Attributes
    hProc,      // process handle
    (LPTHREAD_START_ROUTINE)lpStartAddress, // thread address
    (LPVOID)dwParam,                    // thread param
    FALSE, // if suspend after creation
    0x0,  // stack initial bits
    dwStackCommitSize,
    dwStackReserveSize,
    &ntbuffer
  );
#endif // WINVER

  if (NT_SUCCESS(status)) {
    DOUT("exit: ret =" << hThread);
    return hThread;
  } else {
    DOUT("exit: WARNING: failed to create native thread, addr =" << lpStartAddress);
    return INVALID_HANDLE_VALUE;
  }
}

//Query module export table. Return function address if find.
//Similar to GetProcAddress
DWORD
GetExportAddress(DWORD hModule,DWORD hash)
{
  IMAGE_DOS_HEADER *DosHdr;
  IMAGE_NT_HEADERS *NtHdr;
  IMAGE_EXPORT_DIRECTORY *ExtDir;
  UINT uj;
  char* pcExportAddr,*pcFuncPtr,*pcBuffer;
  DWORD dwReadAddr,dwFuncAddr,dwFuncName;
  WORD wOrd;

  DosHdr = (IMAGE_DOS_HEADER*)hModule;
  if (IMAGE_DOS_SIGNATURE == DosHdr->e_magic) {
    dwReadAddr = hModule+DosHdr->e_lfanew;
    NtHdr = (IMAGE_NT_HEADERS*)dwReadAddr;
    if (IMAGE_NT_SIGNATURE == NtHdr->Signature) {
      pcExportAddr = (char*)((DWORD)hModule+
        (DWORD)NtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
      if (pcExportAddr == 0)
        return 0;
      ExtDir = (IMAGE_EXPORT_DIRECTORY*)pcExportAddr;
      pcExportAddr = (char*)((DWORD)hModule + (DWORD)ExtDir->AddressOfNames);

      for (uj = 0; uj < ExtDir->NumberOfNames; uj++) {
        dwFuncName = *(DWORD*)pcExportAddr;
        pcBuffer = (char*)((DWORD)hModule + dwFuncName);
        if (GetHash(pcBuffer) == hash) {
          pcFuncPtr = (char*)((DWORD)hModule + (DWORD)ExtDir->AddressOfNameOrdinals + (uj*sizeof(WORD)));
          wOrd = *(WORD*)pcFuncPtr;
          pcFuncPtr = (char*)((DWORD)hModule + (DWORD)ExtDir->AddressOfFunctions + (wOrd*sizeof(DWORD)));
          dwFuncAddr = *(DWORD*)pcFuncPtr;
          return hModule + dwFuncAddr;
        }
        pcExportAddr += sizeof(DWORD);
      }
    }
  }
  return 0;
}

void
IthSleep(int time)
{ __asm {
  mov eax,0x2710
  mov ecx,time
  mul ecx
  neg eax
  adc edx,0
  neg edx
  push edx
  push eax
  push esp
  push 0
  call dword ptr [NtDelayExecution]
  add esp,8
} }

void
IthSystemTimeToLocalTime(LARGE_INTEGER *time)
{ time->QuadPart -= GetTimeBias()->QuadPart; }

} // extern "C"

// EOF
