#ifndef QXTEXTHIGHLIGHTER_H
#define QXTEXTHIGHLIGHTER_H

// qxtexthighlighter.h
// 8/14/2012

#include "qtx/qtxglobal.h"
#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextCharFormat>

QTX_BEGIN_NAMESPACE

class QxTextHighlighter : public QSyntaxHighlighter
{
  Q_OBJECT
  Q_DISABLE_COPY(QxTextHighlighter)
  typedef QxTextHighlighter Self;
  typedef QSyntaxHighlighter Base;

  QString highlightText_;
  QTextCharFormat highlightFormat_;

  // - Construction -
public:
  explicit QxTextHighlighter(QObject *parent = nullptr)
    : Base(parent) { init(); }
  explicit QxTextHighlighter(QTextDocument *parent)
    : Base(parent) { init(); }
  explicit QxTextHighlighter(QTextEdit *parent)
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

QTX_END_NAMESPACE

#endif // QXTEXTHIGHLIGHTER_H
