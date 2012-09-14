// edict.cc
// 9/11/2012

#include "lib/edrdg/edict.h"
#include "lib/edrdg/edict_p.h"
#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtCore/QTextStream>

#define DEBUG "edict"
#include "lib/debug/debug.h"

// - Construction -

Edict::
Dictionary::~Dictionary()
{
  if (entryIndex_) {
    entryIndex_->deleteTree();
    delete entryIndex_;
  }
  if (yomiIndex_) {
    yomiIndex_->deleteTree();
    delete yomiIndex_;
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

Edict::YomiNode*
Edict::
Dictionary::yomiIndex() const
{
  if (!yomiIndex_)
    yomiIndex_ = new YomiNode;
  return yomiIndex_;
}

// - Add -

// Edict format: entry [yomi] /detail1/detail2/detail3/.../
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
    DOUT("line =" << line);
    QStringList details = line.split('/', QString::SkipEmptyParts);
    if (details.size() < 2) {
      Q_ASSERT(0);
      DOUT("warning: invalid edict file");
      continue;
    }
    QString subject = details.takeFirst();
    QString entry, yomi;
    QRegExp re("(.*) \\[(.*)\\] ");
    if (re.exactMatch(subject)) {
      Q_ASSERT(re.captureCount() == 2);
      entry = re.cap(1);
      yomi = re.cap(2);
    } else
      entry = subject.trimmed();
    if (!entry.isEmpty() && !details.isEmpty())
      addItem(entry, yomi, details);
  }

  DOUT("exit");
}

void
Edict::
Dictionary::addItem(const QString &entry, const QString &yomi, const QStringList &details)
{
  if (entry.isEmpty())
    return;

  Item *item = new Item(entry, yomi, details);
  items_.append(item);

  {
    Node *index = entryIndex();
    ushort u;
    foreach (QChar c, entry) {
      u = c.unicode();
      auto p = index->children.find(u);
      if (p == index->children.end())
        break;
      index = p.value();
    }
    index->children[u] = new EntryNode(item);
  }

  if (!yomi.isEmpty()) {
    Node *index = yomiIndex();
    ushort u;
    foreach (QChar c, yomi) {
      u = c.unicode();
      auto p = index->children.find(u);
      if (p == index->children.end())
        break;
      index = p.value();
    }
    YomiNode *n = static_cast<YomiNode *>(index->children[u]);
    if (!n)
      index->children[u] = n = new YomiNode;
    n->items.append(item);
  }
}

// - Query -

Edict::ItemList
Edict::
Dictionary::query(const QString &t) const
{
  ItemList l1 = queryByEntry(t),
           l2 = queryByYomi(t);
  return l1.isEmpty() && l2.isEmpty() ? l1 :
         l1.isEmpty() ? l2 :
         l2.isEmpty() ? l1 :
         l1 + l2;
}

Edict::ItemList
Edict::
Dictionary::queryByEntry(const QString &entry) const
{
  DOUT("enter: entry =" << entry);

  ItemList ret;
  if (entry.isEmpty())
    return ret;

  Node *index = entryIndex();
  ushort u;
  foreach (QChar c, entry) {
    u = c.unicode();
    auto p = index->children.find(u);
    if (p == index->children.end())
      break;
    index = p.value();
  }
  if (index != entryIndex())
    ret = static_cast<EntryNode *>(index)->allItems();

  DOUT("exit: ret.size =" << ret.size());
  return ret;
}

Edict::ItemList
Edict::
Dictionary::queryByYomi(const QString &yomi) const
{
  DOUT("enter: yomi =" << yomi);

  ItemList ret;
  if (yomi.isEmpty())
    return ret;

  Node *index = yomiIndex();
  ushort u;
  foreach (QChar c, yomi) {
    u = c.unicode();
    auto p = index->children.find(u);
    if (p == index->children.end())
      break;
    index = p.value();
  }
  if (index != yomiIndex())
    ret = static_cast<YomiNode *>(index)->allItems();

  DOUT("exit: ret.size =" << ret.size());
  return ret;
}

// EOF
