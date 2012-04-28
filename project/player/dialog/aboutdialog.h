#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

// aboutdialog.h
// 11/13/2011

#include "project/common/acaboutdialog.h"
#include <QtCore/QString>

class AboutDialog : public AcAboutDialog
{
  Q_OBJECT
  typedef AboutDialog Self;
  typedef AcAboutDialog Base;

public:
  explicit AboutDialog(QWidget *parent = 0);
};

#endif // ABOUTDIALOG_H
