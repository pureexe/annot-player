#ifndef _CORE_PAGE_H
#define _CORE_PAGE_H

// core/page.h
// 6/28/2011
// Describe page in document.

#include "core/text.h"
#include <poppler-qt4.h>
#include <QImage>

namespace Core {

  // Page
  /**
   *  \brief  Page of the document.
   *
   *  This class seperates the page out.
   *
   *  Note: The Poppler::Page is not owned by this class.
   *  Page is not responsible to delete Poppler::Page on destruction.
   */
  class Page : public QObject {
    Q_OBJECT
    typedef Page Self;
    typedef QObject Base;

    Poppler::Page *page_;
    int num_;
    mutable TextRectCatalog *catalog_; // cache

  public:
    Poppler::Page *impl() const { return page_; }

    // Constructions:
  private:
    Page(const Self&);
  public:
    explicit Page(QObject *parent = 0)
      : Base(parent), page_(0), num_(0), catalog_(0) { }

    explicit Page(Poppler::Page *page, int number, QObject *parent = 0)
      : Base(parent), page_(page), num_(number), catalog_(0) { }

    bool bad() const { return !page_; }
    void clear() { page_ = 0; }
    int hash() const { return reinterpret_cast<long>(impl()); }

    // Properties:
  public:

    ///< Page number in the document, starting from 0.
    int num() const { return num_; }
    QSize size() const { return page_? page_->pageSize() : QSize(); }
    QSizeF sizeF() const { return page_? page_->pageSizeF() : QSizeF(); }

    ///  Render to image. This could take a long time.
    // poppler-qt4.h:  QImage renderToImage(double xres=72.0, double yres=72.0, int x=-1, int y=-1, int w=-1, int h=-1, Rotation rotate = Rotate0) const;
    QImage toImage(qreal xres, qreal yres, int x = -1, int y = -1, int w = -1, int h = -1) const
                 //Rotation rot = Poppler::Page::Rotate0)
    { return page_? page_->renderToImage(xres, yres, x, y, w, h) : QImage(); }

    ///  Render to painter. This could take a long time.
    //bool render(QPainter *painter, double xres, double yres,
    //            int x = -1, int y = -1, int w = -1, int h = -1) const
    //{ return page_? page_->renderToPainter(painter, xres, yres, x, y, w, h) : false; }


    // Queries:
    //@{
    ///  \name  Queries
  public:
    // bool Poppler::Page::search(const QString &text, double &rectLeft, double &rectTop, double &rectRight, double &rectBottom, SearchDirection direction, SearchMode caseSensitive, Rotation rotate = Rotate0) const;
    QRectF search(const QString &text) const
    {
      if (bad())
        return QRectF();

      #ifdef Q_OS_LINUX
      // Following function is depricated, but works on old ubuntu qt-creator
      QRectF ret;
      if (page_->search(text, ret, Poppler::Page::NextResult, Poppler::Page::CaseInsensitive))
        return ret;
      #else
      qreal x1, y1, x2, y2;
      if (page_->search(text, x1, y1, x2, y2, Poppler::Page::NextResult, Poppler::Page::CaseInsensitive))
        return QRectF(x1, y1, x2 - x1, y2 - y1);
      #endif // Q_OS_LINUX
      else
        return QRectF();
    }

    const TextRectList &words() const ///< words, cached
    { if (!catalog_) new_catalog_(); return catalog_->words(); }

    const TextRectList &lines() const ///< lines, cached
    { if (!catalog_) new_catalog_(); return catalog_->lines(); }

    const TextRectList &blocks() const ///< blocks, cached
    { if (!catalog_) new_catalog_(); return catalog_->blocks(); }

    ///  Clean cached catalog for words, lines, and blocks
    void clearCache() const
    { if (!bad() && catalog_) delete catalog_; }
    private:
      void new_catalog_() const;
    //@}

  };

} // namespace Core

#endif // _CORE_PAGE_H
