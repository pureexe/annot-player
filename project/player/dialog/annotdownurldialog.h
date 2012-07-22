#ifndef ANNOTDOWNURLDIALOG_H
#define ANNOTDOWNURLDIALOG_H

// annotdownurldialog.h
// 2/7/2012

#include "urldialog.h"

class AnnotDownUrlDialog : public UrlDialog
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotDownUrlDialog)
  typedef AnnotDownUrlDialog Self;
  typedef UrlDialog Base;

public:
  explicit AnnotDownUrlDialog(QWidget *parent = nullptr);
};

#endif // ANNOTDOWNURLDIALOG_H
