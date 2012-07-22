#ifndef SEEKDIALOG_H
#define SEEKDIALOG_H

// seekdialog.h
// 8/16/2011

#include "timeinputdialog.h"

class SeekDialog : public TimeInputDialog
{
  Q_OBJECT
  Q_DISABLE_COPY(SeekDialog)
  typedef SeekDialog Self;
  typedef TimeInputDialog Base;

public:
  explicit SeekDialog(QWidget *parent = nullptr);
};

#endif // SEEKDIALOG_H
