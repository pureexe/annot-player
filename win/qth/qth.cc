// qth.cc  10/14/2011

#include "qth.h"
#include "ith.h"
#include "win/qtwin/qtwin.h"
#include <QtCore>

#define DEBUG "Qth"
#include "module/debug/debug.h"

// - Constructions -

Qth *Qth::globalInstance() { static Self global; return &global; }

//void
//Qth::startService()
//{ Ith::startService(); }

Qth::Qth(QObject *parent)
  : Base(parent)
{
  DOUT("Qth:enter");
  Ith::init();
  DOUT("Qth:exit");
}

Qth::~Qth()
{
  DOUT("~Qth:enter");
  clear();
  // FIXME: need wait for detaching processes before destroying ITH
  Ith::destroy();
  DOUT("~Qth:exit");
}

void
Qth::setParentWinId(WId hwnd)
{ Ith::setWindowHandle(hwnd); }

// - Actions -

bool
Qth::isEmpty() const
{ return pids_.empty(); }

void
Qth::clear()
{
  DOUT("clear:enter");
  foreach (ulong pid, pids_)
    detachProcess(pid);
  DOUT("clear:exit");
}

bool
Qth::attachProcess(ulong pid, bool checkActive)
{
  DOUT("attachProcess:enter: pid =" << pid);
  DOUT("attachProcess: isAttached =" << isProcessAttached(pid));

  if (checkActive && !QtWin::isProcessActiveWithId(pid)) {
    DOUT("attachProcess:exit: ret = false, isActive = false");
    return false;
  }

  bool ret = Ith::attachProcess(pid);
  if (ret && !isProcessAttached(pid))
    pids_.append(pid);

  DOUT("attachProcess:exit: ret =" << ret);
  return ret;
}

bool
Qth::detachProcess(ulong pid, bool checkActive)
{
  DOUT("detachProcess:enter: pid =" << pid);
  DOUT("detachProcess: isAttached =" << isProcessAttached(pid));

  pids_.removeAll(pid);

  if (checkActive && !QtWin::isProcessActiveWithId(pid)) {
    DOUT("detachProcess:exit: ret = false, isActive = false");
    return false;
  }

  bool ret = Ith::detachProcess(pid);
  DOUT("detachProcess:exit: ret =" << ret);
  return ret;
}

bool
Qth::isProcessAttached(ulong pid) const
{ return pids_.indexOf(pid) >= 0; }

// - Signals -

void
Qth::emit_textReceived(const QString &text, int hookId, qint64 tsMsecs) ///< Ith callback
{
  DOUT("emit_textReceived:enter: text =" << text);
  emit textReceived(text, hookId, tsMsecs);
  DOUT("emit_textReceived:exit");
}

// - Helpers -

bool
Qth::isElevated() const
{
  static int ret = -1;
  if (ret <= 0)
    ret = Ith::isElevated() ? 1 : 0;
  return ret;
}

ulong
Qth::getProcessIdByName(const QString &name) const
{ return Ith::getProcessIdByName(name); }

// EOF
