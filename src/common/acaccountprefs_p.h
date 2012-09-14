#ifndef ACACCOUNTPREFS_P_H
#define ACACCOUNTPREFS_P_H

// acaccountprefs_p.h
// 2/8/2012

#include "src/common/acpreferencestab.h"

QT_FORWARD_DECLARE_CLASS(QLineEdit)
QT_FORWARD_DECLARE_CLASS(QComboBox)

class AcAccountPrefs : public AcPreferencesTab
{
  Q_OBJECT
  Q_DISABLE_COPY(AcAccountPrefs)
  typedef AcAccountPrefs Self;
  typedef AcPreferencesTab Base;

public:
  explicit AcAccountPrefs(AcSettings *settings, QWidget *parent = nullptr);

public slots:
  bool save() override;
  void load() override;

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
