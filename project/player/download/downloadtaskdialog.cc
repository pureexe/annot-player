// downloadtaskdialog.cc
// 2/18/2012

#include "downloadtaskdialog.h"
#include "uistyle.h"
#include "logger.h"
#include "tr.h"
#include "util/textview.h"
#include "module/qtext/algorithm.h"
#include <QtGui>

#define DEBUG "downloaddialog"
#include "module/debug/debug.h"

using namespace Logger;

#ifdef Q_OS_MAC
  #define K_CTRL        "cmd"
#else
  #define K_CTRL        "Ctrl"
#endif // Q_OS_MAC

// - Construciton -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint | \
  Qt::WindowStaysOnTopHint )

DownloadTaskDialog::DownloadTaskDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  UiStyle::globalInstance()->setWindowStyle(this);
  setWindowTitle(tr("Add download URLs"));
  createLayout();

  // Shortcuts
  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));
  QShortcut *closeShortcut = new QShortcut(QKeySequence::Close, this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(hide()));

  QShortcut *saveShortcut = new QShortcut(QKeySequence::Save, this);
  connect(saveShortcut, SIGNAL(activated()), SLOT(add()));

  // Focus
  textView_->setFocus();
}

void
DownloadTaskDialog::createLayout()
{
  UiStyle *ui = UiStyle::globalInstance();

  textView_ = new TextView;

  urlButton_ = ui->makeToolButton(
        UiStyle::UrlHint,
        tr("http://www.nicovideo.jp/watch/sm12159572"),
        tr("Click to paste the URL example"),
        this, SLOT(showExampleUrl()));

  QLabel *urlLabel = ui->makeLabel(UiStyle::BuddyHint, TR(T_EXAMPLE), urlButton_);

  QToolButton *addButton = ui->makeToolButton(
        UiStyle::PushHint | UiStyle::HighlightHint, TR(T_ADD), "", K_CTRL "+S",
        this, SLOT(add()));
  QToolButton *pasteButton = ui->makeToolButton(
        UiStyle::PushHint, TR(T_PASTE), this, SLOT(paste()));
  QToolButton *clearButton = ui->makeToolButton(
        UiStyle::PushHint, TR(T_CLEAR), this, SLOT(clear()));

  // Layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout,
                *footer = new QHBoxLayout;
    rows->addLayout(header);
    rows->addWidget(textView_);
    rows->addLayout(footer);

    header->addWidget(urlLabel);
    header->addWidget(urlButton_);

    footer->addWidget(clearButton);
    footer->addWidget(pasteButton);
    footer->addStretch();
    footer->addWidget(addButton);

    // l, t, r, b
    int patch = 0;
    if (!UiStyle::isAeroAvailable())
      patch = 9;
    header->setContentsMargins(0, 0, 0, 0);
    footer->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(9, patch, 9, patch);
  } setLayout(rows);
}

void
DownloadTaskDialog::setText(const QString &urls)
{ textView_->setText(urls); }

void
DownloadTaskDialog::addText(const QString &urls)
{ textView_->append(urls); }

// - Actions -

void
DownloadTaskDialog::add()
{
  hide();
  QString text = textView_->toPlainText();
  QStringList urls;
  foreach (QString t, text.split('\n', QString::SkipEmptyParts)) {
    t = t.trimmed();
    if (t.contains("://"))
      urls.append(t);
  }
  bool batch = false;
  if (!urls.isEmpty()) {
    urls = QtExt::uniqueList(urls);
    DOUT("urls =" << urls);
    emit urlsAdded(urls, batch);
  } else if (!text.isEmpty())
    warn(tr("invalid URLs") + ": " + text);
}

void
DownloadTaskDialog::paste()
{
  QClipboard *clipboard = QApplication::clipboard();
  if (clipboard) {
    QString t = clipboard->text().trimmed();
    textView_->append(t);
  }
}

void
DownloadTaskDialog::showExampleUrl()
{ setText(urlButton_->text()); }

void
DownloadTaskDialog::clear()
{ textView_->clear(); }

// EOF
