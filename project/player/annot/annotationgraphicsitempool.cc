// annotationgraphicsitempool.cc
// 5/4/2012

#include "annotationgraphicsitempool.h"
#include "annotationgraphicsitem.h"
#include <QtCore/QDebug>
#include <boost/typeof/typeof.hpp>

//#define DEBUG "annotationgraphicsitempool"
#include "module/debug/debug.h"

// - Actions -

AnnotationGraphicsItemPool::value_type*
AnnotationGraphicsItemPool::create() const
{ return new value_type(hub_, view_); }

void
AnnotationGraphicsItemPool::clear()
{
  qDebug() << "annotationgraphicsitempool::clear: size =" << capacity();
  if (!used_.isEmpty()) {
    for (BOOST_AUTO(p, used_.begin()); p != used_.end(); ++p)
      if (p.key()->autoDelete())
        delete p.key();
    used_.clear();
  }
}

void
AnnotationGraphicsItemPool::reserve(int size)
{
  qDebug() << "annotationgraphicsitempool::reserve: size =" << size;
  for (int i = used_.size(); i < size; i++)
    used_[create()] = false;
}

AnnotationGraphicsItemPool::value_type*
AnnotationGraphicsItemPool::allocate()
{
  DOUT("enter");
  for (BOOST_AUTO(p, used_.begin()); p != used_.end(); ++p)
    if (!p.value()) {
      p.value() = true;
      DOUT("exit: reuse");
      value_type *ret = p.key();
      ret->reset();
      return ret;
    }

  value_type *ret = create();
  used_[ret] = true;
  DOUT("exit: new");
  return ret;
}

void
AnnotationGraphicsItemPool::release(value_type *item)
{
  DOUT("enter");
  used_[item] = false;
  DOUT("exit");
}

// EOF
