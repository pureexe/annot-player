#ifndef CASTMANAGER_P_H
#define CASTMANAGER_P_H

// castmanager_p.h
// 11/15/2011

#include "module/annotcloud/traits.h"
#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QObject>

struct CastEvent
{
  typedef CastEvent Self;
  enum Type { NullEvent = 0, BlessEvent, CurseEvent, BlockEvent };
  typedef AnnotCloud::Traits::Entity Entity;

  Type type;
  qint64 targetId;
  Entity targetType;

public:
  explicit
  CastEvent(Type t = Type(0), qint64 tid = 0, Entity ttype = Entity(0))
    : type(t), targetId(tid), targetType(ttype)
  { }

  bool operator==(const Self &that) const
  { return type == that.type && targetId == that.targetId && targetType == that.targetType; }
};

class CastManager : public QObject
{
  typedef CastManager Self;
  typedef QObject Base;

  QList<CastEvent> events_;
  mutable QMutex mutex_;

public:
  explicit CastManager(QObject *parent = nullptr)
    : Base(parent) { }

  bool containsEvent(const CastEvent &e) const;
  bool registerEvent(const CastEvent &e);
};

#endif // CASTMANAGER_P_H
