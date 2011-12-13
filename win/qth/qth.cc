// qth.cc  10/14/2011

#include "qth.h"
#include "ith.h"
#include "win/qtwin/qtwin.h"
#include <QtCore>

#define DEBUG "qth"
#include "module/debug/debug.h"

// - Constructions -

Qth *Qth::globalInstance() { static Self global; return &global; }

//void
//Qth::startService()
//{ Ith::startService(); }

Qth::Qth(QObject *parent)
  : Base(parent)
{
  DOUT("enter");
  Ith::init();
  DOUT("exit");
}

Qth::~Qth()
{
  DOUT("enter");
  clear();
  // FIXME: need wait for detaching processes before destroying ITH
  Ith::destroy();
  DOUT("exit");
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
  DOUT("enter");
  foreach (ulong pid, pids_)
    detachProcess(pid);
  DOUT("exit");
}

bool
Qth::attachProcess(ulong pid, bool checkActive)
{
  DOUT("enter: pid =" << pid);
  DOUT("isAttached =" << isProcessAttached(pid));

  if (checkActive && !QtWin::isProcessActiveWithId(pid)) {
    DOUT("exit: ret = false, isActive = false");
    return false;
  }

  bool ret = Ith::attachProcess(pid);
  if (ret && !isProcessAttached(pid))
    pids_.append(pid);

  DOUT("exit: ret =" << ret);
  return ret;
}

bool
Qth::detachProcess(ulong pid, bool checkActive)
{
  DOUT("enter: pid =" << pid);
  DOUT("isAttached =" << isProcessAttached(pid));

  pids_.removeAll(pid);

  if (checkActive && !QtWin::isProcessActiveWithId(pid)) {
    DOUT("exit: ret = false, isActive = false");
    return false;
  }

  bool ret = Ith::detachProcess(pid);
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
Qth::isProcessAttached(ulong pid) const
{ return pids_.indexOf(pid) >= 0; }

// - Signals -

void
Qth::emit_textReceived(const QString &text, int hookId, qint64 tsMsecs) ///< Ith callback
{
  DOUT("enter: text =" << text);
  emit textReceived(text, hookId, tsMsecs);
  DOUT("exit");
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
