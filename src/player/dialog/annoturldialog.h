#ifndef ANNOTURLDIALOG_H
#define ANNOTURLDIALOG_H

// annoturldialog.h
// 2/7/2012

#include "urldialog.h"

class AnnotUrlDialog : public UrlDialog
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotUrlDialog)
  typedef AnnotUrlDialog Self;
  typedef UrlDialog Base;

public:
  explicit AnnotUrlDialog(QWidget *parent = nullptr);
};

#endif // ANNOTURLDIALOG_H
