#ifndef WBLINEEDIT_H
#define WBLINEEDIT_H

// gui/wbaddresslineedit.h
// 3/31/2012

#include <QtGui/QLineEdit>

class WbLineEdit : public QLineEdit
{
  Q_OBJECT
  typedef WbLineEdit Self;
  typedef QLineEdit Base;

public:
  explicit WbLineEdit(QWidget *parent = 0)
    : Base(parent)
  { setAlignment(Qt::AlignLeft | Qt::AlignVCenter); }

protected:
  virtual void keyPressEvent(QKeyEvent *event);
};

#endif // WBLINEEDIT_H
