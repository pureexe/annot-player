#ifndef ACLINEEDIT_H
#define ACLINEEDIT_H

// aclineedit.h
// 10/10/2011

#include <QtGui/QLineEdit>

class AcLineEdit : public QLineEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(AcLineEdit)
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

#endif // ACLINEEDIT_H
