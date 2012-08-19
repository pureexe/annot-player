#ifndef _CORE_CATALOG_H
#define _CORE_CATALOG_H

// core/catalog.h
// 7/5/2011
// Describe document catalog

#include <QtAlgorithms>
#include <QAbstractItemModel>
#include <QDomDocument>
#include <QHash>
#include <QStringList>

namespace Core {
  // CatalogItem
  /**
   *  \brief
   */
  class CatalogItem
  {
    typedef CatalogItem Self;
    typedef QDomNode node_type;
    typedef QDomNodeList node_list;

    node_type node_;
    mutable node_list node_children_; // cached

    int row_; // which child I am
    mutable Self *parent_;
    mutable QHash<int, Self*> children_;

  public:
    explicit CatalogItem(const node_type &node = node_type(), int row = 0, Self *parent = nullptr)
      : node_(node), row_(row), parent_(parent)
    {
      if (!node_.isNull() && node_.hasChildNodes())
        node_children_ = node_.childNodes();
    }

    ~CatalogItem()
    {
      if (!children_.isEmpty())
        qDeleteAll(children_);
    }

    int row() const { return row_; }

    node_type &node() { return node_; }
    const node_type &node() const { return node_; }

    Self *parent() { return parent_; }
    const Self *parent() const { return parent_; }

    int degree() const { return node_children_.size(); }

    Self *child(int i) const;
  };

  class CatalogModel : public QAbstractItemModel
  {
    Q_OBJECT
    typedef CatalogModel Self;
    typedef QAbstractItemModel Base;

    QDomDocument *dom_;
    CatalogItem *root_;

  public:
    const QDomDocument *impl() const { return dom_; }

    // Constructions:
  public:
    explicit CatalogModel(QDomDocument *dom = nullptr, QObject *parent = nullptr)
      : Base(parent), dom_(dom), root_(nullptr)
    { if (dom_) build(*dom_); }

    ~CatalogModel()
    { if (root_) delete root_; }

    void build(const QDomNode &node)
    {
      if (root_)
        delete root_;
      root_ = new CatalogItem(node);
    }

    void clear()
    {
      dom_ = 0;
      if (root_) {
        delete root_;
        root_ = 0;
      }
    }

    bool bad() const { return !root_; }
    int hash() const { return reinterpret_cast<ulong>(impl()); }

    // Implement base class.
    //@{
    ///  \name  Model
  public:
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    //@}
  };

  class Catalog : public QObject
  {
    Q_OBJECT
    typedef Catalog Self;
    typedef QObject Base;

    CatalogModel *model_;

  public:
    const QDomDocument *impl() const { return model_? model_->impl() : 0; }

  public:
    explicit Catalog(QDomDocument *dom = nullptr, QObject *parent = nullptr)
      : Base(parent), model_(nullptr)
    { if (dom) model_ = new CatalogModel(dom, this); }

    // Properties:
  public:
    CatalogModel *model() const { return model_; }
    bool bad() const { return !model_ || model_->bad(); }
    int hash() const { return model_? model_->hash() : 0; }

  };

} // namespace Core

#endif // _CORE_CATALOG_H
