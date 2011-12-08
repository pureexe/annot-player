#ifndef LINEEDIT_H
#define LINEEDIT_H

// lineedit.h
// 10/10/2011

#include <QLineEdit>

QT_FORWARD_DECLARE_CLASS(QMenu)

class LineEdit : public QLineEdit
{
  Q_OBJECT
  typedef LineEdit Self;
  typedef QLineEdit Base;

public:
  explicit LineEdit(QWidget *parent = 0);

  // - Events -
protected:
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override

private:
  QMenu *contextMenu_;
};

#endif // LINEEDIT_H
