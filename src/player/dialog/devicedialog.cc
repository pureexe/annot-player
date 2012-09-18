// devicedialog.cc
// 12/2/2011

#include "devicedialog.h"
#include "tr.h"
#include "src/common/acui.h"
#include "htmlutil/sstags.h"
#ifdef Q_OS_WIN
# include "win/qtwin/qtwin.h"
#endif // Q_OS_WIN
#ifdef Q_OS_UNIX
# include "unix/qtunix/qtunix.h"
#endif // Q_OS_UNIX
#ifdef WITH_LIB_IOUTIL
# include "lib/ioutil/ioutil.h"
#endif // WITH_LIB_IOUTIL
#include <QtGui>

#define DEBUG "devicedialog"
#include "qtx/qxdebug.h"

#define COMBOBOX_MINWIDTH      100

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
#elif defined(Q_OS_UNIX)
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

  // Post behaviors
  autoRadioButton_->setChecked(true);
  okButton_->setFocus();
}

void
DeviceDialog::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  QString holder =
#ifdef Q_OS_WIN
    "x:\\"
#elif defined(Q_OS_MAC)
    "/dev/cdrom"
#else
    "/dev/cdrom"
#endif // Q_OS_
  ;

  pathEdit_ = ui->makeComboBox(0, TR(T_PATH), TR(T_PATH), holder);
  pathEdit_->setMinimumWidth(COMBOBOX_MINWIDTH);
  pathEditStyleSheet_ = pathEdit_->styleSheet();
  //connect(pathEdit_, SIGNAL(activated(int)), SLOT(setMoveStyle(int)));

  autoRadioButton_ = ui->makeRadioButton(0, tr("auto"));
  dvdRadioButton_ = ui->makeRadioButton(0, tr("DVD"));
  cdRadioButton_ = ui->makeRadioButton(0, tr("CD"));

  okButton_ = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, TR(T_OPEN), TR(T_TIP_OPENDEVICE), this, SLOT(ok()));
  QToolButton *cancelButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_CANCEL), this, SLOT(fadeOut()));
  QToolButton *refreshButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_REFRESH), this, SLOT(refresh()));

  // Layouts
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *buttons = new QHBoxLayout,
                *path = new QHBoxLayout;

    //rows->addWidget(messageLabel_);
    rows->addLayout(path);
    rows->addLayout(buttons);

    path->addWidget(pathEdit_);
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
  connect(pathEdit_, SIGNAL(currentIndexChanged(int)), SLOT(updateButtons()));
  connect(pathEdit_, SIGNAL(editTextChanged(QString)), SLOT(updateButtons()));
  connect(pathEdit_->lineEdit(), SIGNAL(returnPressed()), SLOT(ok()));
}

// - Properties -

//void
//DeviceDialog::setMessage(const QString &text)
//{ messageLabel_->setText(text); }

QString
DeviceDialog::currentPath() const
{ return pathEdit_->currentText(); }

// - Actions -

void
DeviceDialog::ok()
{
  fadeOut();

  QString path = currentPath();
  if (!QFile::exists(path)) {
    emit warning(TR(T_ERROR_BAD_FILEPATH) + ": " + path);
    return;
  }

  bool isAudioCD;
#if WITH_LIB_IOUTIL
  if (autoRadioButton_->isChecked())
    isAudioCD = IOUtil::isAudioCD(path);
  else
#endif // WITH_LIB_IOUTIL
  isAudioCD = cdRadioButton_->isChecked();
  emit deviceSelected(path, isAudioCD);
}

void
DeviceDialog::refresh()
{
  //updateMessage();
  updateComboBox();
  updateButtons();
}

void
DeviceDialog::updateComboBox()
{
  pathEdit_->clear();
  QStringList items = devices();
  foreach (const QString &path, items)
    pathEdit_->addItem(path);

  if (pathEdit_->count())
    pathEdit_->setCurrentIndex(0);
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
DeviceDialog::updateButtons()
{
  QString path = currentPath();
  bool t = QFile::exists(path);
  okButton_->setEnabled(t);

  pathEdit_->setStyleSheet(pathEditStyleSheet_ + (t ?
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
