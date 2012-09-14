#ifndef GAME_H
#define GAME_H

// game.h
// 8/18/2012

#include "textthread.h"
#include <QtCore/QHash>
#include <QtCore/QMetaType>

class Game
{
  typedef Game Self;

  // - Operators -  required ordinary containers, including QSet
public:
  bool operator==(Self &that) const { return key() == that.key(); }

  // - Properties -

private:
  TextThreadList threads_;
public:
  const TextThreadList &threads() const { return threads_; }
  TextThreadList &threads() { return threads_; }
  void setThreads(const TextThreadList &l) { threads_ = l; }
  bool hasThreads() const { return !threads_.isEmpty(); }

private:
  ulong tokenId_;
public:
  ulong tokenId() const { return tokenId_; }
  void setTokenId(ulong id) { tokenId_ = id; }
  bool hasTokenId() const { return tokenId_; }

private:
  QString encoding_;
public:
  const QString &encoding() const { return encoding_; }
  void setEncoding(const QString &value) { encoding_ = value; }
  bool hasEncoding() const { return !encoding_.isEmpty(); }

private:
  QString digest_;
public:
  const QString &digest() const { return digest_; }
  void setDigest(const QString &digest) { digest_ = digest; }
  bool hasDigest() const { return !digest_.isEmpty(); }

private:
  QString hook_;
public:
  const QString &hook() const { return hook_; }
  void setHook(const QString &value) { hook_ = value; }
  bool hasHook() const { return !hook_.isEmpty(); }

private:
  bool enabled_;
public:
  bool isEnabled() const { return enabled_; }
  void setEnabled(bool t) { enabled_ = t; }

private:
  QString location_;
public:
  const QString &location() const { return location_; }
  void setLocation(const QString &location) { location_ = location; }
  bool hasLocation() const { return !location_.isEmpty(); }

private:
  QString title_;
public:
  const QString &title() const { return title_; }
  void setTitle(const QString &title) { title_ = title; }
  bool hasTitle() const { return !title_.isEmpty(); }

private:
  qint64 visitTime_;
public:
  qint64 visitTime() const { return visitTime_; }
  void setVisitTime(qint64 msec) { visitTime_ = msec; }
  bool hasVisitTime() const { return visitTime_; }

private:
  int visitCount_;
public:
  int visitCount() const { return visitCount_; }
  int &visitCount() { return visitCount_; }
  void setVisitCount(qint64 count) { visitCount_ = count; }
  bool hasVisitCount() const { return visitCount_; }

  // - Construction -
public:
  Game() : enabled_(true), visitTime_(0), visitCount_(0) { }

  bool hasKey() const { return hasDigest(); }
  const QString &key() const { return digest(); }

  static bool isValidHook(const QString &code);

  // - I/O -
public:
  template <typename L>
    static bool write(const L &l, const QString &fileName);

  static QList<Self> readList(const QString &fileName);
  static QHash<QString, Self> readHash(const QString &fileName);
};

QT_BEGIN_NAMESPACE
inline uint qHash(const Game &game) { return qHash(game.key()); }
QT_END_NAMESPACE

#endif // GAME_H
