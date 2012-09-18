// edict2.cc
// 9/15/2012

#include "lib/edrdg/edict2.h"
#include "lib/edrdg/edict2_p.h"
#include "qtx/qxalgorithm.h"
#include <QtAlgorithms>
#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtCore/QTextStream>

#define DEBUG "edict"
#include "qtx/qxdebug.h"
#include <QtCore/QDebug>

// - Construction -

Edict2::
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
    qDeleteAll(items_);
}

Edict2::EntryNode*
Edict2::
Dictionary::entryIndex() const
{
  if (!entryIndex_)
    entryIndex_ = new EntryNode;
  return entryIndex_;
}

Edict2::ReadingNode*
Edict2::
Dictionary::readingIndex() const
{
  if (!readingIndex_)
    readingIndex_ = new ReadingNode;
  return readingIndex_;
}

// - Add -

// Edict format: entry [reading] /detail1/detail2/detail3/.../
void
Edict2::
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
    QStringList entries, readings;
    QRegExp re("(.*) \\[(.*)\\] ");
    if (re.exactMatch(subject)) {
      Q_ASSERT(re.captureCount() == 2);
      entry = re.cap(1);
      reading = re.cap(2);
    } else
      entry = subject.trimmed();

    if (!entry.isEmpty() && !details.isEmpty()) {
      if (!reading.isEmpty())
        readings = reading.split(';');
      foreach (const QString &e, entry.split(';')) {
        int i = e.indexOf('(');
        entries.append(
          i > 0 ? e.left(i) : e
        );
      }

      addItem(entries, readings, details);
    }
  }

  DOUT("exit");
}

void
Edict2::
Dictionary::addItem(const QStringList &entries, const QStringList &readings, const QStringList &details)
{
  if (entries.isEmpty())
    return;

  Item *item = new Item(entries, readings, details);
  items_.append(item);

  {
    Node *index = entryIndex();
    ushort u;
    int pos;
    foreach (const QString &entry, entries) {
      pos = 0;
      foreach (const QChar &c, entry) {
        pos++;
        u = c.unicode();
        auto p = index->children.find(u);
        if (p == index->children.end())
          break;
        index = p.value();
      }
      index = index->children[u] = new EntryNode(item);
      for (; pos < entry.size(); pos++) {
        u = entry[pos].unicode();
        index = index->children[u] = new EntryNode(item);
      }
    }
  }

  {
    Node *index = readingIndex();
    ushort u;
    int pos;
    foreach (const QString &reading, readings.isEmpty() ? entries : readings) {
      pos = 0;
      foreach (const QChar &c, reading) {
        pos++;
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
      if (pos < reading.size()) {
        index = n;
        for (; pos < reading.size(); pos++) {
          u = reading[pos].unicode();
          index = index->children[u] = new ReadingNode(item);
        }
      }
    }
  }
}

// - Query -

Edict2::ItemList
Edict2::
Dictionary::query(const QString &t, int limit) const
{
  ItemList l1 = queryByReading(t, limit);
  if (limit >= 0 && l1.size() >= limit)
    return l1;

  ItemList l2 = queryByEntry(t, limit >= 0 ? limit - l1.size() : 0);
  return l1.isEmpty() && l2.isEmpty() ? l1 :
         l1.isEmpty() ? l2 :
         l2.isEmpty() ? l1 :
         (l1.append(l2), qxUniqueList(l1));
}

Edict2::ItemList
Edict2::
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
  Q_ASSERT(index);
  if (index != entryIndex())
    ret = static_cast<EntryNode *>(index)->allItems(limit);

  DOUT("exit: ret.size =" << ret.size());
  return ret;
}

Edict2::ItemList
Edict2::
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
  Q_ASSERT(index);
  if (index != readingIndex())
    ret = static_cast<ReadingNode *>(index)->allItems(limit);

  DOUT("exit: ret.size =" << ret.size());
  return ret;
}

// EOF
