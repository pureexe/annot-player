// acaccountprefs_p.cc
// 2/8/2012

#include "src/common/acaccountprefs_p.h"
#include "src/common/acui.h"
#include "src/common/acsettings.h"
#include <QtGui>
#include <boost/tuple/tuple.hpp>

#define EDIT_MIN_WIDTH  150

// - Constructions -

AcAccountPrefs::AcAccountPrefs(AcSettings *settings, QWidget *parent)
  : Base(settings, parent)
{
  setWindowTitle(tr("Account"));
  createLayout();
}

void
AcAccountPrefs::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

#define ADDSITE(_pref, _Pref, _name, _tip) \
  QToolButton *_pref##SiteButton = ui->makeToolButton( \
    AcUi::UrlHint | AcUi::BuddyHint, _name, _tip, this, SLOT(visit##_Pref())); \
  _pref##UsernameEdit_ = ui->makeComboBox(AcUi::EditHint, "", tr("Username"), "user@email.com"); \
    _pref##UsernameEdit_->setMinimumWidth(EDIT_MIN_WIDTH); \
  _pref##PasswordEdit_ = ui->makeLineEdit(AcUi::PasswordHint, "", tr("Password"), "password"); \
    _pref##PasswordEdit_->setMinimumWidth(EDIT_MIN_WIDTH); \
  QToolButton *_pref##ClearButton = ui->makeToolButton(AcUi::PushHint, tr("Clear")); \
  connect(_pref##ClearButton, SIGNAL(clicked()), _pref##UsernameEdit_, SLOT(clearEditText())); \
  connect(_pref##ClearButton, SIGNAL(clicked()), _pref##PasswordEdit_, SLOT(clear())); \
  connect(_pref##ClearButton, SIGNAL(clicked()), SLOT(saveIfValid())); \
  connect(_pref##UsernameEdit_->lineEdit(), SIGNAL(editingFinished()), SLOT(saveIfValid())); \
  connect(_pref##PasswordEdit_, SIGNAL(editingFinished()), SLOT(saveIfValid())); \

  ADDSITE(nicovideo, Nicovideo, tr("Nicovideo.jp"), "nicovideo.jp")
  ADDSITE(bilibili, Bilibili, tr("Bilibili.tv"), "bilibili.tv")
#undef ADDSITE

  // Layouts
  QGridLayout *grid = new QGridLayout; {
    // (row, col, rowspan, colspan, alignment)
    int r, c;
    grid->addWidget(nicovideoSiteButton, r=0, c=0);
    grid->addWidget(nicovideoUsernameEdit_, r, ++c);
    grid->addWidget(nicovideoPasswordEdit_, r, ++c);
    grid->addWidget(nicovideoClearButton, r, ++c);

    grid->addWidget(bilibiliSiteButton, ++r, c=0);
    grid->addWidget(bilibiliUsernameEdit_, r, ++c);
    grid->addWidget(bilibiliPasswordEdit_, r, ++c);
    grid->addWidget(bilibiliClearButton, r, ++c);

    grid->setContentsMargins(6, 6, 6, 6);
    setContentsMargins(0, 0, 0, 0);
  } setLayout(grid);

  // Focus
  setTabOrder(nicovideoUsernameEdit_, nicovideoPasswordEdit_);
  setTabOrder(nicovideoPasswordEdit_, bilibiliUsernameEdit_);
  setTabOrder(bilibiliUsernameEdit_, bilibiliPasswordEdit_);
  nicovideoUsernameEdit_->setFocus();
}

// - Actions -

void
AcAccountPrefs::visitNicovideo()
{ QDesktopServices::openUrl(QString("http://www.nicovideo.jp")); }

void
AcAccountPrefs::visitBilibili()
{ QDesktopServices::openUrl(QString("http://www.bilibili.tv")); }

void
AcAccountPrefs::load()
{
  QString username, password;
  boost::tie(username, password) = settings()->nicovideoAccount();
  nicovideoUsernameEdit_->setEditText(username);
  nicovideoPasswordEdit_->setText(password);

  boost::tie(username, password) = settings()->bilibiliAccount();
  bilibiliUsernameEdit_->setEditText(username);
  bilibiliPasswordEdit_->setText(password);
}

bool
AcAccountPrefs::save()
{
  QString username, password;
  username = nicovideoUsernameEdit_->currentText().trimmed();
  password = nicovideoPasswordEdit_->text().trimmed();
  settings()->setNicovideoAccount(username, password);

  username = bilibiliUsernameEdit_->currentText().trimmed();
  password = bilibiliPasswordEdit_->text().trimmed();
  settings()->setBilibiliAccount(username, password);

  return true;
}

void
AcAccountPrefs::saveIfValid()
{
  QString username, password;
  username = nicovideoUsernameEdit_->currentText().trimmed();
  password = nicovideoPasswordEdit_->text().trimmed();
  if (!(username.isEmpty() ^ password.isEmpty()))
    settings()->setNicovideoAccount(username, password);

  username = bilibiliUsernameEdit_->currentText().trimmed();
  password = bilibiliPasswordEdit_->text().trimmed();
  if (!(username.isEmpty() ^ password.isEmpty()))
    settings()->setBilibiliAccount(username, password);
}
// EOF
