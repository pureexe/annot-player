// 9/11/2012
// edict_p.cc
#include "lib/edrdg/edict_p.h"
#include "lib/edrdg/edict.h"

Edict::ItemList
Edict::
EntryNode::allItems(int limit) const
{
  ItemList ret;
  if (limit == 0)
    return ret;
  if (item)
    ret.append(item);
  if (limit == 1)
    return ret;
  foreach (Base *c, children) {
    if (ret.size() >= limit)
       break;
    QList<Item *> l = static_cast<Self *>(c)->allItems(limit - ret.size());
    if (!l.isEmpty()) {
      if (limit < 0 || ret.size() + l.size() <= limit)
        ret.append(l);
      else {
        Q_ASSERT(0);
        foreach (Item *i, l) {
          ret.append(i);
          if (ret.size() >= limit)
            return ret;
        }
      }
    }
  }
  return ret;
}

Edict::ItemList
Edict::
ReadingNode::allItems(int limit) const
{
  ItemList ret;
  if (limit == 0)
    return ret;
  if (limit > 0) {
    if (limit == items.size())
      return items;
    if (limit > items.size())
      foreach (Item *item, items) {
        ret.append(item);
        if (ret.size() >= limit)
          return ret;
      }
  }
  ret = items;
  foreach (Base *c, children) {
    QList<Item *> l = static_cast<Self *>(c)->allItems(limit - ret.size());
    if (!l.isEmpty()) {
      if (limit < 0 || ret.size() + l.size() <= limit)
        ret.append(l);
      else {
        Q_ASSERT(0);
        foreach (Item *i, l) {
          ret.append(i);
          if (ret.size() >= limit)
            return ret;
        }
      }
    }
  }
  return ret;
}

// EOF
