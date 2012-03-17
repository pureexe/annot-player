// urldialog.cc
// 1/24/2012

#include "urldialog.h"
#include "stylesheet.h"
#include "uistyle.h"
#include "tr.h"
#include "comboedit.h"
#include "module/qtext/ss.h"
#include "module/qtext/toolbutton.h"
#include "module/qtext/string.h"
#include <QtGui>

#ifdef Q_OS_MAC
  #define K_CTRL        "cmd"
#else
  #define K_CTRL        "Ctrl"
#endif // Q_OS_MAC

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
    //edit_->setEditText("http://");
    edit_->setToolTip(tr("Enter local or Internet media URL"));
  } connect(edit_->lineEdit(), SIGNAL(returnPressed()), SLOT(open()));

  urlButton_ = new QtExt::ToolButton; {
    //urlButton_->setText(exampleUrl());
    urlButton_->setStyleSheet(SS_TOOLBUTTON_TEXT_URL);
    urlButton_->setToolTip(tr("Click to paste the URL example"));
    urlButton_->setCheckable(true);
    urlButton_->setChecked(true);
  } connect(urlButton_, SIGNAL(clicked(bool)), SLOT(showExampleUrl()));

  QLabel *urlLabel = new QLabel; {
    urlLabel->setStyleSheet(SS_LABEL);
    urlLabel->setText(TR(T_EXAMPLE) + ":");
    urlLabel->setToolTip(TR(T_EXAMPLE));
    urlLabel->setBuddy(urlButton_);
  }

  QToolButton *openButton = new QtExt::ToolButton; {
    openButton->setStyleSheet(SS_TOOLBUTTON_TEXT_HIGHLIGHT);
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
  } connect(clearButton, SIGNAL(clicked()), edit_->lineEdit(), SLOT(clear()));

  QToolButton *increaseButton = new QtExt::ToolButton; {
    increaseButton->setStyleSheet(SS_TOOLBUTTON_TEXT);
    increaseButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    increaseButton->setText("+");
    increaseButton->setToolTip(TR(T_INCREASE) + " [" K_CTRL "+=]");
  } connect(increaseButton, SIGNAL(clicked()), SLOT(increase()));

  QToolButton *decreaseButton = new QtExt::ToolButton; {
    decreaseButton->setStyleSheet(SS_TOOLBUTTON_TEXT);
    decreaseButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    decreaseButton->setText("-");
    decreaseButton->setToolTip(TR(T_DECREASE) + " [" K_CTRL "+-]");
  } connect(decreaseButton, SIGNAL(clicked()), SLOT(decrease()));

  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout,
                *footer = new QHBoxLayout;
    QLayout *inc = new QVBoxLayout;
    QGridLayout *body = new QGridLayout;

    rows->addLayout(header);
    rows->addLayout(body);
    rows->addLayout(footer);

    header->addWidget(urlLabel);
    header->addWidget(urlButton_);

    body->addWidget(edit_, 0, 0, 1, 2);
    body->addLayout(inc, 0, 1, 1, 1, Qt::AlignRight);
    inc->addWidget(increaseButton);
    inc->addWidget(decreaseButton);

    footer->addWidget(clearButton);
    footer->addWidget(pasteButton);
    footer->addStretch();
    footer->addWidget(openButton);

    rows->setContentsMargins(9, 9, 9, 9);
    //setContentsMargins(0, 0, 0, 0);
  } setLayout(rows);

#undef MAKE_BUDDY

  setStyleSheet(styleSheet() +
    SS_BEGIN(QToolButton)
      SS_NO_BORDER
    SS_END
  );

  // Shortcuts
  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));
  QShortcut *closeShortcut = new QShortcut(QKeySequence::Close, this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(hide()));

  QShortcut *increaseShortcut = new QShortcut(QKeySequence("CTRL+="), this);
  connect(increaseShortcut, SIGNAL(activated()), SLOT(increase()));
  QShortcut *decreaseShortcut = new QShortcut(QKeySequence("CTRL+-"), this);
  connect(decreaseShortcut, SIGNAL(activated()), SLOT(decrease()));
}

// - Slots -

void
UrlDialog::open()
{
  hide();
  QString url = text().trimmed();
  if (url.isEmpty() || url == "http://")
    return;

  url = autoCompleteUrl(url);
  emit urlEntered(url);
}

QString
UrlDialog::text() const
{ return edit_->currentText(); }

void
UrlDialog::setText(const QString &text)
{ edit_->setEditText(text); }

void
UrlDialog::increase()
{
  QString t = text().trimmed();
  if (t.isEmpty())
    return;
  t = QtExt::increaseString(t);
  setText(t);
}

void
UrlDialog::decrease()
{
  QString t = text().trimmed();
  if (t.isEmpty())
    return;
  t = QtExt::decreaseString(t);
  setText(t);
}

void
UrlDialog::paste()
{
  QClipboard *c = QApplication::clipboard();
  if (c) {
    QString t = c->text().trimmed();
    if (!t.isEmpty())
      setText(t);
  }
}

QString
UrlDialog::autoCompleteUrl(const QString &url)
{
  QString ret = url.trimmed();
  if (!url.contains("://"))
    ret = "http://" + ret;
  else if (url.startsWith("ttp://"))
    ret.prepend('h');

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
