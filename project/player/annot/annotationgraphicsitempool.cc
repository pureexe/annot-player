// annotationgraphicsitempool.cc
// 5/4/2012

#include "annotationgraphicsitempool.h"
#include "annotationgraphicsitem.h"

#define DEBUG "annotationgraphicsitempool"
#include "module/debug/debug.h"

// - Actions -

AnnotationGraphicsItemPool::value_type*
AnnotationGraphicsItemPool::create()
{
  capacity_++;
  return new value_type(data_, hub_, view_);
}

void
AnnotationGraphicsItemPool::clear()
{
  DOUT("capacity =" << capacity_);
  while (!stack_.isEmpty()) {
    value_type *t =  stack_.pop();
    if (t->autoDelete())
      delete t;
  }
}

void
AnnotationGraphicsItemPool::reserve(int size)
{
  DOUT("size =" << size);
  while (capacity_ < size)
    stack_.push(create());
}

AnnotationGraphicsItemPool::value_type*
AnnotationGraphicsItemPool::allocate()
{
  //DOUT("enter");
  return stack_.isEmpty() ? create() : stack_.pop();
  //DOUT("exit");
}

void
AnnotationGraphicsItemPool::release(value_type *item)
{
  //DOUT("enter");
  stack_.push(item);
  //DOUT("exit");
}

// EOF
