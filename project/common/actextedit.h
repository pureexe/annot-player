#ifndef ACTEXTEDIT_H
#define ACTEXTEDIT_H

// textedit.h
// 10/10/2011

#include "module/qtext/highlighttextedit.h"

class AcTextEdit : public QtExt::HighlightTextEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(AcTextEdit)
  typedef AcTextEdit Self;
  typedef QtExt::HighlightTextEdit Base;

public:
  explicit AcTextEdit(QWidget *parent = nullptr)
    : Base(parent) { init(); }
  explicit AcTextEdit(const QString &text, QWidget *parent = nullptr)
    : Base(text, parent) { init(); }

  bool containsPlainText(const QString &text) const;
  bool containsHtml(const QString &text) const;

private:
  void init();
};

#endif // ACTEXTEDIT_H
