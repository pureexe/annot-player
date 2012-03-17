// networkproxydialog.cc
// 3/12/2012

#include "networkproxydialog.h"
#include "uistyle.h"
#include "tr.h"
#include "stylesheet.h"
#include "lineedit.h"
#include "comboedit.h"
#include "logger.h"
#include "settings.h"
#include "module/qtext/combobox.h"
#include "module/qtext/toolbutton.h"
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
  UiStyle::globalInstance()->setWindowStyle(this);

  createLayout();

  hostEdit_->setFocus();
}

void
NetworkProxyDialog::createLayout()
{
#define MAKE_BUTTON(_button, _title, _tip, _slot) \
    _button = new QtExt::ToolButton; { \
      _button->setStyleSheet(SS_TOOLBUTTON_TEXT); \
      _button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
      _button->setText(QString("[ %1 ]").arg(_title)); \
      _button->setToolTip(_tip); \
    } connect(_button, SIGNAL(clicked()), _slot);
#define MAKE_TOGGLER(_button, _title, _tip, _slot) \
    _button = new QtExt::ToolButton; { \
      _button->setStyleSheet(SS_TOOLBUTTON_TEXT_CHECKABLE); \
      _button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
      _button->setText(QString("| %1 |").arg(_title)); \
      _button->setToolTip(_tip); \
      _button->setCheckable(true); \
    } connect(_button, SIGNAL(clicked(bool)), _slot);

#define MAKE_BUDDY(_label, _title, _tip, _buddy) \
  _label = new QLabel; { \
    _label->setStyleSheet(SS_LABEL); \
    _label->setText(_title + ":"); \
    _label->setToolTip(_tip); \
    _label->setBuddy(_buddy); \
  }

  // typeCombo_
  typeCombo_ = new QtExt::ComboBox; {
    UiStyle::globalInstance()->setComboBoxStyle(typeCombo_);
    typeCombo_->setEditable(false);
    //typeCombo_->setMaximumWidth(MOVESTYLECOMBOBOX_WIDTH);
    //typeCombo_->setMinimumWidth(MOVESTYLECOMBOBOX_WIDTH);
    typeCombo_->setToolTip(tr("Proxy type"));

    // Must be consistent with ProxyType
    typeCombo_->addItem("Socks5");
    typeCombo_->addItem("HTTP");
  } //connect(typeCombo_, SIGNAL(activated(int)), SLOT(setProxyType(int)));

  QStringList hosts("localhost");
  hostEdit_ = new ComboEdit(hosts); {
    hostEdit_->setToolTip(tr("Host"));
  }

  QStringList ports("8080");
  portEdit_ = new ComboEdit(ports); {
    portEdit_->setToolTip(tr("Port"));
  }
  userNameEdit_ = new ComboEdit; {
    userNameEdit_->setToolTip(tr("Username"));
  }
  passwordEdit_ = new LineEdit; {
    passwordEdit_->setToolTip(tr("Password"));
    passwordEdit_->setEchoMode(QLineEdit::Password);
  }

  QToolButton *MAKE_BUTTON(cancelButton, TR(T_CANCEL), TR(T_CANCEL), SLOT(hide()))
  QToolButton *MAKE_BUTTON(saveButton, TR(T_SAVE), TR(T_SAVE), SLOT(save()))
  saveButton->setStyleSheet(SS_TOOLBUTTON_TEXT_HIGHLIGHT);

  MAKE_TOGGLER(enableButton_, TR(T_ENABLE), TR(T_ENABLE), SLOT(invalidateButtons()));

  MAKE_BUDDY(typeLabel_, tr("Type"), tr("Type"), typeCombo_)
  MAKE_BUDDY(hostLabel_, tr("Host"), tr("Host"), hostEdit_)
  MAKE_BUDDY(userNameLabel_, tr("Username"), tr("Username"), userNameEdit_)
  MAKE_BUDDY(passwordLabel_, tr("Password"), tr("Password"), passwordEdit_)

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

#undef MAKE_BUTTON
#undef MAKE_TOGGLER
#undef MAKE_BUDDY

  // Shortcuts
  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));
  QShortcut *closeShortcut = new QShortcut(QKeySequence::Close, this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(hide()));
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
  Settings *s = Settings::globalInstance();

  enableButton_->setChecked(s->isProxyEnabled());

  ProxyType type;
  switch (s->proxyType()) {
  case QNetworkProxy::Socks5Proxy: type = SocksProxy; break;
  case QNetworkProxy::HttpProxy: type = HttpProxy; break;
  default: type = SocksProxy; break;
  }
  typeCombo_->setCurrentIndex(type);

  QString host = s->proxyHostName();
  if (host.isEmpty())
    host = "localhost";
  hostEdit_->setEditText(host);

  int port = s->proxyPort();
  if (port <= 0)
    port = 8080;
  portEdit_->setEditText(QString::number(port));

  userNameEdit_->setEditText(s->proxyUser());
  passwordEdit_->setText(s->proxyPassword());

  invalidateButtons();
}

void
NetworkProxyDialog::invalidateButtons()
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
  if (!isEnabled()) {
    hide();
    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
    log(tr("network proxy disabled"));
  } else {
    QString host = hostEdit_->currentText().trimmed();
    if (host.isEmpty()) {
      warn("empty proxy host name");
      return;
    }
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
    QNetworkProxy proxy(type, host, port, user, pass);
    QNetworkProxy::setApplicationProxy(proxy);

    QString url = host + ":" + QString::number(port);
    if (!user.isEmpty())
      url.prepend(user + "@");
    log(tr("network proxy enabled") + ": " + url);
  }
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
