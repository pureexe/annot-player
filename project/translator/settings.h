#ifndef SETTINGS_H
#define SETTINGS_H

// settings.h
// 7/30/2011

#include <QtCore/QSettings>
#include <QtCore/QSize>

class Settings : public QSettings
{
  Q_OBJECT
  Q_DISABLE_COPY(Settings)
  typedef Settings Self;
  typedef QSettings Base;

  // - Constructions -
public:
  static Self *globalSettings() { static Self g; return &g; }
protected:
  explicit Settings(QObject *parent = nullptr);

public slots:
  void sync() { Base::sync(); }

  // - Properties -
public:
  QString version() const;
  void setVersion(const QString &version);

  QSize recentSize() const;
  void setRecentSize(const QSize &size);
  void clearRecentSize();

  int languageIndex() const;
  void setLanguageIndex(int index);
};

#endif // SETTINGS_H
