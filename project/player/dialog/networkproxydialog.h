#ifndef NETWORKPROXYDIALOG_H
#define NETWORKPROXYDIALOG_H

// networkproxydialog.h
// 3/12/2012

#include "module/qtext/dialog.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QLineEdit;
class QLabel;
class QToolButton;
QT_END_NAMESPACE

typedef QtExt::Dialog NetworkProxyDialogBase;
class NetworkProxyDialog : public NetworkProxyDialogBase
{
  Q_OBJECT
  typedef NetworkProxyDialog Self;
  typedef NetworkProxyDialogBase Base;

  enum ProxyType { SocksProxy = 0, HttpProxy, ProxyTypeCount };

public:
  explicit NetworkProxyDialog(QWidget *parent = 0);

public slots:
  void setHost(const QString &url);
  void setPort(int port);
  void setUserName(const QString &userName);
  void setPassword(const QString &password);

  virtual void setVisible(bool visible); ///< \override

protected slots:
  void save();
  void refresh();
  void invalidateButtons();

protected:
  //virtual void mouseDoubleClickEvent(QMouseEvent *event);
  //virtual void contextMenuEvent(QContextMenuEvent *event);
  bool isEnabled() const;

private:
  void createLayout();

private:
  QComboBox *typeCombo_;
  QComboBox *hostEdit_,
            *portEdit_,
            *userNameEdit_;
  QLineEdit *passwordEdit_;
  QToolButton *enableButton_;
              //*defaultProxyButton_;

  QLabel *typeLabel_,
         *hostLabel_,
         *userNameLabel_,
         *passwordLabel_;
};

#endif // NETWORKPROXYDIALOG_H
