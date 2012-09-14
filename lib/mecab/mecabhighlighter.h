#ifndef MECABHIGHLIGHTER_H
#define MECABHIGHLIGHTER_H

// 9/3/2012
// mecabhighlighter.h

#include <QtGui/QSyntaxHighlighter>

namespace MeCab { class Tagger; }

class MeCabHighlighter : public QSyntaxHighlighter
{
  Q_OBJECT
  Q_DISABLE_COPY(MeCabHighlighter)
  typedef MeCabHighlighter Self;
  typedef QSyntaxHighlighter Base;

  mutable MeCab::Tagger *tagger_;

  // - Construction -
public:
  explicit MeCabHighlighter(QObject *parent)
    : Base(parent), tagger_(nullptr) { }
  explicit MeCabHighlighter(QTextDocument *parent)
    : Base(parent), tagger_(nullptr) { }
  explicit MeCabHighlighter(QTextEdit *parent)
    : Base(parent), tagger_(nullptr) { }

  ~MeCabHighlighter();

  // - Properties -
protected:
  MeCab::Tagger *tagger() const;

  // - Implementations -
protected:
  void highlightBlock(const QString &text) override;
};

#endif // MECABHIGHLIGHTER_H
