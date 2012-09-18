#ifndef QXHIGHLIGHTTEXTEDIT_H
#define QXHIGHLIGHTTEXTEDIT_H

// qxhighlighttextedit.h
// 6/2/2012

#include "qtx/qxtextedit.h"

QTX_FORWARD_DECLARE_CLASS(QxTextHighlighter)

QTX_BEGIN_NAMESPACE
class QxHighlightTextEdit : public QxTextEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(QxHighlightTextEdit)
  typedef QxHighlightTextEdit Self;
  typedef QxTextEdit Base;

  QString selectedText_;
  QxTextHighlighter *highlighter_;

public:
  explicit QxHighlightTextEdit(QWidget *parent = nullptr)
    :  Base(parent) { init(); }

  explicit QxHighlightTextEdit(const QString &text, QWidget *parent = nullptr)
    :  Base(text, parent) { init(); }

  QxTextHighlighter *highlighter() const { return highlighter_; }

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

QTX_END_NAMESPACE

#endif // QXHIGHLIGHTTEXTEDIT_H
