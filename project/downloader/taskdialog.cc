// taskdialog.cc
// 2/18/2012

#include "taskdialog.h"
#include "project/common/acui.h"
#include "project/common/actextview.h"
#include "module/mrlanalysis/mrlanalysis.h"
#include "module/qtext/algorithm.h"
#include "module/qtext/string.h"
#include <QtGui>

#define DEBUG "taskdialog"
#include "module/debug/debug.h"

#ifdef Q_OS_MAC
#  define K_CTRL        "cmd"
#else
#  define K_CTRL        "Ctrl"
#endif // Q_OS_MAC

// - Construciton -

TaskDialog::TaskDialog(QWidget *parent)
  : Base(parent)
{
  setWindowTitle(tr("Add download URLs"));
  createLayout();

  // Shortcuts
  new QShortcut(QKeySequence::Save, this, SLOT(add()));

  // Focus
  textView_->setFocus();
}

void
TaskDialog::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  textView_ = new AcTextView(this);

  urlButton_ = ui->makeToolButton(
        AcUi::UrlHint,
        tr("http://www.nicovideo.jp/watch/sm12159572"),
        tr("Click to paste the URL example"),
        this, SLOT(showExampleUrl()));

  QLabel *urlLabel = ui->makeLabel(AcUi::BuddyHint, tr("Example"), urlButton_);

  QToolButton *addButton = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, tr("Add"), "", K_CTRL "+S",
        this, SLOT(add()));
  QToolButton *increaseButton = ui->makeToolButton(
        0, "+", tr("Increase"), this, SLOT(increase()));
  QToolButton *decreaseButton = ui->makeToolButton(
        0, "-", tr("Decrease"), this, SLOT(decrease()));
  QToolButton *pasteButton = ui->makeToolButton(
        AcUi::PushHint, tr("Paste"), this, SLOT(paste()));
  QToolButton *clearButton = ui->makeToolButton(
        AcUi::PushHint, tr("Clear"), this, SLOT(clear()));

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
    footer->addWidget(decreaseButton);
    footer->addWidget(increaseButton);
    footer->addWidget(addButton);

    // l, t, r, b
    int patch = 0;
    if (!AcUi::isAeroAvailable())
      patch = 9;
    header->setContentsMargins(0, 0, 0, 0);
    footer->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(9, patch, 9, patch);
  } setLayout(rows);
}

// - Properties -

void
TaskDialog::setText(const QString &urls)
{ textView_->setText(urls); }

void
TaskDialog::addText(const QString &text)
{
  if (text.isEmpty())
    return;
  if (!textView_->containsPlainText(text))
    textView_->append(text);
  else
    emit message(tr("duplicated") + ": " + text);
}

QString
TaskDialog::lastUrl() const
{
  if (textView_->isEmpty())
    return QString();
  QString text = textView_->last().trimmed();
  QStringList urls = text.split('\n', QString::SkipEmptyParts);
  return urls.isEmpty() ? QString() : urls.last().trimmed();
}

// - Actions -

void
TaskDialog::add()
{
  fadeOut();
  QString text = textView_->toPlainText();
  QStringList urls;
  foreach (const QString &t, text.split('\n', QString::SkipEmptyParts))
    urls.append(t.trimmed());
  bool batch = false;
  if (!urls.isEmpty()) {
    QStringList ret;
    foreach (const QString &url, QtExt::uniqueList(urls))
      ret.append(formatUrl(url));
    DOUT("urls =" << ret);
    emit urlsAdded(ret, batch);
  } else if (!text.isEmpty())
    emit warning(tr("invalid URLs") + ": " + text);
}

void
TaskDialog::paste()
{
  QClipboard *clipboard = QApplication::clipboard();
  if (clipboard) {
    QString t = clipboard->text().trimmed();
    if (!t.isEmpty())
      t.append("\n");
    addText(t);
  }
}

void
TaskDialog::showExampleUrl()
{ setText(urlButton_->text()); }

void
TaskDialog::clear()
{ textView_->clear(); }

void
TaskDialog::increase()
{
  QString prevUrl = lastUrl();
  if (prevUrl.isEmpty())
    return;
  QString url = prevUrl;
  bool ac = url.contains(MA_EIGEN_BILIBILI, Qt::CaseInsensitive) ||
            url.contains(MA_EIGEN_ACFUN, Qt::CaseInsensitive);
  if (ac) {
    if (url.endsWith('#'))
      url.chop(1);
    if (url.endsWith('/'))
      url.append("index_1.html");
  }
  url = QtExt::increaseString(url);

  if (url != prevUrl)
    addText(url);
}

void
TaskDialog::decrease()
{
  QString prevUrl = lastUrl();
  if (prevUrl.isEmpty())
    return;
  QString url = prevUrl;
  bool ac = url.contains(MA_EIGEN_BILIBILI, Qt::CaseInsensitive) ||
            url.contains(MA_EIGEN_ACFUN, Qt::CaseInsensitive);
  if (ac) {
    if (url.endsWith('#'))
      url.chop(1);
    if (!url.endsWith('/') ||
        url.endsWith("/index_1.html"))
      return;
  }
  url = QtExt::decreaseString(url);

  if (ac) {
    url.replace(QRegExp("/index_1.html$"), "/")
       .replace(QRegExp("/index_0.html$"), "/");
  }

  if (url != prevUrl)
    addText(url);
}

QString
TaskDialog::formatUrl(const QString &url)
{
  QString ret = url.trimmed();
  if (ret.isEmpty())
    return ret;
  if (ret.startsWith("ttp://"))
    ret.prepend('h');
  else if (!ret.startsWith("http://", Qt::CaseInsensitive))
    ret.prepend("http://");

  ret.remove(QRegExp("#$"))
     .remove(QRegExp("#titles$"))
     .replace(QRegExp("/index.html$", Qt::CaseInsensitive), "/")
     .replace(QRegExp("/index_1.html$", Qt::CaseInsensitive), "/")
     .replace(QRegExp("/#$"), "/");
  return ret;
}

// EOF
