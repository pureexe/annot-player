// textthread_p.cc
// 6/6/2012

#include "textthread_p.h"
#include "texthook.h"
#include <ITH/IHF.h>
#include <QtCore>

//#define DEBUG "textthread_p"
#include "module/debug/debug.h"

// - Actions -

void
TextThreadProperty::send()
{
  ulong pid = t_->PID();
  //ulong tid = t_->Number();
  ulong tid = reinterpret_cast<ulong>(t_);
  TextHook::globalInstance()->sendText(text(), tid, pid);
}

// EOF
