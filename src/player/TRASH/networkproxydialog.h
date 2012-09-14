#ifndef NETWORKPROXYDIALOG_H
#define NETWORKPROXYDIALOG_H

// networkproxydialog.h
// 3/12/2012

#include "src/common/acwindow.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QLineEdit;
class QToolButton;
QT_END_NAMESPACE

class NetworkProxyDialog : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(NetworkProxyDialog)
  typedef NetworkProxyDialog Self;
  typedef AcWindow Base;

  enum ProxyType { SocksProxy = 0, HttpProxy, ProxyTypeCount };

public:
  explicit NetworkProxyDialog(QWidget *parent = nullptr);

public slots:
  void setHost(const QString &url);
  void setPort(int port);
  void setUserName(const QString &userName);
  void setPassword(const QString &password);

  void setVisible(bool visible) override;

protected slots:
  void save();
  void refresh();
  void updateButtons();

protected:
  //void mouseDoubleClickEvent(QMouseEvent *event) override;
  //void contextMenuEvent(QContextMenuEvent *event) override;
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
