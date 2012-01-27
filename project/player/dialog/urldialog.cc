// urldialog.cc
// 1/24/2012

#include "urldialog.h"
#include "stylesheet.h"
#include "uistyle.h"
#include "tr.h"
#include "lineedit.h"
#include "core/gui/toolbutton.h"
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
  setWindowTitle(tr("Open Media URL"));
  UiStyle::globalInstance()->setWindowStyle(this);

  createLayout();
}

void
UrlDialog::createLayout()
{
  edit_ = new LineEdit; {
    //edit_->setText("http://");
    edit_->setToolTip(tr("Enter local or Internet media URL"));
  }
  connect(edit_, SIGNAL(returnPressed()), SLOT(open()));

  QToolButton *urlButton = new Core::Gui::ToolButton; {
    urlButton->setText(exampleUrl());
    urlButton->setStyleSheet(SS_TOOLBUTTON_TEXT);
    urlButton->setToolTip(tr("Click to paste the URL example"));
    urlButton->setCheckable(true);
    urlButton->setChecked(true);
  } connect(urlButton, SIGNAL(clicked(bool)), SLOT(showExampleUrl()));

  QLabel *urlLabel = new QLabel; {
    urlLabel->setStyleSheet(SS_LABEL);
    urlLabel->setText(TR(T_EXAMPLE) + ":");
    urlLabel->setToolTip(TR(T_EXAMPLE));
    urlLabel->setBuddy(urlLabel);
  }

  QToolButton *openButton = new Core::Gui::ToolButton; {
    openButton->setStyleSheet(SS_TOOLBUTTON_TEXT);
    openButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    openButton->setText(QString("[ %1 ]").arg(TR(T_OPEN)));
    openButton->setToolTip(TR(T_OPEN));
  } connect(openButton, SIGNAL(clicked()), SLOT(open()));

  QToolButton *pasteButton = new Core::Gui::ToolButton; {
    pasteButton->setStyleSheet(SS_TOOLBUTTON_TEXT);
    pasteButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    pasteButton->setText(QString("[ %1 ]").arg(TR(T_PASTE)));
    pasteButton->setToolTip(TR(T_PASTE));
  } connect(pasteButton, SIGNAL(clicked()), SLOT(paste()));

  QToolButton *clearButton = new Core::Gui::ToolButton; {
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
    header->addWidget(urlButton);

    footer->addWidget(openButton);
    footer->addWidget(pasteButton);
    footer->addWidget(clearButton);

    rows->setContentsMargins(5, 6, 6, 6);
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
  QString url = edit_->text().trimmed();

  url = autoCompleteUrl(url);
  emit urlEntered(url);
}

void
UrlDialog::paste()
{
  QClipboard *clipboard = QApplication::clipboard();
  if (clipboard)
    edit_->setText(clipboard->text());
}

QString
UrlDialog::autoCompleteUrl(const QString &url)
{
  if (!url.contains("://"))
    return "http://" + url;
  if (url.startsWith("ttp://"))
    return "h" + url;

  return url;
}

QString
UrlDialog::exampleUrl() const
{ return tr("http://www.youtube.com/watch?v=-DJqnomZoLk"); }

void
UrlDialog::showExampleUrl()
{ edit_->setText(exampleUrl()); }

// EOF

