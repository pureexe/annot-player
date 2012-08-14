// mediaitemmodel.cc_
// 8/5/2012

#include "mediaitemmodel.h"
#include <QIcon>
#include <QPixmap>

MediaItemModel::MediaItemModel(int rows, int columns, QObject *parent)
  : Base(parent),
    services(QPixmap(":/images/services.png")),
    rc_(rows), cc_(columns)
{
  root_ = new QVector<Node>(rows, Node(0));
}

MediaItemModel::~MediaItemModel()
{ delete root_; }

QModelIndex
MediaItemModel::index(int row, int column, const QModelIndex &parent) const
{
  if (row < rc_ && row >= 0 && column < cc_ && column >= 0) {
    Node *p = static_cast<Node*>(parent.internalPointer());
    Node *n = node(row, p);
    if (n)
      return createIndex(row, column, n);
  }
  return QModelIndex();
}

QModelIndex
MediaItemModel::parent(const QModelIndex &child) const
{
  if (child.isValid()) {
    Node *n = static_cast<Node*>(child.internalPointer());
    Node *p = parent(n);
    if (p)
      return createIndex(row(p), 0, p);
  }
  return QModelIndex();
}

int
MediaItemModel::rowCount(const QModelIndex &parent) const
{
  return (parent.isValid() && parent.column() != 0) ? 0 : rc_;
}

int
MediaItemModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return cc_;
}

QVariant
MediaItemModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();
  if (role == Qt::DisplayRole)
    return "Item " + QString::number(index.row()) + ":" + QString::number(index.column());
  if (role == Qt::DecorationRole) {
    if (index.column() == 0)
      return iconProvider_.icon(QFileIconProvider::Folder);
    return iconProvider_.icon(QFileIconProvider::File);
  }
  return QVariant();
}

QVariant
MediaItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole)
    return QString::number(section);
  if (role == Qt::DecorationRole)
    return QVariant::fromValue(services);
  return Base::headerData(section, orientation, role);
}

bool
MediaItemModel::hasChildren(const QModelIndex &parent) const
{
  if (parent.isValid() && parent.column() != 0)
    return false;
  return rc_ > 0 && cc_ > 0;
}

Qt::ItemFlags
MediaItemModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return 0;
  return (Qt::ItemIsDragEnabled|Qt::ItemIsSelectable|Qt::ItemIsEnabled);
}

MediaItemModel::Node*
MediaItemModel::node(int row, Node *parent) const
{
  if (parent && !parent->children)
    parent->children = new QVector<Node>(rc_, Node(parent));
  QVector<Node> *v = parent ? parent->children : root_;
  return const_cast<Node*>(&(v->at(row)));
}

MediaItemModel::Node*
MediaItemModel::parent(Node *child) const
{
  return child ? child->parent : 0;
}

int
MediaItemModel::row(Node *node) const
{
   const Node *first = node->parent ? &(node->parent->children->at(0)) : &(root_->at(0));
   return (node - first);
}

// EOF
