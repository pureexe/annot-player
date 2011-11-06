#ifndef _CORE_UTIL_HASH_H
#define _CORE_UTIL_HASH_H

// core/util/hash.h
// 8/1/2011

#include <QString>
//#include <cstdarg>

namespace Core {

  // String hash: http://www.cse.yorku.ca/~oz/hash.html
  namespace Hash {

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

  } // namespace Hash

  //inline qint64 hashString(const QString &str, Hash::cstr_hash hash = Hash::djb2)
  //{
  //  const char *c_str = str.toLatin1();
  //  Q_ASSERT(hash);
  //  return hash(c_str);
  //}

} // namespace Core

#endif // _CORE_UTIL_HASH_H
