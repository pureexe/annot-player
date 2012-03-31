// urldialog.cc
// 2/8/2012

#include "urldialog.h"
#include "tr.h"
#include "ac/acui.h"
#include "module/qtext/ss.h"
#include "module/qtext/string.h"
#include "module/qtext/overlaylayout.h"
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

  createLayout();

  edit_->setFocus();
}

void
UrlDialog::createLayout()
{
  AcUi *ui = AcUi::globalInstance();
  ui->setWindowStyle(this);

  edit_ = ui->makeComboBox(AcUi::EditHint);
  connect(edit_->lineEdit(), SIGNAL(returnPressed()), SLOT(open()));

  saveButton_ = ui->makeToolButton(
        AcUi::CheckHint, TR(T_SAVE), tr("Save the association online"));
  saveButton_->setChecked(true);

  urlButton_ = ui->makeToolButton(
        AcUi::UrlHint, "", tr("Click to paste the URL example"), this, SLOT(showExampleUrl()));
  QLabel *urlLabel = ui->makeLabel(AcUi::BuddyHint, TR(T_EXAMPLE), urlButton_);

  QToolButton *openButton = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, TR(T_OPEN), this, SLOT(open()));
  QToolButton *pasteButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_PASTE), this, SLOT(paste()));
  QToolButton *clearButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_CLEAR), edit_->lineEdit(), SLOT(clear()));

  QToolButton *increaseButton = ui->makeToolButton(
        AcUi::NoHint, "+", TR(T_INCREASE), K_CTRL "+=", this, SLOT(increase()));
  QToolButton *decreaseButton = ui->makeToolButton(
        AcUi::NoHint, "-", TR(T_INCREASE), K_CTRL "+-", this, SLOT(decrease()));

  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout,
                *footer = new QHBoxLayout;
    QLayout *inc = new QVBoxLayout;
    OverlayLayout *body = new OverlayLayout;

    rows->addLayout(header);
    rows->addLayout(body);
    rows->addLayout(footer);

    header->addWidget(urlLabel);
    header->addWidget(urlButton_);

    body->addWidget(edit_);
    body->addLayout(inc, Qt::AlignRight);
    inc->addWidget(increaseButton);
    inc->addWidget(decreaseButton);

    footer->addWidget(clearButton);
    footer->addWidget(pasteButton);
    footer->addStretch();
    footer->addWidget(saveButton_);
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

  edit_->addItem(url);

  url = autoCompleteUrl(url);
  emit urlEntered(url, saveButton_->isChecked());
}

void
UrlDialog::paste()
{
  QClipboard *c = QApplication::clipboard();
  if (c){
    QString t = c->text().trimmed();
    if (!t.isEmpty())
      setText(t);
  }
}

QString
UrlDialog::text() const
{ return edit_->currentText(); }

bool
UrlDialog::isEmpty() const
{ return text().trimmed().isEmpty(); }

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

QString
UrlDialog::autoCompleteUrl(const QString &url)
{
  QString ret = url.trimmed();
  if (!url.contains("://"))
    ret = "http://" + ret;
  else if (url.startsWith("ttp://"))
    ret.prepend('h');

  ret.replace(QRegExp("/index.html$", Qt::CaseInsensitive), "/");
  ret.replace(QRegExp("/#$"), "/");

  return ret;
}

void
UrlDialog::showExampleUrl()
{ setText(urlButton_->text()); }

void
UrlDialog::setExampleUrl(const QString &text)
{ urlButton_->setText(text); }

void
UrlDialog::setSave(bool t)
{ saveButton_->setChecked(t); }

// EOF
