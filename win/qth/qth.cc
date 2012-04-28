// qth.cc  10/14/2011

#include "qth.h"
#include "ith.h"
#include "qtwin/qtwin.h"
#include <QtCore>

//#define DEBUG "qth"
#include "module/debug/debug.h"

// - Constructions -

//bool Qth::ithActive_= false;

Qth *Qth::globalInstance() { static Self g; return &g; }

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
  Ith::destroy();
  DOUT("exit");
}

// - Properties -

WId
Qth::parentWinId() const
{ return Ith::parentWindow(); }

void
Qth::setParentWinId(WId hwnd)
{ Ith::setParentWindow(hwnd); }

int
Qth::interval() const
{ return Ith::messageInterval(); }

void
Qth::setInterval(int msecs)
{ Ith::setMessageInterval(msecs); }

bool
Qth::isEmpty() const
{ return pids_.empty(); }

// - Injection -

void
Qth::clear()
{
  DOUT("enter");
  if (!isEmpty())
    detachAllProcesses();
  DOUT("exit");
}

bool
Qth::attachProcess(ulong pid, bool checkActive)
{
  DOUT("enter: pid =" << pid);
  DOUT("isAttached =" << isProcessAttached(pid));
  //if (!ithActive_) {
  //  ithActive_ = true;
  //  Ith::init();
  //}

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

void
Qth::detachAllProcesses()
{
  DOUT("enter");
  foreach (ulong pid, pids_)
    detachProcess(pid);
  DOUT("exit");
}

bool
Qth::isProcessAttached(ulong pid) const
{ return pids_.indexOf(pid) >= 0; }

// - Helpers -

bool
Qth::isElevated() const
{
  static int ret = -1;
  if (ret <= 0)
    ret = Ith::isElevated() ? 1 : 0;
  return ret;
}

bool
Qth::isStandardHookName(const QString &name) const
{
  static QSet<uint> hashes;
  if (hashes.isEmpty()) {
#define ADD(_text)  hashes.insert(qHash(QString(_text)))
    ADD("ConsoleOutput");
    ADD("GetTextExtentPoint32A");
    ADD("GetGlyphOutlineA");
    ADD("ExtTextOutA");
    ADD("TextOutA");
    ADD("GetCharABCWidthsA");
    ADD("DrawTextA");
    ADD("DrawTextExA");
    ADD("GetTextExtentPoint32W");
    ADD("GetGlyphOutlineW");
    ADD("ExtTextOutW");
    ADD("TextOutW");
    ADD("GetCharABCWidthsW");
    ADD("DrawTextW");
    ADD("DrawTextExW");
#undef ADD
  }
  uint h = qHash(name);
  return hashes.contains(h);
}

bool
Qth::isKnownHookForProcess(const QString &hook, const QString &proc) const
{
  // TODO: update database on line periodically
  qDebug() << "qth::isKnownHookForProcess: hook ="  << hook << ", proc =" << proc;

  static QSet<uint> hashes;
  if (hashes.isEmpty()) {
#define ADD(_hook, _proc)  hashes.insert(qHash(QString(_hook) + "\n" + _proc))
    ADD("Malie", "malie"); // light
    ADD("GetGlyphOutlineA", "STEINSGATE");
    ADD("StuffScriptEngine", "EVOLIMIT");
#undef ADD
  }
  uint h = qHash(hook + "\n" + proc);
  return hashes.contains(h);
}

ulong
Qth::processIdByName(const QString &name) const
{ return Ith::getProcessIdByName(name); }

QString
Qth::hookNameById(ulong hookId) const
{ return Ith::getHookNameById(hookId); }

// EOF
