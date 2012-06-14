#ifndef SECONDINPUTDIALOG_H
#define SECONDINPUTDIALOG_H

// secondinputdialog.h
// 6/12/2012

#include "project/common/acwindow.h"

QT_FORWARD_DECLARE_CLASS(QSpinBox)

class SecondInputDialog : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(SecondInputDialog)
  typedef SecondInputDialog Self;
  typedef AcWindow Base;

  QSpinBox *timeEdit_;

public:
  explicit SecondInputDialog(QWidget *parent = 0);

  qint64 time() const; ///< in msecs

signals:
  void timeChanged(qint64 msecs);

public slots:
  void setTime(qint64 msecs);
  void resetTime() { setTime(0); }

protected slots:
  void reportTime(int seconds) { emit timeChanged(seconds * 1000); }

private:
  void createLayout();
};

#endif // SECONDINPUTDIALOG_H
