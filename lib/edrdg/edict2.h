#ifndef EDICT2_H
#define EDICT2_H

// 9/15/2012
// edict2.h

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QList>

namespace Edict2 {

  struct EntryNode;
  struct ReadingNode;

  struct Item
  {
    typedef Item Self;

    QStringList entries;  // not null, unique
    QStringList readings; // nullable
    QStringList details;  // not null

  public:
    Item() { }
    Item(const QStringList &_entries, const QStringList &_readings, const QStringList &_details)
      : entries(_entries), readings(_readings), details(_details) { }
    Item(const Self &that)
      : entries(that.entries), readings(that.readings), details(that.details) { }

    Self &operator=(const Self &that)
    { entries = that.entries; readings = that.readings; details = that.details; return *this; }
  };
  typedef QList<Item *> ItemList;

  class Dictionary
  {
    Q_DISABLE_COPY(Dictionary)
    typedef Dictionary Self;

    ItemList items_;
    mutable EntryNode *entryIndex_;
    mutable ReadingNode *readingIndex_;

  public:
    Dictionary() : entryIndex_(nullptr), readingIndex_(nullptr) { }
    ~Dictionary();

  public:
    bool isEmpty() const { return items_.isEmpty(); }
    int size() const { return items_.size(); }

    void addFile(const QString &fileName);

    void addItem(const Item& item)
    { addItem(item.entries, item.readings, item.details); }

    void addItem(const QStringList &entries, const QStringList &readings, const QStringList &details);

    ItemList query(const QString &entryOrReading, int limit = -1) const;
    ItemList queryByEntry(const QString &entry, int limit = -1) const;

    ItemList queryByReading(const QString &reading, int limit = -1) const;

  protected:
    EntryNode *entryIndex() const;
    ReadingNode *readingIndex() const;
  };

} // namespace Edict2

#endif // EDICT2_H
