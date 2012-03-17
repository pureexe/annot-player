// siteaccountview.cc
// 2/8/2012

#include "siteaccountview.h"
#include "uistyle.h"
#include "tr.h"
#include "stylesheet.h"
#include "lineedit.h"
#include "comboedit.h"
#include "logger.h"
#include "module/annotcloud/user.h"
#include "module/qtext/toolbutton.h"
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
  UiStyle::globalInstance()->setWindowStyle(this);

  createLayout();
}

void
SiteAccountView::createLayout()
{
#define ADDSITE(_pref, _Pref, _name, _tip) \
  QToolButton *_pref##SiteButton = new QtExt::ToolButton; { \
    _pref##SiteButton->setStyleSheet(SS_TOOLBUTTON_TEXT); \
    _pref##SiteButton->setToolButtonStyle(Qt::ToolButtonTextOnly); \
    _pref##SiteButton->setText(_name + ":"); \
    _pref##SiteButton->setToolTip(_tip); \
  } connect(_pref##SiteButton, SIGNAL(clicked()), SLOT(visit##_Pref())); \
  _pref##UsernameEdit_ = new ComboEdit; { \
    _pref##UsernameEdit_->setToolTip(TR(T_TOOLTIP_USERNAME)); \
    _pref##UsernameEdit_->setMinimumWidth(EDIT_MIN_WIDTH); \
  } \
  _pref##PasswordEdit_ = new LineEdit; { \
    _pref##PasswordEdit_->setToolTip(TR(T_TOOLTIP_PASSWORD)); \
    _pref##PasswordEdit_->setEchoMode(QLineEdit::Password); \
    _pref##PasswordEdit_->setMinimumWidth(EDIT_MIN_WIDTH); \
  } \
  QToolButton *_pref##ClearButton = new QtExt::ToolButton; { \
    _pref##ClearButton->setStyleSheet(SS_TOOLBUTTON_TEXT); \
    _pref##ClearButton->setToolButtonStyle(Qt::ToolButtonTextOnly); \
    _pref##ClearButton->setText(TR(T_CLEAR)); \
    _pref##ClearButton->setToolTip(TR(T_CLEAR)); \
  } \
  connect(_pref##ClearButton, SIGNAL(clicked()), _pref##UsernameEdit_, SLOT(clearEditText())); \
  connect(_pref##ClearButton, SIGNAL(clicked()), _pref##PasswordEdit_, SLOT(clear())); \
  connect(_pref##UsernameEdit_->lineEdit(), SIGNAL(returnPressed()), SLOT(save())); \
  connect(_pref##PasswordEdit_, SIGNAL(returnPressed()), SLOT(save()));

  ADDSITE(nicovideo, Nicovideo, tr("Nicovideo.jp"), "nicovideo.jp")
  ADDSITE(bilibili, Bilibili, tr("Bilibili.tv"), "bilibili.tv")
#undef ADDSITE

  QToolButton *saveButton = new QtExt::ToolButton; {
    saveButton->setStyleSheet(SS_TOOLBUTTON_TEXT_HIGHLIGHT);
    saveButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    saveButton->setText(QString("[ %1 ]").arg(TR(T_SAVE)));
    saveButton->setToolTip(TR(T_SAVE));
  } connect(saveButton, SIGNAL(clicked()), SLOT(save()));
  QToolButton *cancelButton = new QtExt::ToolButton; {
    cancelButton->setStyleSheet(SS_TOOLBUTTON_TEXT);
    cancelButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    cancelButton->setText(QString("[ %1 ]").arg(TR(T_CANCEL)));
    cancelButton->setToolTip(TR(T_CANCEL));
  } connect(cancelButton, SIGNAL(clicked()), SLOT(hide()));

  //QLabel *websiteLabel = new QLabel; {
  //  websiteLabel->setStyleSheet(SS_LABEL);
  //  websiteLabel->setText(TR(T_WEBSITE));
  //  websiteLabel->setToolTip(TR(T_WEBSITE));
  //}
  //QLabel *usernameLabel = new QLabel; {
  //  usernameLabel->setStyleSheet(SS_LABEL);
  //  usernameLabel->setText(TR(T_LABEL_USERNAME));
  //  usernameLabel->setToolTip(TR(T_TOOLTIP_USERNAME));
  //}
  //QLabel *passwordLabel = new QLabel; {
  //  passwordLabel->setStyleSheet(SS_LABEL);
  //  passwordLabel->setText(TR(T_LABEL_PASSWORD));
  //  passwordLabel->setToolTip(TR(T_TOOLTIP_PASSWORD));
  //}

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
  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));
  QShortcut *closeShortcut = new QShortcut(QKeySequence::Close, this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(hide()));

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
