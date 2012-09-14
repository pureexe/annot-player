// textthread_p.cc
// 6/6/2012

#include "textthread_p.h"
#include "texthook.h"
#include <ITH/IHF.h>
#include <QtCore>

//#define DEBUG "textthread_p"
#include "lib/debug/debug.h"

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

qint64
TextThreadDelegate::signature() const
{
  if (!signature_)
    signature_ =
        (threadContext() & 0xFFFF) |
        (threadSubcontext() & 0xFFFF)<<16;
  return signature_;
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
TextThreadDelegate::provider() const
{
  if (provider_.isEmpty()) {
    enum { buf_size = 0x200 }; // 0x200 is used by ITH internally
    wchar_t buf[buf_size];
    ulong len = t_->GetThreadString(buf, buf_size);
    if (len)
      provider_ = QString::fromWCharArray(buf, len);
  }
  return provider_;
}

// - Actions -

void
TextThreadDelegate::send()
{ TextHook::globalInstance()->sendMessage(buffer_, signature(), provider()); }

// EOF
