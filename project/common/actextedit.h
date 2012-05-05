#ifndef ACTEXTEDIT_H
#define ACTEXTEDIT_H

// textedit.h
// 10/10/2011

#include <QtGui/QTextEdit>

class AcTextEdit : public QTextEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(AcTextEdit)
  typedef AcTextEdit Self;
  typedef QTextEdit Base;

public:
  explicit AcTextEdit(QWidget *parent = 0);

  bool containsPlainText(const QString &text) const;
  bool containsHtml(const QString &text) const;

  // - Events -
protected:
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override
};

#endif // ACTEXTEDIT_H
