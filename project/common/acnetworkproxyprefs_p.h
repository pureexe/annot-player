#ifndef ACNETWORKPROXYPREFS_P_H
#define ACNETWORKPROXYPREFS_P_H

// acnetworkproxyprefs_p.h
// 3/12/2012

#include "project/common/acpreferencestab.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QLineEdit;
class QToolButton;
QT_END_NAMESPACE

class AcNetworkProxyPrefs : public AcPreferencesTab
{
  Q_OBJECT
  Q_DISABLE_COPY(AcNetworkProxyPrefs)
  typedef AcNetworkProxyPrefs Self;
  typedef AcPreferencesTab Base;

  enum ProxyType { SocksProxy = 0, HttpProxy, ProxyTypeCount };
public:
  explicit AcNetworkProxyPrefs(AcSettings *settings, QWidget *parent = 0);

public slots:
  virtual bool save(); ///< \override
  virtual void load(); ///< \override
  void updateButtons();

protected:
  bool isEnabled() const;
  void setHost(const QString &url);
  void setPort(int port);
  void setUserName(const QString &userName);
  void setPassword(const QString &password);

private:
  void createLayout();

private:
  QComboBox *typeCombo_;
  QComboBox *hostEdit_,
            *portEdit_,
            *userNameEdit_;
  QLineEdit *passwordEdit_;
  QToolButton *enableButton_;

  QLabel //*typeLabel_,
         *hostLabel_,
         *userNameLabel_;
         //*passwordLabel_;
};

#endif // ACNETWORKPROXYPREFS_P_H
