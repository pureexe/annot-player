#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

// aboutdialog.h
// 11/13/2011

#include "core/gui/dialog.h"
#include <QString>

QT_FORWARD_DECLARE_CLASS(QTextEdit)

typedef Core::Gui::Dialog AboutDialogBase;
class AboutDialog : public AboutDialogBase
{
  Q_OBJECT
  typedef AboutDialog Self;
  typedef AboutDialogBase Base;

  QTextEdit *textEdit_;

public:
  explicit AboutDialog(QWidget *parent = 0);

  static QString text();

protected slots:
  void ok();
};

#endif // ABOUTDIALOG_H
