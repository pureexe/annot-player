#ifndef QXDATEEDIT_H
#define QXDATEEDIT_H

// qxdateedit.h
// 5/30/2012

#include "qtx/qtxglobal.h"
#include <QtGui/QDateEdit>

QTX_BEGIN_NAMESPACE

class QxDateEdit : public QDateEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(QxDateEdit)
  typedef QxDateEdit Self;
  typedef QDateEdit Base;

public:
  explicit QxDateEdit(QWidget *parent = nullptr)
    : Base(parent) { }

  explicit QxDateEdit(const QDate &date, QWidget *parent = nullptr)
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

#endif // QXDATEEDIT_H
