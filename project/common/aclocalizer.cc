// aclocalizer.cc
// 8/24/2012

#include "project/common/aclocalizer.h"
#ifdef Q_OS_WIN
# include "win/qtwin/qtwin.h"
#endif // Q_OS_WIN
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#define DEBUG "aclocalizer"
#include "module/debug/debug.h"

// - Properties -

QString
AcLocalizer::rootLocation()
{ return ".."; }

QString
AcLocalizer::playerLocation()
{ return "../[ Play ].exe"; }

QString
AcLocalizer::playerLocalizedLocation()
{ return "../[ " + tr("Play") + " ].exe"; }

QString
AcLocalizer::browserLocation()
{ return "../[ Browse ].exe"; }

QString
AcLocalizer::browserLocalizedLocation()
{ return "../[ " + tr("Browse") + " ].exe"; }

QString
AcLocalizer::downloaderLocation()
{ return "../[ Download ].exe"; }

QString
AcLocalizer::downloaderLocalizedLocation()
{ return "../[ " + tr("Download") + " ].exe"; }

QString
AcLocalizer::translatorLocation()
{ return "../[ Translate ].exe"; }

QString
AcLocalizer::translatorLocalizedLocation()
{ return "../[ " + tr("Translate") + " ].exe"; }

QString
AcLocalizer::updaterLocation()
{ return "../[ Update ].exe"; }

QString
AcLocalizer::updaterLocalizedLocation()
{ return "../[ " + tr("Update") + " ].exe"; }

QString
AcLocalizer::readmeLocation()
{ return "../Readme.txt"; }

QString
AcLocalizer::readmeLocalizedLocation()
{ return "../" + tr("Readme") + ".txt"; }

QString
AcLocalizer::changelogLocation()
{ return "../Changes.txt"; }

QString
AcLocalizer::changelogLocalizedLocation()
{ return "../" + tr("Changes") + ".txt"; }

QString
AcLocalizer::licenseLocation()
{ return "../License.txt"; }

QString
AcLocalizer::licenseLocalizedLocation()
{ return "../" + tr("License") + ".txt"; }

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
             QDir(rootLocation()).entryList(QStringList("*.exe") << "*.txt", QDir::Files)) {
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
