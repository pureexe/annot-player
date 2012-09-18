#ifndef ACTEXTEDIT_H
#define ACTEXTEDIT_H

// textedit.h
// 10/10/2011

#include "qtx/qxhighlighttextedit.h"

typedef QxHighlightTextEdit AcTextEditBase;
class AcTextEdit : public AcTextEditBase
{
  Q_OBJECT
  Q_DISABLE_COPY(AcTextEdit)
  typedef AcTextEdit Self;
  typedef AcTextEditBase Base;

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
