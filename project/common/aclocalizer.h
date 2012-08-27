#ifndef ACLOCALIZER_H
#define ACLOCALIZER_H

// aclocalizer.h
// 8/24/2012

#include <QtCore/QObject>

class AcLocalizer : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AcLocalizer)
  typedef AcLocalizer Self;
  typedef QObject Base;

  //static Self *globalInstance() { static Self g; return &g; }
  AcLocalizer() { }

  // - Properties -
public:
  static QString rootLocation();

  static QString playerLocation();
  static QString playerLocalizedLocation();

  static QString browserLocation();
  static QString browserLocalizedLocation();

  static QString downloaderLocation();
  static QString downloaderLocalizedLocation();

  static QString translatorLocation();
  static QString translatorLocalizedLocation();

  static QString updaterLocation();
  static QString updaterLocalizedLocation();

  static QString readmeLocation();
  static QString readmeLocalizedLocation();

  static QString changelogLocation();
  static QString changelogLocalizedLocation();

  static QString licenseLocation();
  static QString licenseLocalizedLocation();

  // - Actions -
public:
  static bool locationNeedsUpdate();
  static void updateLocations();
};

#endif // ACLOCALIZER_H
