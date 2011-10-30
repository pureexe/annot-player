// catalogwidget.cpp
// 7/5/2011

#include "catalogwidget.h"
#include <QtGui>

CatalogWidget::CatalogWidget(Core::Catalog *catalog, QWidget *parent)
  : Base(parent), catalog_(catalog)
{
  setWindowTitle(tr("Catalog"));

  if (catalog_)
    setModel(catalog_->model());
}

// EOF
