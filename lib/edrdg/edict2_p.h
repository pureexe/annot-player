#ifndef EDICT2_P_H
#define EDICT2_P_H

// 9/15/2012
// edict2_p.h

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QList>

namespace Edict2 {

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

    QList<Item *> allItems(int limit = -1) const;
  };

  struct ReadingNode : Node
  {
    typedef ReadingNode Self;
    typedef Node Base;

    ReadingNode() { }
    explicit ReadingNode(const QList<Item *> _items) : items(_items) { }
    explicit ReadingNode(Item *_item) { items.append(_item); }

    QList<Item *> items;
    QList<Item *> allItems(int limit = -1) const;
  };

} // namespace Edict2

#endif // EDICT_P_H
