#ifndef CATALOGWIDGET_H
#define CATALOGWIDGET_H

// catalogwidget.h
// 7/5/2011
// Use tree view to display catalog (toc) of the document.

#include "core/catalog.h"
#include <QTreeView>

class CatalogWidget : public QTreeView
{
  Q_OBJECT
  typedef CatalogWidget Self;
  typedef QTreeView Base;

  Core::Catalog *catalog_;

public:
  explicit CatalogWidget(Core::Catalog *catalog = 0, QWidget *parent = 0);

  bool bad() const { return !catalog_ || catalog_->bad(); }

signals:

public slots:

};

#endif // CATALOGWIDGET_H
