#ifndef ACACCOUNTPREFS_P_H
#define ACACCOUNTPREFS_P_H

// acaccountprefs_p.h
// 2/8/2012

#include "project/common/acpreferencestab.h"

QT_FORWARD_DECLARE_CLASS(QLineEdit)
QT_FORWARD_DECLARE_CLASS(QComboBox)

class AcAccountPreferences : public AcPreferencesTab
{
  Q_OBJECT
  Q_DISABLE_COPY(AcAccountPreferences)
  typedef AcAccountPreferences Self;
  typedef AcPreferencesTab Base;

public:
  explicit AcAccountPreferences(AcSettings *settings, QWidget *parent = 0);

public slots:
  virtual bool save(); ///< \override
  virtual void load(); ///< \override

protected slots:
  void visitNicovideo();
  void visitBilibili();

  void saveIfValid();

private:
  void createLayout();

private:
  QComboBox *nicovideoUsernameEdit_;
  QLineEdit *nicovideoPasswordEdit_;

  QComboBox *bilibiliUsernameEdit_;
  QLineEdit *bilibiliPasswordEdit_;

};

#endif // ACACCOUNTPREFS_P_H
