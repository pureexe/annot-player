#ifndef COUNTDOWNDIALOG_H
#define COUNTDOWNDIALOG_H

// countdowndialog.h
// 3/13/2012

#include "module/qtext/dialog.h"

QT_FORWARD_DECLARE_CLASS(QLabel)
QT_FORWARD_DECLARE_CLASS(QTimer)

typedef QtExt::Dialog CountdownDialogBase;
class CountdownDialog : public CountdownDialogBase
{
  Q_OBJECT
  typedef CountdownDialog Self;
  typedef CountdownDialogBase Base;

  QLabel *messageLabel_;
  QTimer *timer_; // in seconds
  int count_, currentCount_;
  QString message_;

public:
  enum { DefaultCount = 30 }; ///< in second

  explicit CountdownDialog(QWidget *parent = 0);

signals:
  void timeout();

public slots:
  void setCount(int count) { count_ = count; }
  void setText(const QString &text) { message_ = text; } ///< must contain %1

  virtual void setVisible(bool visible); ///< \override

protected slots:
  virtual void run() { emit timeout(); }
  void start();
  void stop();

  void refresh();
  void tick();
  void cancel() { hide(); stop(); }

private:
  void createLayout();
};

#endif // COUNTDOWNDIALOG_H
