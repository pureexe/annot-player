#ifndef _QTEXT_DATETIMEEDIT_H
#define _QTEXT_DATETIMEEDIT_H

// qtext/datetimeedit.h
// 5/30/2012

#include <QtGui/QDateTimeEdit>

namespace QtExt {

class DateTimeEdit : public QDateTimeEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(DateTimeEdit)
  typedef DateTimeEdit Self;
  typedef QDateTimeEdit Base;

public:
  explicit DateTimeEdit(QWidget *parent = nullptr)
    : Base(parent) { }

  explicit DateTimeEdit(const QDateTime &datetime, QWidget *parent = nullptr)
    : Base(datetime, parent) { }

  explicit DateTimeEdit(const QTime &time, QWidget *parent = nullptr)
    : Base(time, parent) { }

  explicit DateTimeEdit(const QDate &date, QWidget *parent = nullptr)
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

#endif // _QTEXT_DATETIMEEDIT_H
