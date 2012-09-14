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
  explicit DateEdit(QWidget *parent = nullptr)
    : Base(parent) { }

  explicit DateEdit(const QDate &date, QWidget *parent = nullptr)
    : Base(date, parent) { }

  QLineEdit *lineEdit() const { return Base::lineEdit(); }

signals:
  void returnPressed();

  // - Events -
protected:
  void keyPressEvent(QKeyEvent *event) override;
  void enterEvent(QEvent *event) override;
  void leaveEvent(QEvent *event) override;
};

} // namespace QtExt

#endif // _QTEXT_TIMEEDIT_H
