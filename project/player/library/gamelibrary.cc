// gamelibrary.cc
// 8/18/2012

#include "gamelibrary.h"
#include <QtCore/QFile>
#include <QtCore/QMutexLocker>

#define DEBUG "gamelibrary"
#include "module/debug/debug.h"

// - Construction -

GameLibrary::GameLibrary(const QString &xmlLocation, QObject *parent)
  : Base(parent), libraryLocation_(xmlLocation), dirty_(true)
{ }

// - Properties -

bool
GameLibrary::exists() const
{ return QFile::exists(libraryLocation_); }

bool
GameLibrary::containsDigest(const QString &digest) const
{
  //DOUT("locking");
  //QMutexLocker lock(&mutex_);
  //DOUT("locked, autorelease");
  return library_.contains(digest);
}

bool
GameLibrary::containsLocation(const QString &location) const
{
  //DOUT("locking");
  //QMutexLocker lock(&mutex_);
  //DOUT("locked, autorelease");
  return digestByLocation_.contains(location);
}

Game
GameLibrary::findGameByDigest(const QString &digest) const
{
  DOUT("locking");
  QMutexLocker lock(&mutex_);
  DOUT("locked, autorelease");
  auto p = library_.find(digest);
  return p == library_.end() ? Game() : p.value();
}

Game
GameLibrary::findGameByLocation(const QString &location) const
{
  DOUT("locking");
  QMutexLocker lock(&mutex_);
  DOUT("locked, autorelease");
  auto p = digestByLocation_.find(location);
  if (p != digestByLocation_.end()) {
    auto q = library_.find(p.value());
    if (q != library_.end())
      return q.value();
  }
  return Game();
}

// - Actions -

void
GameLibrary::load()
{
  //DOUT("locking");
  //QMutexLocker lock(&mutex_);
  //DOUT("locked, autorelease");
  if (!digestByLocation_.isEmpty())
    digestByLocation_.clear();

  if (QFile::exists(libraryLocation_)) {
    library_ = Game::readHash(libraryLocation_);

    foreach (const Game &game, library_)
      if (game.hasLocation() && game.hasDigest())
        digestByLocation_[game.location()] = game.digest();
  } else if (!library_.isEmpty())
    library_.clear();
  dirty_ = false;
  DOUT("library size =" << library_.size());
}

void
GameLibrary::save()
{
  if (!dirty_)
    return;

  DOUT("library size =" << library_.size());

  DOUT("locking");
  QMutexLocker lock(&mutex_);
  DOUT("locked, autorelease");

  if (library_.isEmpty())
    QFile::remove(libraryLocation_);
  else
    Game::write(library_, libraryLocation_);
  dirty_ = false;
}

void
GameLibrary::clear()
{
  if (library_.isEmpty())
    return;
  library_.clear();
  digestByLocation_.clear();
  dirty_ = true;
}

// - Visit -

void
GameLibrary::visit(const Game &game)
{
  QString k = game.key();
  Q_ASSERT(!k.isEmpty());
  if (k.isEmpty())
    return;

  DOUT("enter: location =" << game.location() << ", title =" << game.title());

  DOUT("locking");
  QMutexLocker lock(&mutex_);
  DOUT("locked, autorelease");

  dirty_ = true;

  auto p = library_.find(k);
  if (p == library_.end())
    library_[k] = game;
  else {
    p->setEnabled(game.isEnabled());
    p->visitCount() += game.visitCount();
    p->setVisitTime(game.visitTime());
    if (game.hasLocation())
      p->setLocation(game.location());
    if (game.hasTitle())
      p->setTitle(game.title());
    if (game.hasEncoding())
      p->setEncoding(game.encoding());
    if (game.hasThreads())
      p->setThreads(game.threads());
  }

  DOUT("exit");
}

void
GameLibrary::update(const Game &game)
{
  QString k = game.key();
  Q_ASSERT(!k.isEmpty());
  if (k.isEmpty())
    return;

  DOUT("enter: location =" << game.location() << ", title =" << game.title());

  DOUT("locking");
  QMutexLocker lock(&mutex_);
  DOUT("locked, autorelease");

  dirty_ = true;

  library_[k] = game;

  DOUT("exit");
}

// EOF
