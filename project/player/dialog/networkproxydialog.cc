// networkproxydialog.cc
// 3/12/2012

#include "networkproxydialog.h"
#include "tr.h"
#include "logger.h"
#include "project/common/acsettings.h"
#include "project/common/acui.h"
#include <QtGui>
#include <QNetworkProxy>

using namespace Logger;

#define WINDOW_FLAGS_BASE \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowStaysOnTopHint | \
  Qt::WindowTitleHint | \
  Qt::WindowCloseButtonHint

#ifdef Q_WS_MAC
  #define WINDOW_FLAGS ( \
    Qt::FramelessWindowHint | \
    WINDOW_FLAGS_BASE )
#else
  #define WINDOW_FLAGS ( \
    WINDOW_FLAGS_BASE )
#endif // Q_WS_MAC

// - Constructions -

NetworkProxyDialog::NetworkProxyDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  setWindowTitle(TR(T_NETWORKPROXY));

  createLayout();

  hostEdit_->setFocus();
}

void
NetworkProxyDialog::createLayout()
{
  AcUi *ui = AcUi::globalInstance();
  ui->setWindowStyle(this);

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

  QToolButton *saveButton = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, TR(T_SAVE), this, SLOT(save()));
  QToolButton *cancelButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_CANCEL), this, SLOT(hide()));

  enableButton_ = ui->makeToolButton(
        AcUi::CheckHint, TR(T_ENABLE), this, SLOT(updateButtons()));

  typeLabel_ = ui->makeLabel(AcUi::BuddyHint, tr("Type"), typeCombo_);
  hostLabel_ = ui->makeLabel(AcUi::BuddyHint, tr("Host"), hostEdit_);
  userNameLabel_ = ui->makeLabel(AcUi::BuddyHint, tr("Username"), userNameEdit_);
  passwordLabel_ = ui->makeLabel(AcUi::BuddyHint, tr("Password"), passwordEdit_);

  // Layouts
  QGridLayout *grid = new QGridLayout; {
    // (row, col, rowspan, colspan, alignment)
    int r, c;
    grid->addWidget(hostLabel_, r=0, c=0);
    grid->addWidget(hostEdit_, r, ++c);
    grid->addWidget(portEdit_, r, ++c);

    grid->addWidget(typeLabel_, ++r, c=0);
    grid->addWidget(typeCombo_, r, ++c);

    grid->addWidget(userNameLabel_, ++r, c=0);
    grid->addWidget(userNameEdit_, r, ++c);

    grid->addWidget(passwordLabel_, ++r, c=0);
    grid->addWidget(passwordEdit_, r, ++c);

    grid->addWidget(enableButton_, ++r, c=0, Qt::AlignHCenter);
    grid->addWidget(cancelButton, r, ++c, Qt::AlignHCenter);
    grid->addWidget(saveButton, r, ++c, Qt::AlignHCenter);

    grid->setContentsMargins(9, 9, 9, 9);
    setContentsMargins(0, 0, 0, 0);
  } setLayout(grid);

  setTabOrder(hostEdit_, portEdit_);
  setTabOrder(portEdit_, userNameEdit_);
  setTabOrder(userNameEdit_, passwordEdit_);

  // Shortcuts
  connect(new QShortcut(QKeySequence("Esc"), this), SIGNAL(activated()), SLOT(hide()));
  connect(new QShortcut(QKeySequence("CTRL+W"), this), SIGNAL(activated()), SLOT(hide()));
}

// - Properties -

bool
NetworkProxyDialog::isEnabled() const
{ return enableButton_->isChecked(); }

void
NetworkProxyDialog::setHost(const QString &host)
{ hostEdit_->setEditText(host); }

void
NetworkProxyDialog::setPort(int port)
{ portEdit_->setEditText(QString::number(port)); }

void
NetworkProxyDialog::setUserName(const QString &userName)
{ userNameEdit_->setEditText(userName); }

void
NetworkProxyDialog::setPassword(const QString &password)
{ passwordEdit_->setText(password); }

// - Actions -

void
NetworkProxyDialog::refresh()
{
  AcSettings *s = AcSettings::globalSettings();

  enableButton_->setChecked(s->isProxyEnabled());

  ProxyType type;
  switch (s->proxyType()) {
  case QNetworkProxy::Socks5Proxy: type = SocksProxy; break;
  case QNetworkProxy::HttpProxy: type = HttpProxy; break;
  default: type = SocksProxy;
  }
  typeCombo_->setCurrentIndex(type);

  QString host = s->proxyHostName();
  if (host.isEmpty())
    host = "localhost";
  hostEdit_->setEditText(host);

  int port = s->proxyPort();
  if (port <= 0)
    port = 9050;
  portEdit_->setEditText(QString::number(port));

  userNameEdit_->setEditText(s->proxyUser());
  passwordEdit_->setText(s->proxyPassword());

  updateButtons();
}

void
NetworkProxyDialog::updateButtons()
{
  bool t = isEnabled();
  typeCombo_->setEnabled(t);
  hostEdit_->setEnabled(t);
  portEdit_->setEnabled(t);
  userNameEdit_->setEnabled(t);
  passwordEdit_->setEnabled(t);

  typeLabel_->setEnabled(t);
  hostLabel_->setEnabled(t);
  userNameLabel_->setEnabled(t);
  passwordLabel_->setEnabled(t);
}

void
NetworkProxyDialog::save()
{
  QString host = hostEdit_->currentText().trimmed();
  bool enabled = isEnabled();
  if (!enabled) {
    hide();
    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
    log(tr("network proxy disabled"));
  } else if (host.isEmpty()) {
    warn("empty proxy host name");
    enabled = false;
  } else
    hide();

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
    log(tr("network proxy enabled") + ": " + url);
  }

  AcSettings *s = AcSettings::globalSettings();
  s->setProxyEnabled(enabled);
  s->setProxyHostName(host);
  s->setProxyType(type);
  s->setProxyPort(port);
  s->setProxyUser(user);
  s->setProxyPassword(pass);
  s->sync();
}

// - Events -

void
NetworkProxyDialog::setVisible(bool visible)
{
  if (visible)
    refresh();
  Base::setVisible(visible);
}

// EOF
