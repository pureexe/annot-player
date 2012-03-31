// devicedialog.cc
// 12/2/2011

#include "devicedialog.h"
#include "tr.h"
#include "logger.h"
#include "ac/acui.h"
#ifdef Q_OS_WIN
#  include "win/qtwin/qtwin.h"
#endif // Q_OS_WIN
#ifdef Q_OS_UNIX
#  include "unix/qtunix/qtunix.h"
#endif // Q_OS_UNIX
#ifdef USE_MODULE_IOUTIL
#  include "module/ioutil/ioutil.h"
#endif // USE_MODULE_IOUTIL
#include <QtGui>

#define DEBUG "devicedialog"
#include "module/debug/debug.h"

#define COMBOBOX_MINWIDTH      100

using namespace Logger;

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowCloseButtonHint | \
  Qt::WindowStaysOnTopHint )

// - Helpers -

QStringList
DeviceDialog::devices()
{
#ifdef Q_OS_WIN
  QStringList ret = QtWin::getLogicalDrivesWithType(QtWin::CdRomDrive);
  if (ret.isEmpty())
    ret.append("X:\\");
  return ret;
#elif defined (Q_OS_UNIX)
  QStringList ret = QtUnix::getDevicesWithType(QtUnix::CdRom);
  if (ret.isEmpty())
    ret.append("/dev/cdrom");
  return ret;
#else
  Q_ASSERT(0); // unimplemented
  return "/dev/cdrom";
#endif // Q_OS_WIN | Q_OS_UNIX
}

// - Constructions -

DeviceDialog::DeviceDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
//#ifdef Q_OS_MAC
//  setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
//#endif // Q_OS_MAC
  setWindowTitle(TR(T_TITLE_OPENDEVICE));

  createLayout();

  // Shortcuts
  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));
  QShortcut *closeShortcut = new QShortcut(QKeySequence::Close, this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(hide()));

  // Post behaviors
  autoRadioButton_->setChecked(true);
  okButton_->setFocus();
}

void
DeviceDialog::createLayout()
{
  AcUi *ui = AcUi::globalInstance();
  ui->setWindowStyle(this);

  pathComboBox_ = ui->makeComboBox(0, TR(T_PATH));
  pathComboBox_->setMinimumWidth(COMBOBOX_MINWIDTH);
  //connect(pathComboBox_, SIGNAL(activated(int)), SLOT(setMoveStyle(int)));

  autoRadioButton_ = ui->makeRadioButton(0, tr("auto"));
  dvdRadioButton_ = ui->makeRadioButton(0, tr("DVD"));
  cdRadioButton_ = ui->makeRadioButton(0, tr("CD"));

  okButton_ = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, TR(T_OPEN), TR(T_TIP_OPENDEVICE), this, SLOT(ok()));
  QToolButton *cancelButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_CANCEL), this, SLOT(hide()));
  QToolButton *refreshButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_REFRESH), this, SLOT(refresh()));

  // Layouts
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *buttons = new QHBoxLayout,
                *path = new QHBoxLayout;

    //rows->addWidget(messageLabel_);
    rows->addLayout(path);
    rows->addLayout(buttons);

    path->addWidget(pathComboBox_);
    path->addStretch();
    path->addWidget(autoRadioButton_);
    path->addWidget(dvdRadioButton_);
    path->addWidget(cdRadioButton_);

    buttons->addWidget(refreshButton);
    buttons->addStretch();
    buttons->addWidget(cancelButton);
    buttons->addWidget(okButton_);

    rows->setContentsMargins(9, 9, 9, 9);
    setContentsMargins(0, 0, 0, 0);
  } setLayout(rows);

  // Connections
  connect(pathComboBox_, SIGNAL(currentIndexChanged(int)), SLOT(invalidateButtons()));
  connect(pathComboBox_, SIGNAL(editTextChanged(QString)), SLOT(invalidateButtons()));
  connect(pathComboBox_->lineEdit(), SIGNAL(returnPressed()), SLOT(ok()));
}

// - Properties -

//void
//DeviceDialog::setMessage(const QString &text)
//{ messageLabel_->setText(text); }

QString
DeviceDialog::currentPath() const
{ return pathComboBox_->currentText(); }

// - Actions -

void
DeviceDialog::ok()
{
  hide();

  QString path = currentPath();
  if (!QFile::exists(path)) {
    warn(TR(T_ERROR_BAD_FILEPATH) + ": " + path);
    return;
  }

  bool isAudioCD;
#if USE_MODULE_IOUTIL
  if (autoRadioButton_->isChecked())
    isAudioCD = IOUtil::isAudioCD(path);
  else
#endif // USE_MODULE_IOUTIL
  isAudioCD = cdRadioButton_->isChecked();
  emit deviceSelected(path, isAudioCD);
}

void
DeviceDialog::refresh()
{
  //invalidateMessage();
  invalidateComboBox();
  invalidateButtons();
}

void
DeviceDialog::invalidateComboBox()
{
  pathComboBox_->clear();
  QStringList items = devices();
  foreach (QString path, items)
    pathComboBox_->addItem(path);

  if (pathComboBox_->count())
    pathComboBox_->setCurrentIndex(0);
}

// - Events -

void
DeviceDialog::setVisible(bool visible)
{
  if (visible)
    refresh();
  Base::setVisible(visible);
}

void
DeviceDialog::invalidateButtons()
{
  QString path = currentPath();
  bool t = QFile::exists(path);
  okButton_->setEnabled(t);
}

// EOF
