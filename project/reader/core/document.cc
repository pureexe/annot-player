// core/document.cc
// 6/28/2011

#include "core/document.h"
#include "core/page.h"
#include "core/catalog.h"

#include <QtNetwork>

namespace Core {

  /**
   * Document
   */
  QNetworkReply *reply = 0;
  void
  Document::httpReadyRead()
  {
    Q_ASSERT(0);
    doc_ = Poppler::Document::loadFromData(reply->readAll());
    if (bad())
      Q_ASSERT(0);
  }

  // http://doc.qt.nokia.com/latest/qvector.html
  bool
  Document::open(const QString &path)
  {
    clear();
    path_ = path;

    // NETWORK BEGIN
    /*
    path_ =  "http://www.openmp.org/mp-documents/OpenMP3.1.pdf";
    QNetworkAccessManager qnam;
    reply = qnam.get(QNetworkRequest(QUrl(path_)));
    connect(reply, SIGNAL(readyRead()), SLOT(httpReadyRead()));
    while(bad());
    */

    // NETWORK END

    doc_ = Poppler::Document::load(path_);
    if (bad())
      return false;

    //doc_->setRenderBackend(Poppler::Document::SplashBackend);
    doc_->setRenderHint(Poppler::Document::Antialiasing);
    doc_->setRenderHint(Poppler::Document::TextAntialiasing);
    //doc_->setRenderHint(Poppler::Document::TextHinting);
    //doc_->setRenderHint(Poppler::Document::TextSlightHinting);

    int n = doc_->numPages();
    if (n <= 0) {
      doc_ = 0;
      return false;
    }

    pages_.resize(n);
    for(int i = 0; i < pages_.size(); i++)
      pages_[i] = new Page(doc_->page(i), i, this);

    return true;
  }

  void
  Document::clear()
  {
    // Delete Poppler::Document
    if (doc_) {
      delete doc_;
      doc_ = 0;
    }

    // Delete Poppler::Page
    if (!pages_.empty()) {
      foreach (Page *, pages_)
        if (p) {
          if (p->impl())
            delete p->impl();
          delete p;
        }
      pages_.clear();
    }

    // Delete Poppler::Document::toc
    if (catalog_) {
      if (catalog_->impl())
        delete catalog_->impl();
      delete catalog_;
      catalog_ = 0;
    }

    path_ = "";
  }

  void
  Document::new_catalog_() const
  { catalog_ = new Catalog(doc_->toc()); }

  Catalog*
  Document::catalog() const
  {
    if (!bad() && !catalog_)
      new_catalog_();
    return catalog_->bad()? 0 : catalog_;
  }

} // namespace Core

// EOF
