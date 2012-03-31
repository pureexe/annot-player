#ifndef _AC_ACLINEEDIT_H
#define _AC_ACLINEEDIT_H

// ac/aclineedit.h
// 10/10/2011

#include <QLineEdit>

QT_FORWARD_DECLARE_CLASS(QMenu)

class AcLineEdit : public QLineEdit
{
  Q_OBJECT
  typedef AcLineEdit Self;
  typedef QLineEdit Base;

public:
  explicit AcLineEdit(QWidget *parent = 0);

  // - Events -
protected:
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override

private:
  QMenu *contextMenu_;
};

#endif // _AC_ACLINEEDIT_H
