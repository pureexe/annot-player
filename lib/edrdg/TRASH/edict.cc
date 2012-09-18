// edict.cc
// 9/11/2012

#include "lib/edrdg/edict.h"
#include "lib/edrdg/edict_p.h"
#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtCore/QTextStream>

//#define DEBUG "edict"
#include "qtx/qxdebug.h"
#include <QtCore/QDebug>

// - Construction -

Edict::
Dictionary::~Dictionary()
{
  if (entryIndex_) {
    entryIndex_->deleteTree();
    delete entryIndex_;
  }
  if (readingIndex_) {
    readingIndex_->deleteTree();
    delete readingIndex_;
  }
  if (!items_.isEmpty())
    foreach (Item *p, items_)
      delete p;
}

Edict::EntryNode*
Edict::
Dictionary::entryIndex() const
{
  if (!entryIndex_)
    entryIndex_ = new EntryNode;
  return entryIndex_;
}

Edict::ReadingNode*
Edict::
Dictionary::readingIndex() const
{
  if (!readingIndex_)
    readingIndex_ = new ReadingNode;
  return readingIndex_;
}

// - Add -

// Edict format: entry [reading] /detail1/detail2/detail3/.../
void
Edict::
Dictionary::addFile(const QString &fileName)
{
  DOUT("enter: fileName =" << fileName);
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    DOUT("exit: failed to open file to read as text");
    return;
  }

  QTextStream in(&file);
  in.setCodec("UTF-8");

  for (QString line = in.readLine(); !line.isNull(); line = in.readLine()) {
    //DOUT("line =" << line);
    QStringList details = line.split('/', QString::SkipEmptyParts);
    if (details.size() < 2) {
      Q_ASSERT(0);
      qDebug() << "edict2:addFile: warning: invalid edict file";
      continue;
    }
    QString subject = details.takeFirst();
    QString entry, reading;
    QRegExp re("(.*) \\[(.*)\\] ");
    if (re.exactMatch(subject)) {
      Q_ASSERT(re.captureCount() == 2);
      entry = re.cap(1);
      reading = re.cap(2);
    } else
      entry = subject.trimmed();
    if (!entry.isEmpty() && !details.isEmpty())
      addItem(entry, reading, details);
  }

  DOUT("exit");
}

void
Edict::
Dictionary::addItem(const QString &entry, const QString &reading, const QStringList &details)
{
  if (entry.isEmpty())
    return;

  Item *item = new Item(entry, reading, details);
  items_.append(item);

  {
    Node *index = entryIndex();
    ushort u;
    foreach (const QChar &c, entry) {
      u = c.unicode();
      auto p = index->children.find(u);
      if (p == index->children.end())
        break;
      index = p.value();
    }
    index->children[u] = new EntryNode(item);
  }

  if (!reading.isEmpty()) {
    Node *index = readingIndex();
    ushort u;
    foreach (const QChar &c, reading) {
      u = c.unicode();
      auto p = index->children.find(u);
      if (p == index->children.end())
        break;
      index = p.value();
    }
    ReadingNode *n = static_cast<ReadingNode *>(index->children[u]);
    if (!n)
      index->children[u] = n = new ReadingNode;
    n->items.append(item);
  }
}

// - Query -

Edict::ItemList
Edict::
Dictionary::query(const QString &t, int limit) const
{
  ItemList l1 = queryByEntry(t, limit);
  if (limit >= 0 && l1.size() >= limit)
    return l1;

  ItemList l2 = queryByReading(t, limit >= 0 ? limit - l1.size() : 0);
  return l1.isEmpty() && l2.isEmpty() ? l1 :
         l1.isEmpty() ? l2 :
         l2.isEmpty() ? l1 :
         l1 + l2;
}

Edict::ItemList
Edict::
Dictionary::queryByEntry(const QString &entry, int limit) const
{
  DOUT("enter: limit =" << limit << ", entry =" << entry);

  ItemList ret;
  if (entry.isEmpty())
    return ret;

  Node *index = entryIndex();
  ushort u;
  foreach (const QChar &c, entry) {
    u = c.unicode();
    auto p = index->children.find(u);
    if (p == index->children.end())
      break;
    index = p.value();
  }
  if (index != entryIndex())
    ret = static_cast<EntryNode *>(index)->allItems(limit);

  DOUT("exit: ret.size =" << ret.size());
  return ret;
}

Edict::ItemList
Edict::
Dictionary::queryByReading(const QString &reading, int limit) const
{
  DOUT("enter: limit =" << limit << ", reading =" << reading);

  ItemList ret;
  if (reading.isEmpty())
    return ret;

  Node *index = readingIndex();
  ushort u;
  foreach (const QChar &c, reading) {
    u = c.unicode();
    auto p = index->children.find(u);
    if (p == index->children.end())
      break;
    index = p.value();
  }
  if (index != readingIndex())
    ret = static_cast<ReadingNode *>(index)->allItems(limit);

  DOUT("exit: ret.size =" << ret.size());
  return ret;
}

// EOF
