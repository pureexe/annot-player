// textthread_p.cc
// 6/6/2012

#include "textthread_p.h"
#include "texthook.h"
#include <ITH/IHF.h>
#include <QtCore>

//#define DEBUG "textthread_p"
#include "module/debug/debug.h"

// - Properties

ulong
TextThreadDelegate::processId() const
{ return t_->PID(); }

ulong
TextThreadDelegate::threadContext() const
{ return t_->GetThreadParameter()->retn; }

ulong
TextThreadDelegate::threadSubcontext() const
{ return t_->GetThreadParameter()->spl; }

ulong
TextThreadDelegate::threadAnchor() const
{
  if (!threadAnchor_)
    threadAnchor_ = threadContext() + threadSubcontext();
  return threadAnchor_;
}

QString
TextThreadDelegate::threadSummary() const
{
  enum { buf_size = 0x200 }; // 0x200 is used by ITH internally
  wchar_t buf[buf_size];
  ulong len = t_->GetEntryString(buf, buf_size);
  return len ? QString::fromWCharArray(buf, len) : QString();
}

QString
TextThreadDelegate::threadMethod() const
{
  if (threadMethod_.isEmpty()) {
    enum { buf_size = 0x200 }; // 0x200 is used by ITH internally
    wchar_t buf[buf_size];
    ulong len = t_->GetThreadString(buf, buf_size);
    if (len)
      threadMethod_ = QString::fromWCharArray(buf, len);
  }
  return threadMethod_;
}

// - Actions -

void
TextThreadDelegate::send()
{ TextHook::globalInstance()->sendMessage(buffer_, threadAnchor(), threadMethod()); }

// EOF
