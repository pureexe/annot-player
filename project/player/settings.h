#ifndef SETTINGS_H
#define SETTINGS_H

// settings.h
// 7/30/2011

#include <QSettings>
#include <QStringList>
#include <QDate>
#include <QHash>
#include <utility>

class Settings : protected QSettings
{
  Q_OBJECT
  typedef Settings Self;
  typedef QSettings Base;

  // - Constructions -
public:
  static Self *globalInstance() { static Self g; return &g; }
protected:
  explicit Settings(QObject *parent = 0);

public slots:
  void flush() { sync(); }

  // - Properties -
public:
  QString version() const;
  void setVersion(const QString &version);

  qint64 userId() const;
  void setUserId(qint64 uid);

  QString userName() const;
  void setUserName(const QString &userName);

  QString password() const; ///< Encrypted
  void setPassword(const QString &password);

  bool isQueueEmpty() const; ///< If database queue is empty
  void setQueueEmpty(bool empty);

  bool isTranslateEnabled() const; ///< If use translation in game mode
  void setTranslateEnabled(bool enabled);

  bool isSubtitleOnTop() const;
  void setSubtitleOnTop(bool t);

  bool isWindowOnTop() const;
  void setWindowOnTop(bool t);

  bool isEmbeddedPlayerOnTop() const;
  void setEmbeddedPlayerOnTop(bool t);

  bool isMenuBarVisible() const;
  void setMenuBarVisible(bool t);

  bool isAutoPlayNext() const;
  void setAutoPlayNext(bool t);

  bool isAeroEnabled() const;
  void setAeroEnabled(bool t);

  bool isMenuThemeEnabled() const;
  void setMenuThemeEnabled(bool t);

  bool isLive() const;
  void setLive(bool t);

  QDate updateDate() const;
  void setUpdateDate(const QDate &date);

  int subtitleColor() const;
  void setSubtitleColor(int colorId);

  int annotationEffect() const;
  void setAnnotationEffect(int effect);

  int language() const; ///< Application language
  void setLanguage(int lang);

  ///  Theme in UiStyle
  int themeId() const;
  void setThemeId(int tid);

  qint64 annotationLanguages() const; ///< Annotation language
  void setAnnotationLanguages(qint64 lang);

  void setMultipleWindowsEnabled(bool t);
  bool isMultipleWindowsEnabled() const;

  QStringList browserUrls() const;
  void setBrowserUrls(const QStringList &urls);
  void clearBrowserUrls();

  QStringList recentFiles() const;
  void setRecentFiles(const QStringList &files);
  //void clearRecentFiles();

  QString recentPath() const;
  void setRecentPath(const QString &path);

  bool isAnnotationFilterEnabled() const;
  void setAnnotationFilterEnabled(bool t);

  QStringList blockedKeywords() const;
  void setBlockedKeywords(const QStringList &l);

  QStringList blockedUserNames() const;
  void setBlockedUserNames(const QStringList &l);

  int annotationCountHint() const;
  void setAnnotationCountHint(int count);

  // - Accounts -

  std::pair<QString, QString> nicovideoAccount();
  void setNicovideoAccount(const QString &username, const QString &password);

  std::pair<QString, QString> bilibiliAccount();
  void setBilibiliAccount(const QString &username, const QString &password);

  // - History per token -

  QHash<qint64, qint64> playPosHistory() const;
  void setPlayPosHistory(const QHash<qint64, qint64> &h);

  QHash<qint64, int> subtitleHistory() const;
  void setSubtitleHistory(const QHash<qint64, int> &h);

  QHash<qint64, int> audioTrackHistory() const;
  void setAudioTrackHistory(const QHash<qint64, int> &h);

  QHash<qint64, QString> aspectRatioHistory() const;
  void setAspectRatioHistory(const QHash<qint64, QString> &h);

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
};

#endif // SETTINGS_H
