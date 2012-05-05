#ifndef TASKDIALOG_H
#define TASKDIALOG_H

// taskdialog.h
// 2/18/2012

#include "project/common/acwindow.h"
#include <QtCore/QStringList>

QT_FORWARD_DECLARE_CLASS(QToolButton)

class AcTextView;
class TaskDialog : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(TaskDialog)
  typedef TaskDialog Self;
  typedef AcWindow Base;

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
  void increase();
  void decrease();
  void add();
  void paste();
  void showExampleUrl();
  void clear();
protected:
  QString lastUrl() const;
private:
  void createLayout();
};

#endif // TASKDIALOG_H
