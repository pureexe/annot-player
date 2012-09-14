#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

// logindialog.h
// 8/14/2011

#include "src/common/acwindow.h"

QT_FORWARD_DECLARE_CLASS(QLineEdit)
QT_FORWARD_DECLARE_CLASS(QComboBox)

class LoginDialog : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(LoginDialog)
  typedef LoginDialog Self;
  typedef AcWindow Base;

  QComboBox *userNameEdit_;
  QLineEdit *passwordEdit_;

public:
  explicit LoginDialog(QWidget *parent = nullptr);

  QString userName() const;
  QString password() const;

signals:
  void warning(const QString &text);
  void loginRequested(const QString &userName, const QString &password);

public slots:
  void setUserName(const QString &userName);
  void setPassword(const QString &password);

  void clearUserName() { setUserName(QString()); }
  void clearPassword() { setPassword(QString()); }

protected slots:
  void login();

protected:
  bool containsUserName(const QString &name) const;
  //void mouseDoubleClickEvent(QMouseEvent *event) override;
  //void contextMenuEvent(QContextMenuEvent *event) override;

private:
  void createLayout();
};

#endif // LOGINDIALOG_H
