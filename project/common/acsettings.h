#ifndef ACSETTINGS_H
#define ACSETTINGS_H

// ac/acsettings.h
// 7/30/2011

#include <QtCore/QMutex>
#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <QtCore/QDate>
#include <QtCore/QHash>
#include <utility>

class AcSettings : public QSettings
{
  Q_OBJECT
  Q_DISABLE_COPY(AcSettings)
  typedef AcSettings Self;
  typedef QSettings Base;

  QMutex m_;
  bool disposed_;

  // - Construction -
public:
  static Self *globalSettings() { static Self g; return &g; }
  explicit AcSettings(QObject *parent = nullptr);

  QString version() const;
  void setVersion(const QString &version);

  bool fackCcp() const;

public slots:
  void sync(); ///< thread-safe
  void dispose() { disposed_ = true; }

signals:
  void downloadsLocationChanged(const QString &path);
  void nicovideoAccountChanged(const QString &username, const QString &password);
  void bilibiliAccountChanged(const QString &username, const QString &password);

public:
  // - Accounts -

  qint64 userId() const;
  void setUserId(qint64 uid);

  QString userName() const;
  void setUserName(const QString &userName);

  QString password() const; ///< Encrypted
  void setPassword(const QString &password);

  std::pair<QString, QString> nicovideoAccount();
  void setNicovideoAccount(const QString &username, const QString &password);

  std::pair<QString, QString> bilibiliAccount();
  void setBilibiliAccount(const QString &username, const QString &password);

  // - i18n -

  int language() const; ///< Application language
  void setLanguage(int lang, int script = 0);

  int languageScript() const; ///< Application language script
  void setLanguageScript(int script);

  bool isEnglish() const;
  bool isJapanese() const;
  bool isChinese() const;
  bool isKorean() const;

  // - GUI -

  bool isMenuThemeEnabled() const;
  void setMenuThemeEnabled(bool t);

  int themeId() const;
  void setThemeId(int tid);

  // - Network proxy -

  void setProxyEnabled(bool t);
  bool isProxyEnabled() const;

  void setProxyHostName(const QString &host);
  QString proxyHostName() const;

  void setProxyPort(int port);
  int proxyPort() const;

  void setProxyType(int type);
  int proxyType() const;

  void setProxyUser(const QString &name);
  QString proxyUser() const;

  void setProxyPassword(const QString &password);
  QString proxyPassword() const;

  bool isAeroEnabled() const;
public slots:
  void setAeroEnabled(bool t);

  // - Locations -
public:
  QString downloadsLocation() const;
  void setDownloadsLocation(const QString &path);
};

#endif // ACSETTINGS_H
