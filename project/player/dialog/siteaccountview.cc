// siteaccountview.cc
// 2/8/2012

#include "siteaccountview.h"
#include "tr.h"
#include "logger.h"
#include "project/common/acui.h"
#include "module/annotcloud/user.h"
#include <QtGui>

using namespace AnnotCloud;
using namespace Logger;

#define EDIT_MIN_WIDTH  150

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

SiteAccountView::SiteAccountView(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  setWindowTitle(tr("Link online accounts"));

  createLayout();
}

void
SiteAccountView::createLayout()
{
  AcUi *ui = AcUi::globalInstance();
  ui->setWindowStyle(this);

#define ADDSITE(_pref, _Pref, _name, _tip) \
  QToolButton *_pref##SiteButton = ui->makeToolButton( \
    AcUi::UrlHint | AcUi::BuddyHint, _name, _tip, this, SLOT(visit##_Pref())); \
  _pref##UsernameEdit_ = ui->makeComboBox(AcUi::EditHint, "", TR(T_USERNAME), "user@email.com"); \
    _pref##UsernameEdit_->setMinimumWidth(EDIT_MIN_WIDTH); \
  _pref##PasswordEdit_ = ui->makeLineEdit(AcUi::PasswordHint, "", TR(T_PASSWORD), "password"); \
    _pref##PasswordEdit_->setMinimumWidth(EDIT_MIN_WIDTH); \
  QToolButton *_pref##ClearButton = ui->makeToolButton(AcUi::PushHint, TR(T_CLEAR)); \
  connect(_pref##ClearButton, SIGNAL(clicked()), _pref##UsernameEdit_, SLOT(clearEditText())); \
  connect(_pref##ClearButton, SIGNAL(clicked()), _pref##PasswordEdit_, SLOT(clear())); \
  connect(_pref##UsernameEdit_->lineEdit(), SIGNAL(returnPressed()), SLOT(save())); \
  connect(_pref##PasswordEdit_, SIGNAL(returnPressed()), SLOT(save()));

  ADDSITE(nicovideo, Nicovideo, tr("Nicovideo.jp"), "nicovideo.jp")
  ADDSITE(bilibili, Bilibili, tr("Bilibili.tv"), "bilibili.tv")
#undef ADDSITE

  QToolButton *saveButton = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, TR(T_SAVE), this, SLOT(save()));
  QToolButton *cancelButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_CANCEL), this, SLOT(hide()));

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

    grid->addWidget(cancelButton, ++r, c=1, Qt::AlignHCenter);
    grid->addWidget(saveButton, r, c=2, Qt::AlignHCenter);

    grid->setContentsMargins(6, 6, 6, 6);
    setContentsMargins(0, 0, 0, 0);
  } setLayout(grid);

  // Shortcuts
  connect(new QShortcut(QKeySequence("Esc"), this), SIGNAL(activated()), SLOT(hide()));
  connect(new QShortcut(QKeySequence("CTRL+W"), this), SIGNAL(activated()), SLOT(hide()));

  // Focus
  setTabOrder(nicovideoUsernameEdit_, nicovideoPasswordEdit_);
  setTabOrder(nicovideoPasswordEdit_, bilibiliUsernameEdit_);
  setTabOrder(bilibiliUsernameEdit_, bilibiliPasswordEdit_);
  nicovideoUsernameEdit_->setFocus();
}

// - Events -

void
SiteAccountView::setVisible(bool visible)
{
  if (visible && !isVisible())
    invalidateAccounts();
  Base::setVisible(visible);
}

// - Actions -

void
SiteAccountView::visitNicovideo()
{ QDesktopServices::openUrl(TR(T_URL_NICOVIDEO)); }

void
SiteAccountView::visitBilibili()
{ QDesktopServices::openUrl(TR(T_URL_BILIBILI)); }

void
SiteAccountView::setNicovideoAccount(const QString &username, const QString &password)
{
  nicovideoAccount_.username = username;
  nicovideoAccount_.password = password;
  if (isVisible())
    invalidateAccounts();
}

void
SiteAccountView::setBilibiliAccount(const QString &username, const QString &password)
{
  bilibiliAccount_.username = username;
  bilibiliAccount_.password = password;
  if (isVisible())
    invalidateAccounts();
}

void
SiteAccountView::invalidateAccounts()
{
  nicovideoUsernameEdit_->setEditText(nicovideoAccount_.username);
  nicovideoPasswordEdit_->setText(nicovideoAccount_.password);
  bilibiliUsernameEdit_->setEditText(bilibiliAccount_.username);
  bilibiliPasswordEdit_->setText(bilibiliAccount_.password);
}

void
SiteAccountView::save()
{
  hide();
  bool changed = false;
  if (nicovideoAccount_.username != nicovideoUsernameEdit_->currentText()) {
    nicovideoAccount_.username = nicovideoUsernameEdit_->currentText();
    changed = true;
  }
  if (nicovideoAccount_.password != nicovideoPasswordEdit_->text()) {
    nicovideoAccount_.password = nicovideoPasswordEdit_->text();
    changed = true;
  }
  if (bilibiliAccount_.username != bilibiliUsernameEdit_->currentText()) {
    bilibiliAccount_.username = bilibiliUsernameEdit_->currentText();
    changed = true;
  }
  if (bilibiliAccount_.password != bilibiliPasswordEdit_->text()) {
    bilibiliAccount_.password = bilibiliPasswordEdit_->text();
    changed = true;
  }

  if (changed)
    emit accountChanged();
}

// EOF
