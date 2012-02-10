#ifndef MEDIAURLDIALOG_H
#define MEDIAURLDIALOG_H

// mediaurldialog.h
// 2/7/2012

#include "urldialog.h"

class MediaUrlDialog : public UrlDialog
{
  Q_OBJECT
  typedef MediaUrlDialog Self;
  typedef UrlDialog Base;

public:
  explicit MediaUrlDialog(QWidget *parent = 0);
};

#endif // MEDIAURLDIALOG_H
