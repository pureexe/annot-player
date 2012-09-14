#ifndef PROCESSFILTER_H
#define PROCESSFILTER_H

// processfilter.h
// 8/22/2012

#include <QtCore/QObject>

class MediaLibrary;
class GameLibrary;
class ProcessFilter : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(ProcessFilter)
  typedef ProcessFilter Self;
  typedef QObject Base;

  const GameLibrary *library_;

public:
  explicit ProcessFilter(GameLibrary *library, QObject *parent = nullptr)
  : Base(parent), library_(library) { Q_ASSERT(library_); }

public:
  qint64 currentGamePid() const;

  static bool processNameIsNotGalGame(const QString &procName);
};

#endif // PROCESSFILTER_H
