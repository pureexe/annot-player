#ifndef TIMEINPUTDIALOG_H
#define TIMEINPUTDIALOG_H

// timeinputdialog.h
// 8/16/2011

#include "project/common/acwindow.h"

QT_FORWARD_DECLARE_CLASS(QTimeEdit)

class TimeInputDialog : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(TimeInputDialog)
  typedef TimeInputDialog Self;
  typedef AcWindow Base;

  QTimeEdit *timeEdit_;

public:
  explicit TimeInputDialog(QWidget *parent = 0);

  qint64 time() const;

signals:
  void warning(const QString &text);
  void timeChanged(qint64 msecs);

public slots:
  void setTime(qint64 msecs);

protected slots:
  void ok();

private:
  void createLayout();
};

#endif // TIMEINPUTDIALOG_H
