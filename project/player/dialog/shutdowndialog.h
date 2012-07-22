#ifndef SHUTDOWNDIALOG_H
#define SHUTDOWNDIALOG_H

// shutdowndialog.h
// 3/13/2012

#include "countdowndialog.h"

class ShutdownDialog : public CountdownDialog
{
  Q_OBJECT
  Q_DISABLE_COPY(ShutdownDialog)
  typedef ShutdownDialog Self;
  typedef CountdownDialog Base;

public:
  explicit ShutdownDialog(QWidget *parent = nullptr);

protected:
  void run() override;
};

#endif // SHUTDOWNDIALOG_H
