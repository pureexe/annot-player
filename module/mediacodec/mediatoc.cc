// mediatoc.cc
// 2/13/2012
#include "module/mediacodec/mediatoc.h"

namespace { namespace detail {

  int roundIndex(qint64 pos, const QList<qint64> &l, int start, int stop)
  {
    #define SELF roundIndex
    Q_ASSERT(start <= stop);
    if (start+1 == stop)
      return start;
    int mid = (start + stop) / 2;
    if (pos < l[mid])
      return SELF(pos, l, start, mid);
    else
      return SELF(pos, l, mid, stop);
    #undef SELF
  }
} } // anonymous detail

qint64
MediaToc::roundOffset(qint64 pos) const
{
  const QList<qint64> &l = offsets_;
  m_.lock();
  if (l.isEmpty()) {
    m_.unlock();
    return -1;
  }
  qint64 q = l.first();
  if (pos <= q) {
    m_.unlock();
    return q;
  }
  q = l.last();
  if (pos >= q) {
    m_.unlock();
    return q;
  }

  int i = detail::roundIndex(pos, l, 0, l.size() - 1);
  q = l[i];
  m_.unlock();
  return q;
}

qint64
MediaToc::roundTimestamp(qint64 pos) const
{
  const QList<qint64> &l = timestamps_;
  m_.lock();
  if (l.isEmpty()) {
    m_.unlock();
    return -1;
  }
  qint64 q = l.first();
  if (pos <= q) {
    m_.unlock();
    return q;
  }
  q = l.last();
  if (pos >= q) {
    m_.unlock();
    return q;
  }

  int i = detail::roundIndex(pos, l, 0, l.size() - 1);
  q = l[i];
  m_.unlock();
  return q;
}

qint64
MediaToc::offsetByTime(qint64 msecs) const
{
  m_.lock();
  if (timestamps_.isEmpty()) {
    m_.unlock();
    return -1;
  }
  int index;
  if (msecs <= timestamps_.first())
    index = 0;
  else if (msecs >= timestamps_.last())
    index = timestamps_.size() - 1;
  else
    index = detail::roundIndex(msecs, timestamps_, 0, timestamps_.size() - 1);
  qint64 ret = offsets_[index];
  m_.unlock();
  return ret;
}

// EOF
