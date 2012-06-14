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
  explicit DateTimeEdit(QWidget *parent = 0)
    : Base(parent) { }

  explicit DateTimeEdit(const QDateTime &datetime, QWidget *parent = 0)
    : Base(datetime, parent) { }

  explicit DateTimeEdit(const QTime &time, QWidget *parent = 0)
    : Base(time, parent) { }

  explicit DateTimeEdit(const QDate &date, QWidget *parent = 0)
    : Base(date, parent) { }

  QLineEdit *lineEdit() const { return Base::lineEdit(); }

signals:
  void returnPressed();

  // - Events -
protected:
  virtual void keyPressEvent(QKeyEvent *event);
  virtual void enterEvent(QEvent *event); ///< \reimp
  virtual void leaveEvent(QEvent *event); ///< \reimp
};

} // namespace QtExt

#endif // _QTEXT_DATETIMEEDIT_H
