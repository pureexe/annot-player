// logindialog.cc
// 8/14/2011

#include "logindialog.h"
#include "tr.h"
#include "project/common/acui.h"
#include "module/annotcloud/user.h"
#include <QtGui>

#ifdef Q_OS_MAC
# define K_ENTER       "enter"
# define K_ESC         "esc"
#else
# define K_ENTER       "Enter"
# define K_ESC         "Esc"
#endif // Q_OS_MAC

using namespace AnnotCloud;

#define WINDOW_FLAGS_BASE \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowStaysOnTopHint

#ifdef Q_OS_MAC
# define WINDOW_FLAGS WINDOW_FLAGS_BASE | Qt::FramelessWindowHint
#else
# define WINDOW_FLAGS WINDOW_FLAGS_BASE | Qt::WindowTitleHint
#endif // Q_OS_MAC

// - Constructions -

LoginDialog::LoginDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  setWindowTitle(TR(T_TITLE_LOGIN));

  createLayout();

  // Focus
  userNameEdit_->setFocus();
}

void
LoginDialog::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  QStringList defaultUsers(User::guest().name());
  userNameEdit_ = ui->makeComboBox(
        AcUi::EditHint, TR(T_DEFAULT_USERNAME), TR(T_TOOLTIP_USERNAME), "username", defaultUsers);
  passwordEdit_ = ui->makeLineEdit(
        AcUi::EditHint | AcUi::PasswordHint, TR(T_DEFAULT_PASSWORD), "password", TR(T_TOOLTIP_PASSWORD));

  setTabOrder(userNameEdit_, passwordEdit_);

  QToolButton *loginButton = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, TR(T_LOGIN), TR(T_LOGIN), K_ENTER,
        this, SLOT(login()));
  QToolButton *cancelButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_CANCEL), TR(T_CANCEL), K_ESC,
        this, SLOT(fadeOut()));

  QLabel *userNameLabel = ui->makeLabel(AcUi::BuddyHint, TR(T_USERNAME), userNameEdit_),
         *passwordLabel = ui->makeLabel(AcUi::BuddyHint, TR(T_PASSWORD), passwordEdit_);

  // Layouts
  QGridLayout *grid = new QGridLayout; {
    // (row, col, rowspan, colspan, alignment)
    int r, c;
    grid->addWidget(userNameLabel, r=0, c=0);
    grid->addWidget(userNameEdit_, r, ++c);

    grid->addWidget(passwordLabel, ++r, c=0);
    grid->addWidget(passwordEdit_, r, ++c);

    grid->addWidget(cancelButton, ++r, c=0, Qt::AlignHCenter);
    grid->addWidget(loginButton, r, ++c, Qt::AlignHCenter);

    grid->setContentsMargins(9, 9, 9, 9);
    setContentsMargins(0, 0, 0, 0);
  } setLayout(grid);

  // Connections
  connect(userNameEdit_->lineEdit(), SIGNAL(returnPressed()), SLOT(login()));
  connect(passwordEdit_, SIGNAL(returnPressed()), SLOT(login()));
}

// - Properties -

QString
LoginDialog::userName() const
{ return userNameEdit_->currentText(); }

QString
LoginDialog::password() const
{ return passwordEdit_->text(); }

void
LoginDialog::setUserName(const QString &userName)
{ userNameEdit_->setEditText(userName); }

void
LoginDialog::setPassword(const QString &password)
{ passwordEdit_->setText(password); }

// - Actions -

void
LoginDialog::login()
{
  QString name = userName();
  QString pass = password();

  if (!name.trimmed().isEmpty() && !containsUserName(name))
    userNameEdit_->addItem(name);

  if (!User::isValidName(name)) {
    emit warning(TR(T_ERROR_BAD_USERNAME));
    return;
  }

  if (!User::isValidPassword(pass)) {
    emit warning(TR(T_ERROR_BAD_PASSWORD));
    return;
  }

  pass = User::encryptPassword(pass);

  fadeOut();
  emit loginRequested(name, pass);
}

bool
LoginDialog::containsUserName(const QString &name) const
{
  for (int i = 0; i < userNameEdit_->count(); i++)
    if (userNameEdit_->itemText(i) == name)
      return true;
  return false;
}

// EOF
