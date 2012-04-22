#ifndef _QTEXT_ALGOIRHTM_H
#define _QTEXT_ALGOIRHTM_H

// qtext/algorithm.h
// 3/16/2012

#include <QtCore/QList>

namespace QtExt {

  template <typename L>
  inline L
  revertList(const L &l)
  {
    if (l.size() <= 1)
      return l;
    L ret;
    foreach (typename L::value_type v, l)
      ret.push_front(v);
    return ret;
  }

  template <typename L>
  inline L
  uniqueList(const L &l)
  {
    if (l.size() <= 1)
      return l;
    L ret;
    foreach (typename L::value_type v, l)
      if (!ret.contains(v))
        ret.push_back(v);
    return ret;
  }

  template <typename L>
  inline L
  skipEmpty(const L &l)
  {
    L ret;
    foreach (typename L::value_type v, l)
      if (!v.isEmpty())
        ret.push_back(v);
    return ret;
  }


} // namespace QtExt

#endif // _QTEXT_ALGOIRHTM_H
