#ifndef MEDIAITEMMODEL_H
#define MEDIAITEMMODEL_H

// mediaitemmodel.h
// 8/5/2012

#include <QtCore/QAbstractItemModel>
#include <QtCore/QVector>
#include <QtGui/QFileIconProvider>
#include <QtGui/QIcon>

class MediaItemModel : public QAbstractItemModel
{
  Q_OBJECT
  //Q_DISABLE_COPY(MediaItemModel);
  typedef MediaItemModel Self;
  typedef QAbstractItemModel Base;

public:
  MediaItemModel(int rows, int columns, QObject *parent = 0);
  ~MediaItemModel();

  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  QModelIndex parent(const QModelIndex &child) const;

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;

  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

  bool hasChildren(const QModelIndex &parent) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;

protected:
  struct Node
  {
    Node *parent;
    QVector<Node> *children;

    Node(Node *parent = nullptr) : parent(parent), children(nullptr) {}
    ~Node() { if (children) delete children; }
  };

  Node *node(int row, Node *parent) const;
  Node *parent(Node *child) const;
  int row(Node *node) const;

private:
  QIcon services;
  int rc_, cc_;
  QVector<Node> *root_;
  QFileIconProvider iconProvider_;
};

#endif // MEDIAITEMMODEL_H
