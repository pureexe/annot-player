// core/page.cc
// 6/28/2011

#include "core/page.h"

namespace Core {
  void
  Page::new_catalog_() const
  {
    if (bad())
      catalog_ = new TextRectCatalog(const_cast<Self *>(this));
    else {
      QList<Poppler::TextBox*> l = page_->textList();
      catalog_ = new TextRectCatalog(l, const_cast<Self *>(this));
      foreach (Poppler::TextBox *t, l)
        delete t;
    }
  }

 } // namespace Core

// EOF
