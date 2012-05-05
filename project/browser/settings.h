#ifndef SETTINGS_H
#define SETTINGS_H

// settings.h
// 7/30/2011

#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <QtCore/QSize>
#include <QtCore/QUrl>

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
  explicit Settings(QObject *parent = 0);

public slots:
  void sync() { Base::sync(); }

  // - Properties -
public:
  QString version() const;
  void setVersion(const QString &version);

  // - History -
  QStringList recentUrls() const;
  void setRecentUrls(const QStringList &urls, int limit = 0);
  void clearRecentUrls();

  QStringList recentSearches() const;
  void setRecentSearches(const QStringList &l, int limit = 0);

  QStringList recentTabs() const;
  bool hasRecentTabs() const;
  void setRecentTabs(const QStringList &urls, int limit = 0);
  void clearRecentTabs();

  QList<QUrl> closedUrls() const;
  void setClosedUrls(const QList<QUrl> &urls, int limit = 0);
  void clearClosedUrls();

  int recentTabIndex() const;
  void setRecentTabIndex(int value);

  QSize recentSize() const;
  void setRecentSize(const QSize &value);
  void clearRecentSize();

  bool isFullScreen() const;
  void setFullScreen(bool t);

  int searchEngine() const;
  void setSearchEngine(int engine);
};

#endif // SETTINGS_H
