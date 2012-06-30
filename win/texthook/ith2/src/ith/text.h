#pragma once

// ith/text.h
// 10/14/2011
// TODO: clean up this file

#include "ith/main.h"
#include "ith/main_template.h"

#include <QtCore/QString>
//#include <QtCore/QTextDecoder>

class TextBuffer : public MyVector<BYTE, 0x800>
{
  bool line_; // EOL
public:
  TextBuffer() : line_(false)
  {
    //NtClose(IthCreateThread(FlushThread,(DWORD)this));
  }

  void SetLine() { line_ = true; }

public:
  void AddText(PBYTE text, int len, bool newLine = false);
  void Flush();

  void ClearBuffer()
  { Reset(); line_ = false; }
};

struct RepeatCountNode
{
  short repeat;
  short count;
  RepeatCountNode* next;
};
struct ThreadParameter
{
  DWORD pid;
  DWORD hook;
  DWORD retn;
  DWORD spl;
  /*DWORD spl;
  DWORD retn;
  DWORD hook;
  DWORD pid;*/
};
MK_FUNDA_TYPE(ThreadParameter)

#define COUNT_PER_FOWARD 0x200
#define REPEAT_DETECT 0x10000
#define REPEAT_SUPPRESS 0x20000
#define REPEAT_NEWLINE 0x40000
class TextThread : public MyVector<BYTE, 0x200>
{
  typedef TextThread Self;

  //static QTextDecoder *textDecoder_;

  // - Qth instruments -
  int size_; // used for validation
  int id_;
  QString name_;

public:
  void sendText();

  void setId(int id) { id_ = id; }
  int id() const { return id_; }

  bool good() const { return size_ == sizeof(Self); }

  const QString &name() const
  {
    if (name_.isEmpty())
      const_cast<Self *>(this)->updateName();
    return name_;
  }

protected:
  void updateName();

  static QString decodeWText(LPCWSTR lp, int len = 0)
  { return QString::fromWCharArray(lp, len); }

  static QString decodeText(LPCSTR lp, int len = 0)
  {
    //QString ret;
    //if (lp && len > 0)
    //  ret = textDecoder_ ? textDecoder_->toUnicode(lp, len)
    //                     : QString::fromLocal8Bit(lp, len);
    //return ret;
    return QString::fromLocal8Bit(lp, len);
  }

  // - Original implementations -
public:
  TextThread(DWORD pid, DWORD hook, DWORD retn, DWORD spl, WORD num);
  virtual ~TextThread();
  void Reset();
  void AddToStore(BYTE* con,int len, bool new_line=false, bool console=false);
  void RemoveSingleRepeatAuto(BYTE* con, int &len);
  void RemoveSingleRepeatForce(BYTE* con, int &len);
  void RemoveCyclicRepeat(BYTE* &con, int &len);
  void ResetRepeatStatus();
  void AddLineBreak();
  void ResetEditText();

  void ComboSelectCurrent()
  {
    //int index;
    //WCHAR temp[0x200];
    //GetEntryString(temp);
    //index=SendMessage(hwndCombo, CB_FINDSTRINGEXACT , 0 , (LPARAM)temp);
    //if (index==CB_ERR) return;
    //SendMessage(hwndCombo, CB_SETCURSEL, index , 0);
  }

  void GetEntryString(LPWSTR str);
  void CopyLastSentence(LPWSTR str);
  //void CopyLastToClipboard();
  //void ExportTextToFile(LPWSTR filename);
  void AdjustPrevRepeat(DWORD len);
  void PrevRepeatLength(DWORD &len);
  void SetComment(LPWSTR);
  bool AddToCombo();
  bool RemoveFromCombo();
  bool CheckCycle(TextThread* start);
  void SetNewLineFlag();
  void SetNewLineTimer();

  // - Properties -

  DWORD PID() const {return tp.pid;}
  DWORD Addr() const {return tp.hook;}
  DWORD& Status() {return status;}
  WORD Number() const {return number;}
  WORD& Last() {return last;}
  WORD& LinkNumber() {return link_number;}
  UINT_PTR& Timer() {return timer;}
  ThreadParameter* GetThreadParameter() {return &tp;}
  TextThread*& Link() {return link;}

  void SetRepeatFlag();
  void ClearNewLineFlag();
  void ClearRepeatFlag();
  LPWSTR GetComment() {return comment;}

private:
  ThreadParameter tp;

  WORD number,link_number;
  WORD last,align_space;
  WORD repeat_single;
  WORD repeat_single_current;
  WORD repeat_single_count;
  WORD repeat_detect_count;
  RepeatCountNode* head;

  TextThread *link;
  LPWSTR comment,thread_string;
  UINT_PTR timer;
  DWORD status,repeat_detect_limit;
  DWORD last_sentence,prev_sentence,sentence_length,repeat_index,last_time;
};

// EOF
