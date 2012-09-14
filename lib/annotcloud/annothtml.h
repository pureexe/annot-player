#ifndef _ANNOTCLOUD_ANNOTHTML_H
#define _ANNOTCLOUD_ANNOTHTML_H

// annotcloud/annothtml.h
// 8/19/2011

#include "lib/annotcloud/annotation.h"
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QColor>
#include <QtWebKit/QWebElement>
#include <utility>

namespace AnnotCloud {

class AnnotationHtmlParser : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationHtmlParser)
  typedef AnnotationHtmlParser Self;
  typedef QObject Base;

  QString tinySize_, smallSize_, normalSize_, largeSize_, hugeSize_;

  // - Constructions -
public:
  static Self *globalInstance() { static Self g; return &g; }

  explicit AnnotationHtmlParser(QObject *parent = nullptr);

  // - Properties -
public:
  QString tinySize() const             { return tinySize_; }
  void setTinySize(const QString &px)  { tinySize_ = px; }

  QString smallSize() const            { return smallSize_; }
  void setSmallSize(const QString &px) { smallSize_ = px; }

  QString normalSize() const           { return normalSize_; }
  void setNormalSize(const QString &px){ normalSize_ = px; }

  QString largeSize() const            { return largeSize_; }
  void setLargeSize(const QString &px) { largeSize_ = px; }

  QString hugeSize() const             { return hugeSize_; }
  void setHugeSize(const QString &px)  { hugeSize_ = px; }

  // - API -
public:
  ///  Return pair of parsed HTML and cmd tags.
  // TODO: return attributes for unparsed tags as well! i.e. return std::pair<QString, QList<std::pair<QString, QStringList> > >
  std::pair<QString, QStringList> toHtml(const QString &annot) const;

  QString toHtml(const AnnotationList &l, const QString &title = QString(), bool ignorePos = false) const;

  ///  Unparse rich text to annotation.
  QString reduceHtml(const QString &html) const;

  // - Implemenations -
protected:

  // - Unparse to HTML -
  ///  Translate something like \tag[attr1]{param1}{param2}[attr2]{param3}[attr3]
  QString translate(const QString &tag,
                    const QStringList &params = QStringList(),
                    const QStringList &attrs = QStringList()) const;

  QString reduceHtmlElement(const QWebElement &e) const;

  // - Parse annotation text -

  /**
   *  Parse cmd in the beging of the text starting with '\\'.
   *  Assume \param text has been trimmed.
   *  Return parsed tag and left string if succeed, or 0 as has if failed.
   */
  static std::pair<QString, QString> parseLeadingTag(const QString &text);

  /**
   *  Return the first leading token, and the left string.
   */
  static std::pair<QString, QString> parseNextToken(const QString &text);

  // - Helpers -
  /// Parse "rgb(r,g,b)".
  static QColor parseRGBColor(const QString &rgb);

  static bool isSeparator(QChar c)
  {
    switch (c.unicode()) {
    case ' ': case '\n': case '\t':
      return true;
    default:
      return false;
    }
  }
};

} // namespace AnnotCloud

#endif // _ANNOTCLOUD_ANNOTHTML_H
