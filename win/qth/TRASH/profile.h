#pragma once
/*
#include "main.h"
#include "AVL.h"
#define THREAD_MASK_RETN 1
#define THREAD_MASK_SPLIT 2
struct ThreadParam
{
  WORD hook_index,status;
  DWORD hook_addr;
  DWORD retn;
  DWORD split;
  WORD hm_index,reserve;
};
struct LinkParam
{
  WORD from_index,to_index;
};
struct CommentParam
{
  WORD thread_index,status;
  LPWSTR comment;
};
class Profile
{
public:
  Profile();
  Profile(const Profile& p);
  ~Profile();
  void Release();
  void AddHook(const HookParam& hp);
  void RemoveThread(int index);
  void RemoveLink(int index);
  void RemoveComment(int index);
  void ClearHooks();
  int AddThread(ThreadParam *tp);
  int AddLink(LinkParam* lp);
  int AddComment(LPWSTR comment, WORD index);
  Profile& operator = (Profile& pf);
  HookParam hps[4];
  WORD hook_count,thread_count,link_count,comment_count,select_index;
  WORD engine_type,thread_allocate,link_allocate,comment_allocate,flag;
  ThreadParam* threads;
  LinkParam* links;
  CommentParam *comments;
};
typedef TreeNode<LPWSTR,Profile> ProfileNode;
class ProfileManager
{
public:
  ProfileManager();
  ~ProfileManager();
  void AddProfile(LPWSTR path, const Profile& p);
  void ClearProfile();
  void LoadProfile();
  void SaveProfile();
  void DeleteProfile(int index);
  void DeleteProfile(LPWSTR path);
  void RefreshProfileAddr(DWORD pid,LPWSTR path);
  void SetProfileEngine(LPWSTR path, DWORD type);
  bool IsPathProfile(LPWSTR path);
  ProfileNode* GetProfile(LPWSTR path);
  ProfileNode* GetProfile(int index);
  ProfileNode* BeginProfile();
  ProfileNode* EndProfile();
private:
  AVLTree<WCHAR,Profile,WCMP,WCPY,WLEN> pftree;
  HANDLE hMonitorThread;
};
void GetCode(const HookParam& hp, LPWSTR buffer, DWORD pid=0);
void GetThreadString(ThreadParam* tp, LPWSTR str);
*/
