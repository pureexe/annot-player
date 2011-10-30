#ifndef SETTINGS_H
#define SETTINGS_H

// settings.h
// 7/30/2011

#include <QSettings>
#include <QStringList>

class Settings : public QSettings
{
  Q_OBJECT
  typedef Settings Self;
  typedef QSettings Base;

  // - Constructions -
public:
  static Self *globalInstance();

  // - Properties -
public:
  qint64 userId() const;
  void setUserId(qint64 uid);

  QString userName() const;
  void setUserName(const QString &userName);

  QString password() const; ///< Encrypted
  void setPassword(const QString &password);

  bool isQueueEmpty() const; ///< If database queue is empty
  void setQueueEmpty(bool empty);

  int language() const; ///< Application language
  void setLanguage(int lang);

  qint64 annotationLanguages() const; ///< Annotation language
  void setAnnotationLanguages(qint64 lang);

  QStringList recent() const;
  void setRecent(const QStringList &files);
  void clearRecent();

  // - Constructions -
protected:
  explicit Settings(QObject *parent = 0);
};

#endif // SETTINGS_H
