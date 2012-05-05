#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

// aboutdialog.h
// 11/13/2011

#include "project/common/acabout.h"
#include <QtCore/QString>

class AboutDialog : public AcAbout
{
  Q_OBJECT
  Q_DISABLE_COPY(AboutDialog)
  typedef AboutDialog Self;
  typedef AcAbout Base;

public:
  explicit AboutDialog(QWidget *parent = 0);
};

#endif // ABOUTDIALOG_H
