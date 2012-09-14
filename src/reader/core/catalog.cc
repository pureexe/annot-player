// core/catalog.cc
// 7/5/2011

#include "core/catalog.h"

namespace Core {

  /**
   * class CatalogItem
   */
  CatalogItem*
  CatalogItem::child(int i) const
  {
    Self *child = nullptr;
    if (children_.contains(i))
      child = children_[i];
    else if (i >= 0 && i < node_children_.size()) {
      child = new Self(node_children_.item(i), i,
                       const_cast<Self *>(this));
      children_[i] = child;
    }
    return child;
  }

  /**
   * class CatalogModel
   */

  int
  CatalogModel::columnCount(const QModelIndex &parent) const
  { Q_UNUSED(parent) return 3; }

  int
  CatalogModel::rowCount(const QModelIndex &parent) const
  {
    if (bad() || parent.column() > 0)
      return 0;

    CatalogItem *parentItem = parent.isValid()? static_cast<CatalogItem *>(parent.internalPointer())
                                              : root_;
    Q_ASSERT(parentItem);
    return parentItem->degree();
  }

  QModelIndex
  CatalogModel::parent(const QModelIndex &child) const
  {
    if (bad() || !child.isValid())
      return QModelIndex();

    CatalogItem *childItem = static_cast<CatalogItem *>(child.internalPointer());
    Q_ASSERT(childItem);
    CatalogItem *parentItem = childItem->parent();

    if (!parentItem || parentItem == root_)
      return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
  }

  QVariant
  CatalogModel::data(const QModelIndex &index, int role) const
  {
    if (bad() || !index.isValid() || role != Qt::DisplayRole)
      return QVariant();

    CatalogItem *item = static_cast<CatalogItem *>(index.internalPointer());
    Q_ASSERT(item);
    QDomNode node = item->node();

    // TODO: FIXME on these output, and links
    switch (index.column()) {
    case 0:
      return node.nodeName();
    case 2:
      return node.nodeValue().split("\n").join(" ");
    case 1:
      {
        QStringList attributes;
        QDomNamedNodeMap attributeMap = node.attributes();
        for (int i = 0; i < attributeMap.count(); ++i) {
          QDomNode attribute = attributeMap.item(i);
          attributes << attribute.nodeName() + "=\""
                        +attribute.nodeValue() + "\"";
        }
        return attributes.join(" ");
      }
    default:
      return QVariant();
    }
  }

  Qt::ItemFlags
  CatalogModel::flags(const QModelIndex &index) const
  {
    return bad() || !index.isValid() ? Qt::ItemFlags(0) // Qt::NoItemFlags
         : Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  }

  QVariant
  CatalogModel::headerData(int section, Qt::Orientation orientation, int role) const
  {
    if (!bad() && orientation == Qt::Horizontal && role == Qt::DisplayRole)
      switch (section) {
      case 0:
        return tr("Name");
      case 1:
        return tr("Attributes");
      case 2:
        return tr("Value");
      }

    return QVariant();
  }

  QModelIndex
  CatalogModel::index(int row, int column, const QModelIndex &parent) const
  {
    if (bad() || !hasIndex(row, column, parent))
      return QModelIndex();

    CatalogItem *parentItem = parent.isValid()? static_cast<CatalogItem *>(parent.internalPointer())
                                              : root_;
    Q_ASSERT(parentItem);
    CatalogItem *childItem = parentItem->child(row);
    if (childItem)
      return createIndex(row, column, childItem);
    else
      return QModelIndex();
  }
 ;

} // namespace Core

// EOF
