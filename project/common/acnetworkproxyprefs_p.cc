// acnetworkproxyprefs_p.cc
// 3/12/2012

#include "project/common/acnetworkproxyprefs_p.h"
#include "project/common/acsettings.h"
#include "project/common/acui.h"
#include <QtGui>
#include <QtNetwork/QNetworkProxy>

// - Construction -

AcNetworkProxyPrefs::AcNetworkProxyPrefs(AcSettings *settings, QWidget *parent)
  : Base(settings, parent)
{
  setWindowTitle(tr("Proxy"));
  createLayout();

  connect(enableButton_, SIGNAL(clicked()), SLOT(save()));
  connect(typeCombo_, SIGNAL(currentIndexChanged(int)), SLOT(save()));
  connect(portEdit_->lineEdit(), SIGNAL(editingFinished()), SLOT(save()));
  connect(hostEdit_->lineEdit(), SIGNAL(editingFinished()), SLOT(save()));
  connect(userNameEdit_->lineEdit(), SIGNAL(editingFinished()), SLOT(save()));
  connect(passwordEdit_, SIGNAL(editingFinished()), SLOT(save()));
}

void
AcNetworkProxyPrefs::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  QStringList types("Socks5");
  types.append("HTTP");
  typeCombo_ = ui->makeComboBox(AcUi::ReadOnlyHint, "", tr("Proxy type"), "", types);

  QStringList hosts("localhost");
  hostEdit_ = ui->makeComboBox(AcUi::EditHint, "", tr("Host"), "address", hosts);

  QStringList ports("9050");
  ports.append("8080");
  portEdit_ = ui->makeComboBox(AcUi::EditHint, "", tr("Port"), "port", ports);

  userNameEdit_ = ui->makeComboBox(AcUi::EditHint, "", tr("Username"), "username");
  passwordEdit_ = ui->makeLineEdit(AcUi::PasswordHint, "", tr("Password"), "password");

  enableButton_ = ui->makeToolButton(
        AcUi::CheckHint | AcUi::HighlightHint, tr("Enable"), this, SLOT(updateButtons()));

  //typeLabel_ = ui->makeLabel(AcUi::BuddyHint, tr("Type"), typeCombo_);
  hostLabel_ = ui->makeLabel(AcUi::BuddyHint, tr("Host"), hostEdit_);
  userNameLabel_ = ui->makeLabel(AcUi::BuddyHint, tr("Username"), userNameEdit_);
  //passwordLabel_ = ui->makeLabel(AcUi::BuddyHint, tr("Password"), passwordEdit_);

  // Layouts
  QGridLayout *grid = new QGridLayout; {
    // (row, col, rowspan, colspan, alignment)
    int r, c;
    grid->addWidget(hostLabel_, r=0, c=0, Qt::AlignRight);
    grid->addWidget(hostEdit_, r, ++c, 1, 2);
    grid->addWidget(portEdit_, r, c+=2);
    grid->addWidget(typeCombo_, r, ++c);

    grid->addWidget(userNameLabel_, ++r, c=0, Qt::AlignRight);
    grid->addWidget(userNameEdit_, r, ++c, 1, 2);
    grid->addWidget(passwordEdit_, r, c+=2, 1, 2);

    grid->addWidget(enableButton_, ++r, c=4, Qt::AlignRight);
    //grid->addWidget(cancelButton, r, ++c, Qt::AlignHCenter);
    //grid->addWidget(saveButton, r, ++c, Qt::AlignHCenter);

    grid->setContentsMargins(9, 9, 9, 9);
    setContentsMargins(0, 0, 0, 0);
  } setLayout(grid);

  setTabOrder(hostEdit_, portEdit_);
  setTabOrder(portEdit_, userNameEdit_);
  setTabOrder(userNameEdit_, passwordEdit_);
}

// - Properties -

bool
AcNetworkProxyPrefs::isEnabled() const
{ return enableButton_->isChecked(); }

void
AcNetworkProxyPrefs::setHost(const QString &host)
{ hostEdit_->setEditText(host); }

void
AcNetworkProxyPrefs::setPort(int port)
{ portEdit_->setEditText(QString::number(port)); }

void
AcNetworkProxyPrefs::setUserName(const QString &userName)
{ userNameEdit_->setEditText(userName); }

void
AcNetworkProxyPrefs::setPassword(const QString &password)
{ passwordEdit_->setText(password); }

// - Actions -

void
AcNetworkProxyPrefs::updateButtons()
{
  bool t = isEnabled();
  typeCombo_->setEnabled(t);
  hostEdit_->setEnabled(t);
  portEdit_->setEnabled(t);
  userNameEdit_->setEnabled(t);
  passwordEdit_->setEnabled(t);

  //typeLabel_->setEnabled(t);
  hostLabel_->setEnabled(t);
  userNameLabel_->setEnabled(t);
  //passwordLabel_->setEnabled(t);
}

bool
AcNetworkProxyPrefs::save()
{
  bool ok = true;
  QString host = hostEdit_->currentText().trimmed();
  bool enabled = isEnabled();
  if (!enabled) {
    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
    emit message(tr("network proxy disabled"));
  } else if (host.isEmpty()) {
    emit warning("empty proxy host name");
    enabled = false;
    ok = false;
  }

  QNetworkProxy::ProxyType type;
  switch (typeCombo_->currentIndex()) {
  case SocksProxy: type = QNetworkProxy::Socks5Proxy; break;
  case HttpProxy: type = QNetworkProxy::HttpProxy; break;
  default: Q_ASSERT(0); type = QNetworkProxy::Socks5Proxy;
  }
  quint16 port = portEdit_->currentText().trimmed().toUInt();
  QString user = userNameEdit_->currentText().trimmed();
  QString pass = passwordEdit_->text();
  if (enabled) {
    QNetworkProxy proxy(type, host, port, user, pass);
    QNetworkProxy::setApplicationProxy(proxy);

    QString url = host + ":" + QString::number(port);
    if (!user.isEmpty())
      url.prepend(user + "@");
    emit message(tr("network proxy enabled") + ": " + url);
  }

  settings()->setProxyEnabled(enabled);
  settings()->setProxyHostName(host);
  settings()->setProxyType(type);
  settings()->setProxyPort(port);
  settings()->setProxyUser(user);
  settings()->setProxyPassword(pass);
  return ok;
}

void
AcNetworkProxyPrefs::load()
{
  enableButton_->setChecked(settings()->isProxyEnabled());

  ProxyType type;
  switch (settings()->proxyType()) {
  case QNetworkProxy::Socks5Proxy: type = SocksProxy; break;
  case QNetworkProxy::HttpProxy: type = HttpProxy; break;
  default: type = SocksProxy;
  }
  typeCombo_->setCurrentIndex(type);

  QString host = settings()->proxyHostName();
  if (host.isEmpty())
    host = "localhost";
  hostEdit_->setEditText(host);

  int port = settings()->proxyPort();
  if (port <= 0)
    port = 9050;
  portEdit_->setEditText(QString::number(port));

  userNameEdit_->setEditText(settings()->proxyUser());
  passwordEdit_->setText(settings()->proxyPassword());

  updateButtons();
}

// EOF

