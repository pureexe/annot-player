// urldialog.cc
// 1/24/2012

#include "urldialog.h"
#include "stylesheet.h"
#include "uistyle.h"
#include "tr.h"
#include "comboedit.h"
#include "module/qtext/toolbutton.h"
#include <QtGui>

// - Constructions -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowCloseButtonHint | \
  Qt::WindowStaysOnTopHint )

UrlDialog::UrlDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  Q_ASSERT(server_);
  setWindowTitle(tr("Open URL"));
  UiStyle::globalInstance()->setWindowStyle(this);

  createLayout();
  edit_->setFocus();
}

void
UrlDialog::createLayout()
{
  edit_ = new ComboEdit; {
    edit_->setEditText("http://");
    edit_->setToolTip(tr("Enter local or Internet media URL"));
  } connect(edit_->lineEdit(), SIGNAL(returnPressed()), SLOT(open()));

  urlButton_ = new QtExt::ToolButton; {
    //urlButton_->setText(exampleUrl());
    urlButton_->setStyleSheet(SS_TOOLBUTTON_TEXT);
    urlButton_->setToolTip(tr("Click to paste the URL example"));
    urlButton_->setCheckable(true);
    urlButton_->setChecked(true);
  } connect(urlButton_, SIGNAL(clicked(bool)), SLOT(showExampleUrl()));

  QLabel *urlLabel = new QLabel; {
    urlLabel->setStyleSheet(SS_LABEL);
    urlLabel->setText(TR(T_EXAMPLE) + ":");
    urlLabel->setToolTip(TR(T_EXAMPLE));
    urlLabel->setBuddy(urlLabel);
  }

  QToolButton *openButton = new QtExt::ToolButton; {
    openButton->setStyleSheet(SS_TOOLBUTTON_TEXT);
    openButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    openButton->setText(QString("[ %1 ]").arg(TR(T_OPEN)));
    openButton->setToolTip(TR(T_OPEN));
  } connect(openButton, SIGNAL(clicked()), SLOT(open()));

  QToolButton *pasteButton = new QtExt::ToolButton; {
    pasteButton->setStyleSheet(SS_TOOLBUTTON_TEXT);
    pasteButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    pasteButton->setText(QString("[ %1 ]").arg(TR(T_PASTE)));
    pasteButton->setToolTip(TR(T_PASTE));
  } connect(pasteButton, SIGNAL(clicked()), SLOT(paste()));

  QToolButton *clearButton = new QtExt::ToolButton; {
    clearButton->setStyleSheet(SS_TOOLBUTTON_TEXT);
    clearButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    clearButton->setText(QString("[ %1 ]").arg(TR(T_CLEAR)));
    clearButton->setToolTip(TR(T_CLEAR));
  } connect(clearButton, SIGNAL(clicked()), edit_, SLOT(clear()));

  QVBoxLayout *rows = new QVBoxLayout; {
    QLayout *header = new QHBoxLayout;
    QLayout *footer = new QHBoxLayout;

    rows->addLayout(header);
    rows->addWidget(edit_);
    rows->addLayout(footer);

    header->addWidget(urlLabel);
    header->addWidget(urlButton_);

    footer->addWidget(openButton);
    footer->addWidget(pasteButton);
    footer->addWidget(clearButton);

    rows->setContentsMargins(9, 9, 9, 9);
    //setContentsMargins(0, 0, 0, 0);
  } setLayout(rows);

#undef MAKE_BUDDY

  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));
}

// - Slots -

void
UrlDialog::open()
{
  hide();
  QString url = edit_->currentText().trimmed();

  url = autoCompleteUrl(url);
  emit urlEntered(url);
}

void
UrlDialog::paste()
{
  QClipboard *clipboard = QApplication::clipboard();
  if (clipboard)
    edit_->setEditText(clipboard->text());
}

QString
UrlDialog::autoCompleteUrl(const QString &url)
{
  QString ret = url.trimmed();
  if (!url.contains("://"))
    ret = "http://" + ret;
  else if (url.startsWith("ttp://"))
    ret = "h" + ret;

  ret.replace(QRegExp("/index.html$", Qt::CaseInsensitive), "/");

  return ret;
}

void
UrlDialog::showExampleUrl()
{ edit_->setEditText(urlButton_->text()); }

void
UrlDialog::setExampleUrl(const QString &text)
{ urlButton_->setText(text); }

// EOF
