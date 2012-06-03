#ifndef ACLOCATIONPREFS_P_H
#define ACLOCATIONPREFS_P_H

// aclocationprefs_p.h
// 5/5/2012

#include "project/common/acpreferencestab.h"

QT_FORWARD_DECLARE_CLASS(QComboBox)

class AcLocationManager;
class AcLocationPrefs : public AcPreferencesTab
{
  Q_OBJECT
  Q_DISABLE_COPY(AcLocationPrefs)
  typedef AcLocationPrefs Self;
  typedef AcPreferencesTab Base;

  AcLocationManager *locationManager_;

  QComboBox *downloadsLocationEdit_;
  QString downloadsLocationEditStyleSheet_;

public:
  explicit AcLocationPrefs(AcSettings *settings, QWidget *parent = 0);

public slots:
  virtual bool save(); ///< \override
  virtual void load(); ///< \override

protected slots:
  void verifyLocation(const QString &path);

private:
  void createLayout();
};

#endif // ACLOCATIONPREFS_P_H
