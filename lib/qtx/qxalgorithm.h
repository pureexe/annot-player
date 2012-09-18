#ifndef QXALGOIRHTM_H
#define QXALGOIRHTM_H

// qtx/qxalgorithm.h
// 3/16/2012

#include "qtx/qtxglobal.h"
#ifdef __clang__
# pragma clang diagnostic ignored "-Wunused-parameter" // in boost algorithm
#endif // __clang__
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext.hpp>

QTX_BEGIN_NAMESPACE

template <typename L>
inline L
qxRevertList(const L &l)
{
  if (l.size() <= 1)
    return l;
  L ret;
  foreach (typename L::value_type v, l)
    ret.push_front(v);
  return ret;
}

///  Stable, very inefficient
template <typename L>
inline L
qxCopyUniqueList(const L &l)
{
  if (l.size() <= 1)
    return l;
  L ret;
  foreach (typename L::value_type v, l)
    if (!ret.contains(v))
      ret.push_back(v);
  return ret;
}

#ifdef QT_CORE_LIB
# include <QtAlgorithms>
# include <QtCore/QList>
# include <QtCore/QSet>
# include <QtCore/QStringList>

///  Unstable
template <typename L>
inline L&
qxUniqueList(L &l)
{
  qSort(l);
  boost::erase(l, boost::unique<boost::return_found_end>(l));
  return l;
}

///  Unstable
inline QStringList&
qxUniqueList(QStringList &l)
{
  l.sort();
  boost::erase(l, boost::unique<boost::return_found_end>(l));
  return l;
}

template <typename L>
inline L&
qxStableUniqueList(L &l)
{
  if (l.size() <= 1)
    return l;
  QSet<typename L::value_type> set;
  auto p = l.begin();
  while (p != l.end())
    if (set.contains(*p))
      p = l.erase(p);
    else
      set.insert(*p++);
  return l;
}

#else
# include <set>

///  Unstable
template <typename L>
inline L&
qxUniqueList(L &l)
{
  boost::erase(l, boost::unique<boost::return_found_end>(boost::sort(l)));
  return l;
}

template <typename L>
inline L&
qxStableUniqueList(L &l)
{
  if (l.size() <= 1)
    return l;
  std::set<L::value_type> set;
  auto p = l.begin();
  while (p != l.end())
    if (set.contains(*p))
      p = l.erase(p);
    else
      set.insert(*p++);
  return l;
}

#endif // QT_CORE_LIB

template <typename L>
inline L
qxSubList(const L &l, int count)
{
  if (l.size() <= count)
    return l;
  L ret;
  foreach (typename L::value_type v, l) {
    ret.push_back(v);
    if (ret.size() >= count)
      break;
  }
  return ret;
}

template <typename L>
inline L
qxSkipEmpty(const L &l)
{
  L ret;
  foreach (typename L::value_type v, l)
    if (!v.isEmpty())
      ret.push_back(v);
  return ret;
}

QTX_END_NAMESPACE

#endif // QXALGOIRHTM_H
