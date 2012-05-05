#ifndef SUBURLDIALOG_H
#define SUBURLDIALOG_H

// suburldialog.h
// 2/7/2012

#include "urldialog.h"

class SubUrlDialog : public UrlDialog
{
  Q_OBJECT
  Q_DISABLE_COPY(SubUrlDialog)
  typedef SubUrlDialog Self;
  typedef UrlDialog Base;

public:
  explicit SubUrlDialog(QWidget *parent = 0);
};

#endif // SUBURLDIALOG_H
