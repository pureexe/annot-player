// aclocationprefs_p.cc
// 5/5/2012

#include "project/common/aclocationprefs_p.h"
#include "project/common/acui.h"
#include "project/common/acpaths.h"
#include "project/common/acsettings.h"
#include "module/qtext/ss.h"
#include <QtGui>

// - Locations -

AcLocationPrefs::AcLocationPrefs(AcSettings *settings, QWidget *parent)
  : Base(settings, parent)
{
  locationManager_ = AcLocationManager::globalInstance();
  setWindowTitle(tr("Locations"));
  createLayout();
}

void
AcLocationPrefs::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  QStringList defvals(locationManager_->defaultDownloadsLocation());
  downloadsLocationEdit_ = ui->makeComboBox(
        AcUi::EditHint, "", tr("Downloads Location for Annot Player and Annot Downloader"), defvals.first(), defvals);
  downloadsLocationEdit_->setStatusTip(tr("Downloads Location"));
  connect(downloadsLocationEdit_, SIGNAL(editTextChanged(QString)), SLOT(verifyDownloadsLocation()));

  QToolButton *openButton = ui->makeToolButton(AcUi::HighlightHint, tr("Downloads"), tr("Open Downloads Location"), locationManager_, SLOT(openDownloadsLocation()));
  resetDownloadsLocationButton_ = ui->makeToolButton(AcUi::PushHint, tr("Default"), tr("Default Location"), this, SLOT(resetDownloadsLocation())),
  createDownloadsLocationButton_ = ui->makeToolButton(AcUi::PushHint, tr("Create"), tr("Create Directory"), locationManager_, SLOT(createDownloadsLocation()));

  connect(resetDownloadsLocationButton_, SIGNAL(clicked()), SLOT(verifyDownloadsLocation()));
  connect(createDownloadsLocationButton_, SIGNAL(clicked()), SLOT(verifyDownloadsLocation()));

  downloadsLocationEditStyleSheet_ = downloadsLocationEdit_->styleSheet();

  // Layouts
  QGridLayout *grid = new QGridLayout; {
    // (row, col, rowspan, colspan, alignment)
    int r, c;
    grid->addWidget(openButton, r=0, c=0);
    grid->addWidget(downloadsLocationEdit_, r, ++c);
    grid->addWidget(createDownloadsLocationButton_, r, ++c);
    grid->addWidget(resetDownloadsLocationButton_, r, ++c);

    grid->setContentsMargins(9, 9, 9, 9);
    setContentsMargins(0, 0, 0, 0);
  } setLayout(grid);

  // Connections
  //connect(userNameEdit_->lineEdit(), SIGNAL(returnPressed()), SLOT(login()));
  //connect(passwordEdit_, SIGNAL(returnPressed()), SLOT(login()));
}

void
AcLocationPrefs::load()
{
  loadDownloadsLocation();
  verifyDownloadsLocation();
}

bool
AcLocationPrefs::save()
{ return saveDownloadsLocation(); }

bool
AcLocationPrefs::saveDownloadsLocation()
{
  QString location = downloadsLocationEdit_->currentText().trimmed();
  if (location.isEmpty())
    location = locationManager_->defaultDownloadsLocation();
  settings()->setDownloadsLocation(location);
  return true;
}

void
AcLocationPrefs::loadDownloadsLocation()
{
  QString downloadsLocation = AcLocationManager::globalInstance()->downloadsLocation();
  downloadsLocationEdit_->setEditText(downloadsLocation);
  if (downloadsLocationEdit_->count() && downloadsLocation != downloadsLocationEdit_->itemText(0))
    downloadsLocationEdit_->addItem(downloadsLocation);
}

void
AcLocationPrefs::verifyDownloadsLocation()
{
  QString location = downloadsLocationEdit_->currentText().trimmed();
  bool ok = QFile::exists(location);

  if (ok)
    emit message(tr("exists") + ": " + location);
  else
    emit warning(tr("not exist") + ": " + location);

  resetDownloadsLocationButton_->setEnabled(location == locationManager_->defaultDownloadsLocation());
  createDownloadsLocationButton_->setEnabled(!ok);

  downloadsLocationEdit_->setStyleSheet(downloadsLocationEditStyleSheet_ + (ok ?
    SS_BEGIN(QComboBox)
      SS_COLOR(black)
    SS_END
    :
    SS_BEGIN(QComboBox)
      SS_COLOR(red)
    SS_END
  ));
}

void
AcLocationPrefs::resetDownloadsLocation()
{
  QString location = locationManager_->defaultDownloadsLocation();
  settings()->setDownloadsLocation(location);
}

// EOF
