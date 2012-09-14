#ifndef EDICT_H
#define EDICT_H

// 9/11/2012
// edict.h

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QList>

namespace Edict {

  struct EntryNode;
  struct YomiNode;

  struct Item
  {
    typedef Item Self;

    QString entry;    // not null, unique
    QString yomi;     // nullable
    QStringList details;  // not null

  public:
    Item() { }
    Item(const QString &_entry, const QString &_yomi, const QStringList &_details)
      : entry(_entry), yomi(_yomi), details(_details) { }
    Item(const Self &that)
      : entry(that.entry), yomi(that.yomi), details(that.details) { }

    Self &operator=(const Self &that)
    { entry = that.entry; yomi = that.yomi; details = that.details; return *this; }
  };
  typedef QList<Item *> ItemList;

  class Dictionary
  {
    Q_DISABLE_COPY(Dictionary)
    typedef Dictionary Self;

    ItemList items_;
    mutable EntryNode *entryIndex_;
    mutable YomiNode *yomiIndex_;

  public:
    Dictionary() : entryIndex_(nullptr), yomiIndex_(nullptr) { }
    ~Dictionary();

  public:
    void addFile(const QString &fileName);

    void addItem(const Item& item)
    { addItem(item.entry, item.yomi, item.details); }

    void addItem(const QString &entry, const QString &yomi, const QStringList &details);

    ItemList query(const QString &entryOrYomi) const;
    ItemList queryByEntry(const QString &entry) const;

    ItemList queryByYomi(const QString &yomi) const;

  protected:
    EntryNode *entryIndex() const;
    YomiNode *yomiIndex() const;
  };

} // namespace Edit

#endif // EDICT_H
