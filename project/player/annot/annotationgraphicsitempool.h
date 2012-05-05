#ifndef ANNOTATIONGRAPHICSITEMPOOL_H
#define ANNOTATIONGRAPHICSITEMPOOL_H

// annotationgraphicsitempool.h
// 5/4/2012

#include <QtCore/QHash>
#include <QtCore/QObject>

class AnnotationGraphicsItem;

class AnnotationGraphicsView;
class SignalHub;

class AnnotationGraphicsItemPool : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationGraphicsItemPool)
  typedef AnnotationGraphicsItemPool Self;
  typedef QObject Base;
public:
  typedef AnnotationGraphicsItem value_type;

  AnnotationGraphicsItemPool(AnnotationGraphicsView *view, SignalHub *hub, QObject *parent = 0)
    : Base(parent), view_(view), hub_(hub) { }

  ~AnnotationGraphicsItemPool() { clear(); }

public:
  int capacity() const { return used_.size(); }
  void reserve(int size);
  void clear();
public:
  value_type *allocate();
  void release(value_type *item);

protected:
  value_type *create() const;

private:
  typedef QHash<value_type *, bool> Hash;
  Hash used_;

  mutable AnnotationGraphicsView *view_;
  mutable SignalHub *hub_;
  //QMutex m_;
};

#endif // ANNOTATIONGRAPHICSITEMPOOL_H

