// core/page.cc
// 6/28/2011

#include "core/page.h"
#include <QtAlgorithms>

namespace Core {
  void
  Page::new_catalog_() const
  {
    if (bad())
      catalog_ = new TextRectCatalog(const_cast<Self *>(this));
    else {
      QList<Poppler::TextBox*> l = page_->textList();
      catalog_ = new TextRectCatalog(l, const_cast<Self *>(this));
      qDeleteAll(l);
    }
  }

 } // namespace Core

// EOF
