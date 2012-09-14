#ifndef AUDIODELAYDIALOG_H
#define AUDIODELAYDIALOG_H

// seekdialog.h
// 6/12/2012

#include "secondinputdialog.h"

class AudioDelayDialog : public SecondInputDialog
{
  Q_OBJECT
  Q_DISABLE_COPY(AudioDelayDialog)
  typedef AudioDelayDialog Self;
  typedef SecondInputDialog Base;

public:
  explicit AudioDelayDialog(QWidget *parent = nullptr);
};

#endif // AUDIODELAYDIALOG_H
