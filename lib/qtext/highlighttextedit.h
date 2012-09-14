#ifndef _QTEXT_HIGHLIGHTTEXTEDIT_H
#define _QTEXT_HIGHLIGHTTEXTEDIT_H

// highlighttextedit.h
// 6/2/2012

#include "lib/qtext/textedit.h"

namespace QtExt {

class TextHighlighter;

class HighlightTextEdit : public TextEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(HighlightTextEdit)
  typedef HighlightTextEdit Self;
  typedef TextEdit Base;

  QString selectedText_;
  TextHighlighter *highlighter_;

public:
  explicit HighlightTextEdit(QWidget *parent = nullptr)
    :  Base(parent) { init(); }

  explicit HighlightTextEdit(const QString &text, QWidget *parent = nullptr)
    :  Base(text, parent) { init(); }

  TextHighlighter *highlighter() const { return highlighter_; }

  QString selectedText() const { return selectedText_; }

public slots:
  void highlightText(const QString &t);

signals:
  void selectedTextChanged(const QString &text);

protected slots:
  void updateSelectedText();

private:
  void init();
};

} // namespace QtExt

#endif // _QTEXT_HIGHLIGHTTEXTEDIT_H
