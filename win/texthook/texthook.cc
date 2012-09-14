// texthook.cc
// 10/14/2011

#include "texthook.h"
#include "ihf_p.h"
#include "qtwin/qtwin.h"
#include <QtCore>

#define DEBUG "texthook"
#include "lib/debug/debug.h"

// - Properties -

void
TextHook::setEnabled(bool t)
{
  if (enabled_ != t)
    emit enabledChanged(enabled_ = t);
}

bool
TextHook::isActive() const
{ return Ihf::isLoaded(); }

void
TextHook::start()
{ Ihf::load(); }

void
TextHook::stop()
{
  clear();
  Ihf::unload();
}

WId
TextHook::parentWinId() const
{ return Ihf::parentWindow(); }

void
TextHook::setParentWinId(WId hwnd)
{ Ihf::setParentWindow(hwnd); }

int
TextHook::interval() const
{ return Ihf::messageInterval(); }

void
TextHook::setInterval(int msecs)
{ Ihf::setMessageInterval(msecs); }

// - Injection -

void
TextHook::clear()
{
  DOUT("enter");
  if (!isEmpty())
    detachAllProcesses();
  DOUT("exit");
}

QList<ulong>
TextHook::attachedProcesses(bool checkActive) const
{
  if (isEmpty() || !checkActive)
    return pids_;

  QList<ulong> ret;
  foreach (ulong pid, pids_)
    if (QtWin::isProcessActiveWithId(pid))
      ret.append(pid);
  return ret;
}

ulong
TextHook::anyAttachedProcess(bool checkActive) const
{
  if (isEmpty())
    return 0;
 if (!checkActive)
   return pids_.first();

  foreach (ulong pid, pids_)
    if (QtWin::isProcessActiveWithId(pid))
      return pid;
  return 0;
}

bool
TextHook::attachOneProcess(ulong pid, bool checkActive)
{
  DOUT("enter: pid =" << pid);
  DOUT("isAttached =" << containsProcess(pid));
  if (!isActive())
    start();

  if (checkActive && !QtWin::isProcessActiveWithId(pid)) {
    DOUT("exit: ret = false, isActive = false");
    return false;
  }

  if (containsProcess(pid)) {
    DOUT("exit: pid already attached");
    return true;
  }

  detachAllProcesses();

  bool ret = Ihf::attachProcess(pid);
  if (ret && !containsProcess(pid)) {
    pids_.removeAll(pid);
    pids_.append(pid);
    emit processAttached(pid);
  }

  DOUT("exit: ret =" << ret);
  return ret;
}

bool
TextHook::attachProcess(ulong pid, bool checkActive)
{
  DOUT("enter: pid =" << pid);
  DOUT("isAttached =" << containsProcess(pid));
  if (!isActive())
    start();

  if (checkActive && !QtWin::isProcessActiveWithId(pid)) {
    DOUT("exit: ret = false, isActive = false");
    return false;
  }

  bool ret = Ihf::attachProcess(pid);
  if (ret && !containsProcess(pid)) {
    pids_.removeAll(pid);
    pids_.append(pid);
    emit processAttached(pid);
  }

  DOUT("exit: ret =" << ret);
  return ret;
}

bool
TextHook::detachProcess(ulong pid, bool checkActive)
{
  DOUT("enter: pid =" << pid);
  DOUT("isAttached =" << containsProcess(pid));
  Q_ASSERT(isActive());

  int i = pids_.indexOf(pid);
  if (i < 0) {
    DOUT("exit: ret = false, not attached");
    return false;
  }
  pids_.removeAt(i);

  hooks_.remove(pid);

  if (checkActive && !QtWin::isProcessActiveWithId(pid)) {
    DOUT("exit: ret = false, isActive = false");
    return false;
  }

  bool ret = Ihf::detachProcess(pid);
  DOUT("exit: ret =" << ret);
  return ret;
}

void
TextHook::detachAllProcesses()
{
  DOUT("enter");
  foreach (ulong pid, pids_)
    detachProcess(pid);
  if (!hooks_.isEmpty())
    hooks_.clear();
  DOUT("exit");
}

// - Helpers -

//bool
//TextHook::isStandardHookName(const QString &name) const
//{
//  static QSet<uint> hashes;
//  if (hashes.isEmpty()) {
//#define ADD(_text)  hashes.insert(qHash(QString(_text)))
//    ADD("ConsoleOutput");
//    ADD("GetTextExtentPoint32A");
//    ADD("GetGlyphOutlineA");
//    ADD("ExtTextOutA");
//    ADD("TextOutA");
//    ADD("GetCharABCWidthsA");
//    ADD("DrawTextA");
//    ADD("DrawTextExA");
//    ADD("GetTextExtentPoint32W");
//    ADD("GetGlyphOutlineW");
//    ADD("ExtTextOutW");
//    ADD("TextOutW");
//    ADD("GetCharABCWidthsW");
//    ADD("DrawTextW");
//    ADD("DrawTextExW");
//#undef ADD
//  }
//  uint h = qHash(name);
//  return hashes.contains(h);
//}
//
//bool
//TextHook::isKnownHookForProcess(const QString &hook, const QString &proc) const
//{
//  // TODO: update database on line periodically
//  qDebug() << "qth::isKnownHookForProcess: hook ="  << hook << ", proc =" << proc;
//
//  static QSet<uint> hashes;
//  if (hashes.isEmpty()) {
//#define ADD(_hook, _proc)  hashes.insert(qHash(QString(_hook) + "\n" + _proc))
//    //ADD("Malie", "malie"); // light
//    ADD("GetGlyphOutlineA", "STEINSGATE");
//    ADD("StuffScriptEngine", "EVOLIMIT");
//#undef ADD
//  }
//  uint h = qHash(hook + "\n" + proc);
//  return hashes.contains(h);
//}
//
//QString
//TextHook::hookNameById(ulong hookId) const
//{
//  //return Ihf::getHookNameById(hookId);
//  // FIXME: supposed to be the engine name, unimplemented
//  Q_UNUSED(hookId)
//  return QString();
//}

QString
TextHook::guessEncodingForFile(const QString &fileName)
{
  static QHash<QString, QString> db;
  if (db.isEmpty()) {
    db["malie.exe"] = "UTF-16";
  }
  auto p = db.find(fileName);
  return p == db.end() ? QString() : p.value();
}

// - Hook -

bool
TextHook::addHook(ulong pid, const QString &code)
{
  DOUT("enter: pid =" << pid << ", code =" << code);
  if (isEmpty() || !containsProcess(pid)) {
    DOUT("exit: failed, process not attached");
    return false;
  }
  if (hooks_.contains(pid)) {
    DOUT("exit: failed, hook code not found");
    return false;
  }
  bool ok = Ihf::addHook(pid, code, addedHookName());
  if (ok)
    hooks_[pid] = code;
  DOUT("exit: ret =" << ok);
  return ok;
}

bool
TextHook::removeHook(ulong pid)
{
  DOUT("enter");
  auto p = hooks_.find(pid);
  if (p == hooks_.end()) {
    DOUT("exit: not hooked");
    return false;
  }

  DOUT("remove existing hook, THIS SHOULD NOT HAPPEN");
  bool ok = Ihf::removeHook(pid, p.value());
  hooks_.erase(p);
  DOUT("exit: ret =" << ok);
  return ok;
}

QString
TextHook::processHook(ulong pid) const
{
  auto p = hooks_.find(pid);
  return p == hooks_.end() ? QString() : p.value();
}

// EOF
