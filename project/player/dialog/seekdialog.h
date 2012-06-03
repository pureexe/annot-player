#ifndef SEEKDIALOG_H
#define SEEKDIALOG_H

// seekdialog.h
// 8/16/2011

#include "project/common/acwindow.h"

QT_FORWARD_DECLARE_CLASS(QSpinBox)
QT_FORWARD_DECLARE_CLASS(QTimeEdit)

class SeekDialog : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(SeekDialog)
  typedef SeekDialog Self;
  typedef AcWindow Base;

  QTimeEdit *timeEdit_;

public:
  explicit SeekDialog(QWidget *parent = 0);

  qint64 time() const;

signals:
  void warning(const QString &text);
  void seekRequested(qint64 msecs);

public slots:
  void setTime(qint64 msecs);

protected slots:
  void ok();

private:
  void createLayout();
};

#endif // SEEKDIALOG_H
