#ifndef QXHASH_H
#define QXHASH_H

// qxhash.h
// 8/1/2011

#include "qtx/qtxglobal.h"
#include <QtCore/QString>
//#include <cstdarg>

QTX_BEGIN_NAMESPACE

// See: http://www.cse.yorku.ca/~oz/hash.html
namespace QxHash {

  //typedef quint64 (*cstr_hash)(const char*);

  ///  djb2: h = h*33 + c
  inline quint64
  djb2(const char *str, quint64 hash = 5381)
  {
    int c;
    while ((c = *str++))
      hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash;
  }

  ///  sdbm: hash(i) = hash(i - 1) * 65599 + str[i];
  inline quint64
  sdbm(const char *str, quint64 hash = 0)
  {
    int c;
    while ((c = *str++))
       hash = c + (hash << 6) + (hash << 16) - hash;
    return hash;
  }

  inline quint64
  loselose(const char *str, quint64 hash = 0)
  {
    int c;
    while ((c = *str++))
      hash += c;

    return hash;
  }

} // namespace QxHash

QTX_END_NAMESPACE

#endif // QXHASH_H
