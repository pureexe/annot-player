#ifndef _QTEXT_TEXTHIGHLIGHTER_H
#define _QTEXT_TEXTHIGHLIGHTER_H

// texthighlighter.h
// 8/14/2012

#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextCharFormat>

namespace QtExt {

class TextHighlighter : public QSyntaxHighlighter
{
  Q_OBJECT
  Q_DISABLE_COPY(TextHighlighter)
  typedef TextHighlighter Self;
  typedef QSyntaxHighlighter Base;

  QString highlightText_;
  QTextCharFormat highlightFormat_;

  // - Construction -
public:
  explicit TextHighlighter(QObject *parent = nullptr)
    : Base(parent) { init(); }
  explicit TextHighlighter(QTextDocument *parent)
    : Base(parent) { init(); }
  explicit TextHighlighter(QTextEdit *parent)
    : Base(parent) { init(); }

private:
  void init();

  // - Properties -
public:
  const QTextCharFormat &highlightFormat() const
  { return highlightFormat_; }
  QTextCharFormat &highlightFormat()
  { return highlightFormat_; }
  void setHighlightFormat(const QTextCharFormat &fmt)
  { highlightFormat_ = fmt; }

  QString highlightText() const { return highlightText_; }
public slots:
  void setHighlightText(const QString &t);

  // - Actions -
protected:
  void highlightBlock(const QString &text) override;
};

} // namespace QtExt

#endif // _QTEXT_TEXTHIGHLIGHTER_H
