#ifndef URLDIALOG_H
#define URLDIALOG_H

// urldialog.h
// 2/7/2012

#include "module/qtext/dialog.h"

QT_FORWARD_DECLARE_CLASS(QToolButton)
QT_FORWARD_DECLARE_CLASS(QComboBox)

typedef QtExt::Dialog UrlDialogBase;
class UrlDialog : public UrlDialogBase
{
  Q_OBJECT
  typedef UrlDialog Self;
  typedef UrlDialogBase Base;

  QComboBox *edit_;
  QToolButton *urlButton_, *saveButton_;

public:
  explicit UrlDialog(QWidget *parent = 0);

  QString text() const;
  bool isEmpty() const;

signals:
  void urlEntered(const QString &url, bool save);

public slots:
  void addHistory(const QString &url);
  void open();
  void paste();
  void setExampleUrl(const QString &text);
  void setText(const QString &url);
  void setSave(bool t);
  void increase();
  void decrease();

protected:
  static QString autoCompleteUrl(const QString &url);

protected slots:
  void showExampleUrl();

private:
  void createLayout();
};

#endif // URLDIALOG_H
