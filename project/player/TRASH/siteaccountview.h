#ifndef SITEACCOUNTVIEW_H
#define SITEACCOUNTVIEW_H

// siteaccountview.h
// 2/8/2012

#include "project/common/acwindow.h"

struct SiteAccount
{
  QString username,
          password;

  SiteAccount() { }
  SiteAccount(const QString &name, const QString &pass)
    : username(name), password(pass) { }
  void clear() { username.clear(); password.clear(); }
};

QT_FORWARD_DECLARE_CLASS(QLineEdit)
QT_FORWARD_DECLARE_CLASS(QComboBox)

class SiteAccountView : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(SiteAccountView)
  typedef SiteAccountView Self;
  typedef AcWindow Base;

  SiteAccount nicovideoAccount_;
  QComboBox *nicovideoUsernameEdit_;
  QLineEdit *nicovideoPasswordEdit_;

  SiteAccount bilibiliAccount_;
  QComboBox *bilibiliUsernameEdit_;
  QLineEdit *bilibiliPasswordEdit_;

public:
  explicit SiteAccountView(QWidget *parent = nullptr);

  const SiteAccount &nicovideoAccount() const { return nicovideoAccount_; }
  const SiteAccount &bilibiliAccount() const { return bilibiliAccount_; }

signals:
  void accountChanged();

public slots:
  void setVisible(bool visible) override;

  void setNicovideoAccount(const QString &username, const QString &password);
  void setBilibiliAccount(const QString &username, const QString &password);

  void invalidateAccounts();

protected slots:
  void save();

  void visitNicovideo();
  void visitBilibili();

private:
  void createLayout();
};

#endif // SITEACCOUNTVIEW_H
