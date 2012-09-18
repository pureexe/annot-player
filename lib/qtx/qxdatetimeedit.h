#ifndef QXDATETIMEEDIT_H
#define QXDATETIMEEDIT_H

// qxdatetimeedit.h
// 5/30/2012

#include "qtx/qtxglobal.h"
#include <QtGui/QDateTimeEdit>

QTX_BEGIN_NAMESPACE

class QxDateTimeEdit : public QDateTimeEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(QxDateTimeEdit)
  typedef QxDateTimeEdit Self;
  typedef QDateTimeEdit Base;

public:
  explicit QxDateTimeEdit(QWidget *parent = nullptr)
    : Base(parent) { }

  explicit QxDateTimeEdit(const QDateTime &datetime, QWidget *parent = nullptr)
    : Base(datetime, parent) { }

  explicit QxDateTimeEdit(const QTime &time, QWidget *parent = nullptr)
    : Base(time, parent) { }

  explicit QxDateTimeEdit(const QDate &date, QWidget *parent = nullptr)
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

QTX_END_NAMESPACE

#endif // QXDATETIMEEDIT_H
