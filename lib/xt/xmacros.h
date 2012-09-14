#ifndef XMACROS_H
#define XMACROS_H

// xmacros.h
// 10/16/2011

//inline void xt_noop(void) { }

//template <typename T>
//inline void xUnused(T &x) { (void)x; }

#define X_UNUSED(x) (void)x

#define X_NOP   X_UNUSED(0)

#define X_DISABLE_COPY(Class) \
  Class(const Class &); \
  Class &operator=(const Class &);

#ifndef XT_NO_DEBUG
# include <cassert>
# define X_ASSERT(cond) assert(cond)
#else
# define X_ASSERT(cond) X_NOP
#endif // !XT_NO_DEBUG

#endif // XMACROS_H
