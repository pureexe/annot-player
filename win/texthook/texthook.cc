// texthook.cc
// 10/14/2011

#include "texthook.h"
#include "ith.h"
#include "qtwin/qtwin.h"
#include <QtCore>

//#define DEBUG "qth"
#include "module/debug/debug.h"

// - Constructions -

void
TextHook::setEnabled(bool t)
{
  if (enabled_ != t)
    emit enabledChanged(enabled_ = t);
}

bool
TextHook::isActive() const
{ return Ith::isLoaded(); }

void
TextHook::start()
{ Ith::load(); }

void
TextHook::stop()
{
  clear();
  Ith::unload();
}

// - Properties -

WId
TextHook::parentWinId() const
{ return Ith::parentWindow(); }

void
TextHook::setParentWinId(WId hwnd)
{ Ith::setParentWindow(hwnd); }

int
TextHook::interval() const
{ return Ith::messageInterval(); }

void
TextHook::setInterval(int msecs)
{ Ith::setMessageInterval(msecs); }

// - Injection -

void
TextHook::clear()
{
  DOUT("enter");
  if (!isEmpty())
    detachAllProcesses();
  DOUT("exit");
}

bool
TextHook::attachOneProcess(ulong pid, bool checkActive)
{
  DOUT("enter: pid =" << pid);
  DOUT("isAttached =" << isProcessAttached(pid));
  if (!isActive())
    start();

  if (checkActive && !QtWin::isProcessActiveWithId(pid)) {
    DOUT("exit: ret = false, isActive = false");
    return false;
  }

  if (isProcessAttached(pid))
    return true;

  detachAllProcesses();

  bool ret = Ith::attachProcess(pid);
  if (ret && !isProcessAttached(pid)) {
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
  DOUT("isAttached =" << isProcessAttached(pid));
  if (!isActive())
    start();

  if (checkActive && !QtWin::isProcessActiveWithId(pid)) {
    DOUT("exit: ret = false, isActive = false");
    return false;
  }

  bool ret = Ith::attachProcess(pid);
  if (ret && !isProcessAttached(pid)) {
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
  DOUT("isAttached =" << isProcessAttached(pid));
  Q_ASSERT(isActive());

  int i = pids_.indexOf(pid);
  if (i < 0) {
    DOUT("exit: ret = false, not attached");
    return false;
  }
  pids_.removeAt(i);

  if (checkActive && !QtWin::isProcessActiveWithId(pid)) {
    DOUT("exit: ret = false, isActive = false");
    return false;
  }

  bool ret = Ith::detachProcess(pid);
  DOUT("exit: ret =" << ret);
  return ret;
}

void
TextHook::detachAllProcesses()
{
  DOUT("enter");
  foreach (ulong pid, pids_)
    detachProcess(pid);
  DOUT("exit");
}

bool
TextHook::isProcessAttached(ulong pid) const
{ return pids_.indexOf(pid) >= 0; }

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
//  //return Ith::getHookNameById(hookId);
//  // FIXME: supposed to be the engine name, unimplemented
//  Q_UNUSED(hookId);
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

// EOF
