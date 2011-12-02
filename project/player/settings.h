#ifndef SETTINGS_H
#define SETTINGS_H

// settings.h
// 7/30/2011

#include <QSettings>
#include <QStringList>
#include <QDate>

class Settings : protected QSettings
{
  Q_OBJECT
  typedef Settings Self;
  typedef QSettings Base;

  // - Constructions -
public:
  static Self *globalInstance();

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

  bool isSubtitleStaysOnTop() const;
  void setSubtitleStaysOnTop(bool t);

  bool isEmbeddedPlayerStaysOnTop() const;
  void setEmbeddedPlayerStaysOnTop(bool t);

  bool isAutoPlayNext() const;
  void setAutoPlayNext(bool t);

  QDate updateDate() const;
  void setUpdateDate(const QDate &date);

  int language() const; ///< Application language
  void setLanguage(int lang);

  ///  Theme in UiStyle
  int themeId() const;
  void setThemeId(int tid);

  qint64 annotationLanguages() const; ///< Annotation language
  void setAnnotationLanguages(qint64 lang);

  QStringList recentFiles() const;
  void setRecentFiles(const QStringList &files);
  void clearRecentFiles();

  QString recentPath() const;
  void setRecentPath(const QString &path);

  // - Constructions -
protected:
  explicit Settings(QObject *parent = 0);
};

#endif // SETTINGS_H
