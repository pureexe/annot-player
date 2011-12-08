#ifndef _CORE_TEXT_H
#define _CORE_TEXT_H

// core/text.h
// 6/28/2011
// Describe text in document.

#include <QObject>
#include <QRectF>
#include <QList>

namespace Poppler {
  class TextBox;
} // namespace Poppler

namespace Core {

  namespace detail {
    bool rects_in_the_same_line(const QRectF &left, const QRectF &right);
    bool rects_in_the_same_block(const QRectF &left, const QRectF &right);
  } // namespace detail

  // TextRect
  /**
   *  \brief  Rect of text in the document.
   */
  class TextRect;
  typedef QList<TextRect*> TextRectList;

  class TextRect : public QRectF
  {
    typedef TextRect Self;
    typedef QRectF Base;

    TextRectList children_;
    mutable QString text_;

  public:
    enum catalog_type { t_word = 0, t_line, t_block };
  private:
    catalog_type type_;

  public:
    explicit TextRect(const QRectF &rect = QRectF(),
                      const QString &text = QString())
      : Base(rect), text_(text), type_(t_word) { }

    catalog_type type() const { return type_; }
    void setType(catalog_type t) { type_ = t; }

    void setRect(const QRectF &rect)
    { Base::setRect(rect.x(), rect.y(), rect.width(), rect.height()); }

  public:
    // String operations are expensive. Cache text_ only if needed.
    const QString& text() const
    {
      if (type_ != t_word)
        foreach (Self *child, children_)
          if (child)
            text_ += child->text();

      return text_;
    }

    int degree() const { return children_.size(); }

    TextRectList &children() { return children_; }
    const TextRectList &children() const { return children_; }
  };

  class TextRectCatalog : public QObject
  {
    Q_OBJECT
    typedef TextRectCatalog Self;
    typedef QObject Base;

    TextRectList words_;
    TextRectList lines_;
    TextRectList blocks_;

  public:
    typedef QList<Poppler::TextBox*> TextList;

    explicit TextRectCatalog(QObject *parent = 0)
      : Base(parent) { }

    explicit TextRectCatalog(const TextList &l, QObject *parent = 0)
      : Base(parent) { build(l); }

    ~TextRectCatalog()
    {
      if (!blocks_.empty())
        foreach (TextRect *p, blocks_)
          if (p)
            delete p;
    }

    void build(const TextList &l);

    void clear()
    {
      if (!blocks_.empty())
        foreach (TextRect *p, blocks_)
          delete p;
      blocks_.clear();
      lines_.clear();
      words_.clear();
    }

  public:
    const TextRectList &words() const { return words_; }
    const TextRectList &lines() const { return lines_; }
    const TextRectList &blocks() const { return blocks_; }
  };

} // namespace Core


#endif // _CORE_TEXT_H
