#ifndef PREFERENCES_H
#define PREFERENCES_H

// preferences.h
// 5/25/2012

#include "project/common/acpreferences.h"
#include <QtCore/QList>

class Preferences : public AcPreferences
{
  Q_OBJECT
  Q_DISABLE_COPY(Preferences)
  typedef Preferences Self;
  typedef AcPreferences Base;

public:
  explicit Preferences(QWidget *parent = 0);
};

#endif // PREFERENCES_H
