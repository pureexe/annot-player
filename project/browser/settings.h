#ifndef SETTINGS_H
#define SETTINGS_H

// settings.h
// 7/30/2011

#include <QSettings>
#include <QStringList>
#include <QDate>
#include <QHash>
#include <QUrl>
#include <utility>

class Settings : public QSettings
{
  Q_OBJECT
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

  QStringList recentTabs() const;
  void setRecentTabs(const QStringList &urls, int limit = 0);
  void clearRecentTabs();

  QList<QUrl> closedUrls() const;
  void setClosedUrls(const QList<QUrl> &urls, int limit = 0);
  void clearClosedUrls();
};

#endif // SETTINGS_H
