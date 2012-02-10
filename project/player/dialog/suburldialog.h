#ifndef SUBURLDIALOG_H
#define SUBURLDIALOG_H

// suburldialog.h
// 2/7/2012

#include "module/qtext/dialog.h"

QT_FORWARD_DECLARE_CLASS(QToolButton)
QT_FORWARD_DECLARE_CLASS(QComboBox)

typedef QtExt::Dialog SubUrlDialogBase;
class SubUrlDialog : public SubUrlDialogBase
{
  Q_OBJECT
  typedef SubUrlDialog Self;
  typedef SubUrlDialogBase Base;

  QComboBox *edit_;
  QToolButton *urlButton_, *saveButton_;

public:
  explicit SubUrlDialog(QWidget *parent = 0);

signals:
  void urlEntered(const QString &url, bool save);

public slots:
  void open();
  void paste();
  void setExampleUrl(const QString &text);

protected:
  static QString autoCompleteUrl(const QString &url);

protected slots:
  void showExampleUrl();

private:
  void createLayout();
};

#endif // SUBURLDIALOG_H
