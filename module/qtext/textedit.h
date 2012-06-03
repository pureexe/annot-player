#ifndef _QTEXT_TEXTEDIT_H
#define _QTEXT_TEXTEDIT_H

// qtext/textedit.h
// 6/2/2012

#include <QtGui/QTextEdit>

namespace QtExt {

class TextEdit : public QTextEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(TextEdit)
  typedef TextEdit Self;
  typedef QTextEdit Base;

public:
  explicit TextEdit(QWidget *parent = 0)
    :  Base(parent) { }

  explicit TextEdit(const QString &text, QWidget *parent = 0)
    :  Base(text, parent) { }

public slots:
  void removeLastLine();
};

} // namespace QtExt

#endif // _QTEXT_TEXTEDIT_H
