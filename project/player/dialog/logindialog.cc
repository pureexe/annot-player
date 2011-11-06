// logindialog.cc
// 8/14/2011

#include "logindialog.h"
#include "uistyle.h"
#include "tr.h"
#include "stylesheet.h"
#include "lineedit.h"
#include "logger.h"
#include "core/cloud/user.h"
#include "core/gui/toolbutton.h"
#include <QtGui>

using namespace Core::Cloud;
using namespace Logger;

// - Panel -
LoginDialog::LoginDialog(QWidget *parent)
  : Base(parent)
{
  setWindowTitle(TR(T_TITLE_LOGIN));
  UiStyle::globalInstance()->setWindowStyle(this);
  setContentsMargins(0, 0, 0, 0);

  // Widgets

  userNameEdit_ = new LineEdit; {
    userNameEdit_->setStyleSheet(SS_LINEEDIT);
    userNameEdit_->setToolTip(TR(T_TOOLTIP_USERNAME));
    userNameEdit_->setText(TR(T_DEFAULT_USERNAME));
  }

  passwordEdit_ = new LineEdit; {
    passwordEdit_->setStyleSheet(SS_LINEEDIT);
    passwordEdit_->setToolTip(TR(T_TOOLTIP_PASSWORD));
    passwordEdit_->setEchoMode(QLineEdit::Password);
    passwordEdit_->setText(TR(T_DEFAULT_PASSWORD));
  }

  QToolButton *loginButton = new Core::Gui::ToolButton; {
    loginButton->setStyleSheet(SS_TOOLBUTTON_TEXT);
    loginButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    loginButton->setText(QString("[ %1 ]").arg(TR(T_LOGIN)));
    loginButton->setToolTip(TR(T_LOGIN));
  }

  QToolButton *cancelButton = new Core::Gui::ToolButton; {
    cancelButton->setStyleSheet(SS_TOOLBUTTON_TEXT);
    cancelButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    cancelButton->setText(QString("[ %1 ]").arg(TR(T_CANCEL)));
    cancelButton->setToolTip(TR(T_CANCEL));
  }

  QLabel *userNameLabel = new QLabel; {
    userNameLabel->setBuddy(userNameEdit_);
    userNameLabel->setStyleSheet(SS_LABEL);
    userNameLabel->setText(TR(T_LABEL_USERNAME));
    userNameLabel->setToolTip(TR(T_TOOLTIP_USERNAME));
  }

  QLabel *passwordLabel = new QLabel; {
    passwordLabel->setBuddy(passwordEdit_);
    passwordLabel->setStyleSheet(SS_LABEL);
    passwordLabel->setText(TR(T_LABEL_PASSWORD));
    passwordLabel->setToolTip(TR(T_TOOLTIP_PASSWORD));
  }

  // Layouts
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *row1 = new QHBoxLayout,
                *row2 = new QHBoxLayout,
                *row3 = new QHBoxLayout;
    rows->addLayout(row1);
    rows->addLayout(row2);
    rows->addLayout(row3);

    row1->addWidget(userNameLabel);
    row1->addWidget(userNameEdit_);
    row2->addWidget(passwordLabel);
    row2->addWidget(passwordEdit_);
    row3->addWidget(loginButton);
    row3->addWidget(cancelButton);
  }
  setLayout(rows);

  setTabOrder(userNameEdit_, passwordEdit_);

  /*
  QGridLayout *layout = new QGridLayout; {
    layout->addWidget(userNameLabel, 0, 0);
    layout->addWidget(userNameEdit_, 0, 1);
    layout->addWidget(passwordLabel, 1, 0);
    layout->addWidget(passwordEdit_, 1, 1);
    layout->addWidget(loginButton, 2, 0);
    layout->addWidget(cancelButton, 2, 1);
  }
  setLayout(layout);
  */

  // Connections
  connect(cancelButton, SIGNAL(clicked()), SLOT(cancel()));
  connect(loginButton, SIGNAL(clicked()), SLOT(login()));
  connect(userNameEdit_, SIGNAL(returnPressed()), SLOT(login()));
  connect(passwordEdit_, SIGNAL(returnPressed()), SLOT(login()));

  // Focus
  userNameEdit_->setFocus();
}

// - Properties -

QString
LoginDialog::userName() const
{ return userNameEdit_->text(); }

QString
LoginDialog::password() const
{ return passwordEdit_->text(); }

void
LoginDialog::setUserName(const QString &userName)
{ userNameEdit_->setText(userName); }

void
LoginDialog::setPassword(const QString &password)
{ passwordEdit_->setText(password); }

// - Slots -

void
LoginDialog::login()
{
  QString name = userName();
  QString pass = password();

  if (!User::isValidName(name)) {
    warn(TR(T_ERROR_BAD_USERNAME));
    return;
  }

  if (!User::isValidPassword(pass)) {
    warn(TR(T_ERROR_BAD_PASSWORD));
    return;
  }

  pass = User::encryptPassword(pass);

  hide();
  emit loginRequested(name, pass);
}

void
LoginDialog::cancel()
{ hide(); }

// EOF
