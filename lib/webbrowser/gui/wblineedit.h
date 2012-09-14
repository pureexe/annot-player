#ifndef WBLINEEDIT_H
#define WBLINEEDIT_H

// gui/wbaddresslineedit.h
// 3/31/2012

#include <QtGui/QLineEdit>

class WbLineEdit : public QLineEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(WbLineEdit)
  typedef WbLineEdit Self;
  typedef QLineEdit Base;

public:
  explicit WbLineEdit(QWidget *parent = nullptr)
    : Base(parent)
  { setAlignment(Qt::AlignLeft | Qt::AlignVCenter); }

protected:
  void keyPressEvent(QKeyEvent *event) override;
};

#endif // WBLINEEDIT_H
