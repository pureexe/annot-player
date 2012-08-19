#ifndef GAMELIBRARY_H
#define GAMELIBRARY_H

// gamelibrary.h
// 8/18/2012

#include "game.h"
#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QString>

class GameLibrary : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(GameLibrary)
  typedef GameLibrary Self;
  typedef QObject Base;

  mutable QMutex mutex_;
  QHash<QString, Game> library_;
  QString libraryLocation_;
  bool dirty_;

public:
  explicit GameLibrary(const QString &xmlLocation, QObject *parent = nullptr);

  bool isDirty() const { return dirty_; }
  bool isEmpty() const { return library_.isEmpty(); }
  bool exists() const;

  Game findGame(const QString &digest) const;

public slots:
  void save();
  void load();

  void clear();

  void visit(const Game &game);
  void update(const Game &game);
};

#endif // GAMELIBRARY_H
