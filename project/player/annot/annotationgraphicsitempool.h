#ifndef ANNOTATIONGRAPHICSITEMPOOL_H
#define ANNOTATIONGRAPHICSITEMPOOL_H

// annotationgraphicsitempool.h
// 5/4/2012

#include <QtCore/QObject>
#include <QtCore/QStack>

class AnnotationGraphicsItem;

class AnnotationGraphicsView;
class DataManager;
class SignalHub;

class AnnotationGraphicsItemPool : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationGraphicsItemPool)
  typedef AnnotationGraphicsItemPool Self;
  typedef QObject Base;
public:
  typedef AnnotationGraphicsItem value_type;

  AnnotationGraphicsItemPool(AnnotationGraphicsView *view, DataManager *data, SignalHub *hub, QObject *parent = 0)
    : Base(parent), capacity_(0),
      view_(view), data_(data), hub_(hub) { }

  ~AnnotationGraphicsItemPool() { clear(); }

public:
  int capacity() const { return capacity_; }
  int size() const { return capacity_ - stack_.size(); }
  void reserve(int size);
  void clear();
public:
  value_type *allocate();
  void release(value_type *item);

protected:
  value_type *create();

private:
  //typedef QHash<value_type *, bool> Hash;
  //Hash used_;
  typedef QStack<value_type *> stack_t;
  stack_t stack_;

  int capacity_;
  mutable AnnotationGraphicsView *view_;
  mutable DataManager *data_;
  mutable SignalHub *hub_;
  //QMutex m_;
};

#endif // ANNOTATIONGRAPHICSITEMPOOL_H

