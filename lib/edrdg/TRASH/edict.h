#ifndef EDICT_H
#define EDICT_H

// 9/11/2012
// edict.h

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QList>

namespace Edict {

  struct EntryNode;
  struct ReadingNode;

  struct Item
  {
    typedef Item Self;

    QString entry;          // not null, unique
    QString reading;        // nullable
    QStringList details;    // not null

  public:
    Item() { }
    Item(const QString &_entry, const QString &_reading, const QStringList &_details)
      : entry(_entry), reading(_reading), details(_details) { }
    Item(const Self &that)
      : entry(that.entry), reading(that.reading), details(that.details) { }

    Self &operator=(const Self &that)
    { entry = that.entry; reading = that.reading; details = that.details; return *this; }
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
    { addItem(item.entry, item.reading, item.details); }

    void addItem(const QString &entry, const QString &reading, const QStringList &details);

    ItemList query(const QString &entryOrReading, int limit = -1) const;
    ItemList queryByEntry(const QString &entry, int limit = -1) const;

    ItemList queryByReading(const QString &reading, int limit = -1) const;

  protected:
    EntryNode *entryIndex() const;
    ReadingNode *readingIndex() const;
  };

} // namespace Edict

#endif // EDICT_H
