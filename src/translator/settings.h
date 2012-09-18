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

  int language() const;
  void setLanguage(int codepage);

  int dictionary() const;
  void setDictionary(int index);

  ulong translationServices() const;
  void setTranslationServices(ulong mask);

  bool monitorClipboard() const;
  void setMonitorClipboard(bool value);

  bool windowOnTop() const;
  void setWindowOnTop(bool value);

  bool isAtlasEnabled() const;
  void setAtlasEnabled(bool value);

  bool isEdictEnabled() const;
  void setEdictEnabled(bool value);
};

#endif // SETTINGS_H
