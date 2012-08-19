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

Game
GameLibrary::findGame(const QString &digest) const
{
  DOUT("locking");
  QMutexLocker lock(&mutex_);
  DOUT("locked");
  auto p = library_.find(digest);
  return p == library_.end() ? Game() : p.value();
}

// - Actions -

void
GameLibrary::load()
{
  //DOUT("locking");
  //QMutexLocker lock(&mutex_);
  //DOUT("locked");

  if (QFile::exists(libraryLocation_))
    library_ = Game::readHash(libraryLocation_);
  else if (!library_.isEmpty())
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
  DOUT("locked");

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
  DOUT("locked");

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
    if (game.hasAnchor())
      p->setAnchor(game.anchor());
    if (game.hasEncoding())
      p->setEncoding(game.encoding());
    if (game.hasFunction())
      p->setFunction(game.function());
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
  DOUT("locked");

  dirty_ = true;

  library_[k] = game;

  DOUT("exit");
}

// EOF
