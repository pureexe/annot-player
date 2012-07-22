#ifndef _CORE_DOCUMENT_H
#define _CORE_DOCUMENT_H

// core/document.h
// 6/28/2011
// Describe document.

#include <poppler-qt4.h>
#include <QVector>
#include <QFileInfo>

namespace Core {
  class Page;
  class Catalog;

  namespace detail {

    template <typename DocType>
    inline QStringList
    doc_metakeys(DocType doc)
    { return DocType::__not_defined__(doc); }

    template <typename MetaType, typename DocType>
    inline MetaType
    doc_metadata(DocType doc, const QString &key)
    { return DocType::__not_defined__(doc, key); }

    template <>
    inline QStringList
    doc_metakeys<>(Poppler::Document *doc)
    { return doc? doc->infoKeys() : QStringList(); }

    template <>
    inline QString
    doc_metadata<QString>(Poppler::Document *doc, const QString &key)
    { return doc? doc->info(key) : QString(); }

    template <>
    inline QDateTime
    doc_metadata<QDateTime>(Poppler::Document *doc, const QString &key)
    { return doc? doc->date(key) : QDateTime(); }

  } // namespace detail

  namespace detail {
    template <typename DocType>
    inline qreal
    doc_dpi() { return DocType::__not_defined__; }

    template <typename DocType>
    inline qreal
    doc_dpi_x() { return doc_dpi<DocType>(); }

    template <typename DocType>
    inline qreal
    doc_dpi_y() { return doc_dpi<DocType>(); }

    template <>
    inline qreal
    doc_dpi<Poppler::Document>() { return 72.0; }

  } // namespace detail

} // namespace Core

/*
namespace Core {
  namespace detail {

    struct global
    {
      int dpix, dpiy;

      global()
        : dpix(72), dpiy(72)
    };

  } // namespace detail

  class Global {
    typedef Global Self;

    static detail::global g_;

    Global() { } // No constructor.
  public:

    ///  Horizontal resolution (dot per inch).
    static int dpiX() { return g_.dpix; }
    static void setDpiX(int x) { g_.dpix = x; }

    ///  Vertical resolution (dot per inch).
    static int dpiY() { return g_.dpiy; }
    static void setDpiY(int y) { g_.dpiy = y; }

  };
} // namespace Core
*/

namespace Core {

  // Document
  /**
   *  \brief  Document with Core information.
   *
   *  The purpose of this class is to add in the annotation support
   *  as well as reduce the implementation dependency on poppler.
   *  It is to be extended to support other implementation,
   *  as well as picture format esp. zipped manga later.
   *
   *  Naming style refers to QPrintPreviewWidget:
   *  http://doc.qt.nokia.com/latest/qprintpreviewwidget.html
   */
  class Document : public QObject {
    Q_OBJECT
    typedef Document Self;
    typedef QObject Base;

    Poppler::Document *doc_;
    QString path_;

  public:
    typedef QVector<Page*>       PageVector;
    typedef PageVector::iterator PageIterator;
    typedef PageVector::iterator page_iterator;
  private:
    mutable PageVector pages_; // cache

    mutable Catalog *catalog_; // cache

  public:
    Poppler::Document *impl() const { return doc_; }

    // Constructions:
  private:
    Document(const Self&);
  public:
    explicit Document(QObject *parent = nullptr)
      : Base(parent), doc_(nullptr), catalog_(nullptr) { }

    explicit Document(const QString &path, QObject *parent = nullptr)
     : Base(parent), doc_(nullptr), catalog_(nullptr) { open(path); }

    ~Document() { clear(); }

    /**
     *  \brief  Open the document at path.
     *
     * if succeed, the document become !bad().
     *  All page are cached on after opening.
     *  jichi: If this causes slow down, change the policy to cache each page when first required.
     */
    bool open(const QString &path);

    bool bad() const { return !doc_; }

    void clear(); ///< Delete Poppler::Document and Document::Pages

    int hash() const { return reinterpret_cast<ulong>(impl()); }

    //void close();

  public slots:
    void httpReadyRead();

    // Properties:
  public:
    QString filePath() const { return path_; }
    int pageCount() const { return doc_? doc_->numPages() : 0; }
    const PageVector& pages() const { return pages_; }
    Page *page(int i) const { return i >= 0 && i < pages_.size() ? pages_[i] : 0; }

    Catalog *catalog() const; ///< Table of content, or 0 if not have one.
    QString fileName() const { return QFileInfo(path_).fileName(); }

    // Meta information:
    //@{
    ///  \name  Metadata
  public:
    QStringList infoKeys() const
    { return detail::doc_metakeys(doc_); }

    template <typename InfoType>
    InfoType info(const QString &key) const
    { return detail::doc_metadata<InfoType>(doc_, key); }

  public:
    QString title() const { return info<QString>("Title"); }
    QString subject() const { return info<QString>("Subject"); }
    QString author() const { return info<QString>("Author"); }
    QString producer() const { return info<QString>("Producer"); }
    QDateTime created() const { return info<QDateTime>("CreationDate"); }
    QDateTime modified() const { return info<QDateTime>("ModDate"); }
    //@}

  public:
    static qreal dpiX() { return detail::doc_dpi_x<Poppler::Document>(); }
    static qreal dpiY() { return detail::doc_dpi_y<Poppler::Document>(); }

  private:
    void new_catalog_() const; // create a new catalog_ from toc
  };

} // namespace Core

#endif // _CORE_DOCUMENT_H
