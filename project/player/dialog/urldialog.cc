// urldialog.cc
// 2/8/2012

#include "urldialog.h"
#include "tr.h"
#include "project/common/acui.h"
#include "project/common/accomboedit.h"
#include "project/common/acsettings.h"
#include "module/mrlanalysis/mrlanalysis.h"
#include "module/qtext/overlaylayout.h"
#include "module/qtext/ss.h"
#include "module/qtext/string.h"
#include <QtGui>

#ifdef Q_OS_MAC
# define K_CTRL        "cmd"
#else
# define K_CTRL        "Ctrl"
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
{ createLayout(); }

void
UrlDialog::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  QString holder = "http://www.nicovideo.jp/watch/1327400176";
  edit_ = ui->makeComboBox(AcUi::EditHint, "", TR(T_URL), holder);
  editStyleSheet_ = edit_->styleSheet();
  connect(edit_, SIGNAL(editTextChanged(QString)), SLOT(verifyEditText()));
  connect(edit_->lineEdit(), SIGNAL(returnPressed()), SLOT(open()));
  auto edit = qobject_cast<AcComboEdit *>(edit_);
  Q_ASSERT(edit);
  edit->setContextMenuFlags(edit->contextMenuFlags() | AcComboEdit::PasteAndGoAction);

  //saveButton_ = ui->makeToolButton(
  //      AcUi::CheckHint, TR(T_SAVE), tr("Save the association online"));
  //saveButton_->setChecked(true);

  urlButton_ = ui->makeToolButton(
        AcUi::UrlHint, "", tr("Click to paste the URL example"), this, SLOT(showExampleUrl()));
  QLabel *urlLabel = ui->makeLabel(AcUi::BuddyHint, TR(T_EXAMPLE), urlButton_);

  openButton_ = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, TR(T_OPEN), this, SLOT(open()));
  QToolButton *pasteButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_PASTE), this, SLOT(paste()));
  QToolButton *clearButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_CLEAR), edit_->lineEdit(), SLOT(clear()));

  QToolButton *increaseButton = ui->makeToolButton(
        AcUi::NoHint, "+", TR(T_INCREASE), K_CTRL "+=", this, SLOT(increase()));
  QToolButton *decreaseButton = ui->makeToolButton(
        AcUi::NoHint, "-", TR(T_DECREASE), K_CTRL "+-", this, SLOT(decrease()));

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
    //footer->addWidget(saveButton_);
    footer->addWidget(openButton_);

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
  new QShortcut(QKeySequence("CTRL+="), this, SLOT(increase()));
  new QShortcut(QKeySequence("CTRL+-"), this, SLOT(decrease()));
}

// - Slots -

void
UrlDialog::open()
{
  fadeOut();
  QString url = text().trimmed();
  if (url.isEmpty() || url == "http://")
    return;

  if (edit_->count() &&
      edit_->itemText(edit_->count() - 1) != url)
    edit_->addItem(url);

  url = normalizeUrl(url);
  emit urlEntered(url);
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
  if (t.endsWith("/"))
    t.append("index_2.html");
  else
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
  t.replace("/index_1.html", "/")
   .replace("/index_0.html", "/");
  setText(t);
}

QString
UrlDialog::normalizeUrl(const QString &url)
{
  QString ret = url.trimmed();
  if (ret.isEmpty())
    return ret;
  if (url.startsWith("ttp://"))
    ret.prepend('h');
  else if (!url.startsWith("http://", Qt::CaseInsensitive))
    ret.prepend("http://");

  return MrlAnalysis::normalizeUrl(url);
}

void
UrlDialog::showExampleUrl()
{ setText(urlButton_->text()); }

void
UrlDialog::setExampleUrl(const QString &text)
{ urlButton_->setText(text); }

//void
//UrlDialog::setSave(bool t)
//{ saveButton_->setChecked(t); }

void
UrlDialog::addHistory(const QString &url)
{
  if (edit_->count() &&
      edit_->itemText(edit_->count() - 1) != url)
    edit_->addItem(url);

  if (edit_->currentText().trimmed().isEmpty())
    edit_->setEditText(url);
}

// - Events -

void
UrlDialog::setVisible(bool visible)
{
  if (visible) {
    edit_->setFocus();
    paste();
    verifyEditText();
  }
  Base::setVisible(visible);
}

void
UrlDialog::verifyEditText()
{
  QString t = edit_->currentText().trimmed();
  bool valid = MrlAnalysis::matchSite(t, false); // href = false

  openButton_->setEnabled(valid);
  edit_->setStyleSheet(editStyleSheet_ + (valid ?
    SS_BEGIN(QComboBox)
      SS_COLOR(black)
    SS_END
    :
    SS_BEGIN(QComboBox)
      SS_COLOR(red)
    SS_END
  ));
}

// EOF
