#ifndef QXTIMEEDIT_H
#define QXTIMEEDIT_H

// qxtimeedit.h
// 5/30/2012

#include "qtx/qtxglobal.h"
#include <QtGui/QTimeEdit>

QTX_BEGIN_NAMESPACE

class QxTimeEdit : public QTimeEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(QxTimeEdit)
  typedef QxTimeEdit Self;
  typedef QTimeEdit Base;

public:
  explicit QxTimeEdit(QWidget *parent = nullptr)
    : Base(parent) { }

  explicit QxTimeEdit(const QTime &time, QWidget *parent = nullptr)
    : Base(time, parent) { }

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

#endif // QXTIMEEDIT_H
