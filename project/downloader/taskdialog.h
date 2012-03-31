#ifndef TASKDIALOG_H
#define TASKDIALOG_H

// taskdialog.h
// 2/18/2012

#include "module/qtext/dialog.h"
#include <QStringList>

QT_FORWARD_DECLARE_CLASS(QToolButton)

class AcTextView;
typedef QtExt::Dialog TaskDialogBase;
class TaskDialog : public TaskDialogBase
{
  Q_OBJECT
  typedef TaskDialog Self;
  typedef TaskDialogBase Base;

  AcTextView *textView_;
  QToolButton *urlButton_;

signals:
  void message(const QString &text);
  void warning(const QString &text);
  void error(const QString &text);

  void urlsAdded(const QStringList &urls, bool batch);
public:
  explicit TaskDialog(QWidget *parent = 0);

public slots:
  void setText(const QString &urls);
  void addText(const QString &url);

protected slots:
  void add();
  void paste();
  void showExampleUrl();
  void clear();
private:
  void createLayout();
};

#endif // TASKDIALOG_H
