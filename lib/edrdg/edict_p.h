#ifndef EDICT_P_H
#define EDICT_P_H

// 9/11/2012
// edict_p.h

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QList>

namespace Edict {

  // - Entry Item -
  struct Item;

  // - Index Node -

  struct Node
  {
    typedef Node Self;

    QHash<ushort, Self*> children;

    void deleteTree()
    {
      if (!children.isEmpty())
        foreach (Self *p, children) {
          p->deleteTree();
          delete p;
        }
    }
  };

  struct EntryNode : Node
  {
    typedef EntryNode Self;
    typedef Node Base;

    Item *item;

    explicit EntryNode(Item *_item = nullptr) : item(_item) { }

    QList<Item *> allItems() const
    {
      QList<Item *> ret;
      if (item)
        ret.append(item);
      foreach (Base *c, children) {
        QList<Item *> l = static_cast<Self *>(c)->allItems();
        if (!l.isEmpty())
          ret.append(l);
      }
      return ret;
    }
  };

  struct YomiNode : Node
  {
    typedef YomiNode Self;
    typedef Node Base;

    QList<Item *> items;

    QList<Item *> allItems() const
    {
      QList<Item *> ret = items;
      foreach (Base *c, children) {
        QList<Item *> l = static_cast<Self *>(c)->allItems();
        if (!l.isEmpty())
          ret.append(l);
      }
      return ret;
    }
  };

} // namespace Edict

#endif // EDICT_P_H
