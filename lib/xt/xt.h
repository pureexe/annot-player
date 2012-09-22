#ifndef XT_H
#define XT_H

// xt.h
// 10/16/2011

#include "xt/xmacros.h"
#include <algorithm>
#include <iterator>
#include <string>

namespace xt {

  // See: http://stackoverflow.com/questions/1701067/how-to-check-that-an-element-is-in-a-stdset
  template<class Container>
  inline typename Container::iterator begin(Container &c)
  { return (c.begin()); }

  template<class Container>
  inline typename Container::const_iterator begin(const Container &c)
  { return (c.begin()); }

  template<class Container>
  inline typename Container::iterator end(Container &c)
  { return (c.end()); }

  template<class Container>
  inline typename Container::const_iterator end(const Container &c)
  { return (c.end()); }

  template<class T, size_t size>
  inline T *begin(T (&_Array)[size])
  { return (&_Array[0]); }

  template<class T, size_t size>
  inline T *end(T (&_Array)[size])
  { return (&_Array[0] + size); }

  template<class InputIterator, class T> inline
  bool contains(InputIterator first, InputIterator last, const T &value)
  { return std::find(first, last, value) != last; }

  // Using std::begin std::end for C++11 instead.
  template<class C, class T> inline
  bool contains(const C &container, const T& value)
  { return contains(begin(container), end(container), value);  }

  // Specialize
  template<class T> inline
  bool contains(const std::set<T> &container, const T &value)
  { return container.find(value) != container.end(); }

  // Specialize
  template<class T> inline
  bool contains(const std::string &container, const T &value)
  { return container.find(value) != std::string::npos; }

} // namespace xt

#endif // XT_H
