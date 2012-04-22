#ifndef _AC_ACLINEEDIT_H
#define _AC_ACLINEEDIT_H

// ac/aclineedit.h
// 10/10/2011

#include <QtGui/QLineEdit>

class AcLineEdit : public QLineEdit
{
  Q_OBJECT
  typedef AcLineEdit Self;
  typedef QLineEdit Base;

public:
  explicit AcLineEdit(QWidget *parent = 0);

public slots:
  void enterText(const QString &text); // setText and emit enterPressed

  // - Events -
protected:
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override
};

#endif // _AC_ACLINEEDIT_H
