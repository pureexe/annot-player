#ifndef MRLRESOLVERSETTINGS_H
#define MRLRESOLVERSETTINGS_H

// mrlresolversettings.h
// 2/19/2011

#include <QObject>
#include <QString>

class MrlResolverSettings : public QObject
{
  Q_OBJECT
  typedef MrlResolverSettings Self;
  typedef QObject Base;

public:
  struct Account {
    QString username, password;

    explicit Account(const QString &name = QString::null, const QString &pass = QString::null)
      : username(name), password(pass) { }
    bool isBad() const { return username.isEmpty() || password.isEmpty(); }
    void clear() { username.clear(); password.clear(); }
  };

  static Self *globalSettings() { static Self g; return &g; }

protected:
  explicit MrlResolverSettings(QObject *parent = 0) : Base(parent) { }

  // - Account -
public slots:
  void setNicovideoAccount(const QString &username = QString::null,
                           const QString &password = QString::null)
  {
    nicovideoAccount_.username = username;
    nicovideoAccount_.password = password;
  }
  void setBilibiliAccount(const QString &username = QString::null,
                          const QString &password = QString::null)
  {
    bilibiliAccount_.username = username;
    bilibiliAccount_.password = password;
  }

public:
  Account nicovideoAccount() const
  {
    return nicovideoAccount_.isBad() ? Account(NICOVIDEO_USERNAME, NICOVIDEO_PASSWORD)
                                     : nicovideoAccount_;
  }

  Account bilibiliAccount() const { return bilibiliAccount_; }

  bool hasNicovideoAccount() const { return true; }
  bool hasBilibiliAccount() const { return !bilibiliAccount_.isBad(); }

private:
  Account nicovideoAccount_;
  Account bilibiliAccount_;
};

#endif // MRLRESOLVERSETTINGS_H
