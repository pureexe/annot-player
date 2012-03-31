#ifndef _AC_ACTEXTEDIT_H
#define _AC_ACTEXTEDIT_H

// textedit.h
// 10/10/2011

#include <QTextEdit>

QT_FORWARD_DECLARE_CLASS(QMenu)

class AcTextEdit : public QTextEdit
{
  Q_OBJECT
  typedef AcTextEdit Self;
  typedef QTextEdit Base;

public:
  explicit AcTextEdit(QWidget *parent = 0);

  bool containsPlainText(const QString &text) const;
  bool containsHtml(const QString &text) const;

  // - Events -
protected:
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override

private:
  QMenu *contextMenu_;
};

#endif // _AC_ACTEXTEDIT_H
