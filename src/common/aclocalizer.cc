// aclocalizer.cc
// 8/24/2012

#include "src/common/aclocalizer.h"
#ifdef Q_OS_WIN
# include "win/qtwin/qtwin.h"
#endif // Q_OS_WIN
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#define DEBUG "aclocalizer"
#include "qtx/qxdebug.h"

#define ROOT_DIR    "../.."

// - Properties -

QString
AcLocalizer::rootLocation()
{ return ROOT_DIR; }

QString
AcLocalizer::playerLocation()
{ return ROOT_DIR "/[ Play ].exe"; }

QString
AcLocalizer::playerLocalizedLocation()
{ return ROOT_DIR "/[ " + tr("Play") + " ].exe"; }

QString
AcLocalizer::readerLocation()
{ return ROOT_DIR "/[ Read ].exe"; }

QString
AcLocalizer::readerLocalizedLocation()
{ return ROOT_DIR "/[ " + tr("Read") + " ].exe"; }

QString
AcLocalizer::browserLocation()
{ return ROOT_DIR "/[ Browse ].exe"; }

QString
AcLocalizer::browserLocalizedLocation()
{ return ROOT_DIR "/[ " + tr("Browse") + " ].exe"; }

QString
AcLocalizer::downloaderLocation()
{ return ROOT_DIR "/[ Download ].exe"; }

QString
AcLocalizer::downloaderLocalizedLocation()
{ return ROOT_DIR "/[ " + tr("Download") + " ].exe"; }

QString
AcLocalizer::translatorLocation()
{ return ROOT_DIR "/[ Translate ].exe"; }

QString
AcLocalizer::translatorLocalizedLocation()
{ return ROOT_DIR "/[ " + tr("Translate") + " ].exe"; }

QString
AcLocalizer::updaterLocation()
{ return ROOT_DIR "/[ Update ].exe"; }

QString
AcLocalizer::updaterLocalizedLocation()
{ return ROOT_DIR "/[ " + tr("Update") + " ].exe"; }

QString
AcLocalizer::readmeLocation()
{ return ROOT_DIR "/Readme.txt"; }

QString
AcLocalizer::readmeLocalizedLocation()
{ return ROOT_DIR "/" + tr("Readme") + ".txt"; }

QString
AcLocalizer::changelogLocation()
{ return ROOT_DIR "/Changes.txt"; }

QString
AcLocalizer::changelogLocalizedLocation()
{ return ROOT_DIR "/" + tr("Changes") + ".txt"; }

QString
AcLocalizer::licenseLocation()
{ return ROOT_DIR "/License.txt"; }

QString
AcLocalizer::licenseLocalizedLocation()
{ return ROOT_DIR "/" + tr("License") + ".txt"; }

// - Actions -

bool
AcLocalizer::locationNeedsUpdate()
{
  QFileInfo fi(translatorLocalizedLocation());
  return !fi.exists() || fi.isHidden();
}

void
AcLocalizer::updateLocations()
{
#ifdef Q_OS_WIN
  if (locationNeedsUpdate()) {
    DOUT("updating locations");

    foreach (const QString &file,
             QDir(rootLocation()).entryList(QStringList("*.exe") << "*.txt", QDir::Files))
      if (file.contains('[')) {
        DOUT("updating:" << file);
        QtWin::setFileAttributes(rootLocation() + '/' + file, QtWin::HiddenAttribute);
      }

    QString x, y;

    x = browserLocation();
    y = browserLocalizedLocation();
    if (x != y) {
      if (!QFile::exists(y))
        QFile::copy(x, y);
      QtWin::setFileAttributes(x, QtWin::HiddenAttribute);
    }
    QtWin::removeFileAttributes(y);

    x = downloaderLocation();
    y = downloaderLocalizedLocation();
    if (x != y) {
      if (!QFile::exists(y))
        QFile::copy(x, y);
      QtWin::setFileAttributes(x, QtWin::HiddenAttribute);
    }
    QtWin::removeFileAttributes(y);

    x = playerLocation();
    y = playerLocalizedLocation();
    if (x != y) {
      if (!QFile::exists(y))
        QFile::copy(x, y);
      QtWin::setFileAttributes(x, QtWin::HiddenAttribute);
    }
    QtWin::removeFileAttributes(y);

    x = readerLocation();
    y = readerLocalizedLocation();
    if (x != y) {
      if (!QFile::exists(y))
        QFile::copy(x, y);
      QtWin::setFileAttributes(x, QtWin::HiddenAttribute);
    }
    QtWin::removeFileAttributes(y);

    x = translatorLocation();
    y = translatorLocalizedLocation();
    if (x != y) {
      if (!QFile::exists(y))
        QFile::copy(x, y);
      QtWin::setFileAttributes(x, QtWin::HiddenAttribute);
    }
    QtWin::removeFileAttributes(y);

    x = updaterLocation();
    y = updaterLocalizedLocation();
    if (x != y) {
      if (!QFile::exists(y))
        QFile::copy(x, y);
      QtWin::setFileAttributes(x, QtWin::HiddenAttribute);
    }
    QtWin::removeFileAttributes(y);

    x = readmeLocation();
    y = readmeLocalizedLocation();
    if (x != y) {
      if (!QFile::exists(y))
        QFile::copy(x, y);
      QtWin::setFileAttributes(x, QtWin::HiddenAttribute);
    }
    QtWin::setFileAttributes(y, QtWin::ReadOnlyAttribute);

    x = changelogLocation();
    y = changelogLocalizedLocation();
    if (x != y) {
      if (!QFile::exists(y))
        QFile::copy(x, y);
      QtWin::setFileAttributes(x, QtWin::HiddenAttribute);
    }
    QtWin::setFileAttributes(y, QtWin::ReadOnlyAttribute);

    x = licenseLocation();
    y = licenseLocalizedLocation();
    if (x != y) {
      if (!QFile::exists(y))
        QFile::copy(x, y);
      QtWin::setFileAttributes(x, QtWin::HiddenAttribute);
    }
    QtWin::setFileAttributes(y, QtWin::ReadOnlyAttribute);
  }
#endif // Q_OS_WIN
}

// EOF
