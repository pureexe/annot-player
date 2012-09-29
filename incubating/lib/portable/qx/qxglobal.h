#ifndef QXGLOBAL_H
#define QXGLOBAL_H

// qxglobal.h
// 9/15/2012 jichi

// Redefine QX_BEGIN_NAMESPACE/QX_END_NAMESPACE to turn on custom namespace
#ifndef QX_BEGIN_NAMESPACE
# define QX_BEGIN_NAMESPACE
#endif // QX_BEGIN_NAMESPACE
#ifndef QX_END_NAMESPACE
# define QX_END_NAMESPACE
#endif // QX_END_NAMESPACE

#define QX_FORWARD_DECLARE_CLASS(name)     QX_BEGIN_NAMESPACE class name;  QX_END_NAMESPACE
#define QX_FORWARD_DECLARE_STRUCT(name)    QX_BEGIN_NAMESPACE struct name; QX_END_NAMESPACE

QX_BEGIN_NAMESPACE
namespace Qtx {

} // Qtx
QX_END_NAMESPACE

// Qt style Pimp
#define QX_DECLARE_PRIVATE(_class) \
  typedef _class D; \
  friend class D; \
  D *const d;

// Qt style Pimp
#define QX_DECLARE_PUBLIC(_class) \
  typedef _class Q; \
  friend class Q; \
  Q *const q;

#define QX_CLASS(_self) \
  typedef _self Self; \
  Self *self() const { return const_cast<Self *>(this); }

#define QX_EXTEND_CLASS(_self, _base) \
  QX_CLASS(_self) \
  typedef _base Base;

#define QX_OFFSCREEN_POS   QPoint(-1, -1)
#define QX_OFFSCREEN_POSF  QPointF(-1, -1)

#endif // QXGLOBAL_H
