#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

// logindialog.h
// 8/14/2011

#include "module/qtext/dialog.h"

QT_FORWARD_DECLARE_CLASS(QLineEdit)
QT_FORWARD_DECLARE_CLASS(QComboBox)

typedef QtExt::Dialog LoginDialogBase;
class LoginDialog : public LoginDialogBase
{
  Q_OBJECT
  typedef LoginDialog Self;
  typedef LoginDialogBase Base;

  QComboBox *userNameEdit_;
  QLineEdit *passwordEdit_;

public:
  explicit LoginDialog(QWidget *parent = 0);

  QString userName() const;
  QString password() const;

signals:
  void loginRequested(const QString &userName, const QString &password);

public slots:
  void setUserName(const QString &userName);
  void setPassword(const QString &password);

protected slots:
  void login();

protected:
  bool containsUserName(const QString &name) const;
  //virtual void mouseDoubleClickEvent(QMouseEvent *event);
  //virtual void contextMenuEvent(QContextMenuEvent *event);
};

#endif // LOGINDIALOG_H
