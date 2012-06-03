#ifndef _QTEXT_DATEEDIT_H
#define _QTEXT_TIMEEDIT_H

// qtext/dateedit.h
// 5/30/2012

#include <QtGui/QDateEdit>

namespace QtExt {

class DateEdit : public QDateEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(DateEdit)
  typedef DateEdit Self;
  typedef QDateEdit Base;

public:
  explicit DateEdit(QWidget *parent = 0)
    : Base(parent) { }

  explicit DateEdit(const QDate &date, QWidget *parent = 0)
    : Base(date, parent) { }

  QLineEdit *lineEdit() const { return Base::lineEdit(); }

signals:
  void returnPressed();

  // - Events -
protected:
  virtual void keyPressEvent(QKeyEvent *event);
  virtual void enterEvent(QEvent *event); ///< \override
  virtual void leaveEvent(QEvent *event); ///< \override
};

} // namespace QtExt

#endif // _QTEXT_TIMEEDIT_H
