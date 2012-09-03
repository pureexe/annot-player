#ifndef GAMELIBRARY_H
#define GAMELIBRARY_H

// gamelibrary.h
// 8/18/2012

#include "game.h"
#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QString>

#define GAME_LIBRARY_VERSION    "0.1.8"

class GameLibrary : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(GameLibrary)
  typedef GameLibrary Self;
  typedef QObject Base;

  mutable QMutex mutex_;
  QString libraryLocation_;
  bool dirty_;

  QHash<QString, Game> library_; // indexed by digest
  QHash<QString, QString> digestByLocation_; // index on location

public:
  explicit GameLibrary(const QString &xmlLocation, QObject *parent = nullptr);

  bool isDirty() const { return dirty_; }
  bool isEmpty() const { return library_.isEmpty(); }
  bool exists() const;

  Game findGameByDigest(const QString &digest) const;
  bool containsDigest(const QString &digest) const;

  Game findGameByLocation(const QString &location) const;
  bool containsLocation(const QString &location) const;

  bool containsExecutable(const QString &fileName) const;

public slots:
  void save();
  void load();

  void clear();

  void visit(const Game &game);
  void update(const Game &game);
};

#endif // GAMELIBRARY_H
