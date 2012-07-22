#ifndef SETTINGS_H
#define SETTINGS_H

// settings.h
// 7/30/2011

#include "module/downloadtask/downloadtask.h"
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

  DownloadTaskInfoList recentTasks() const;
  void setRecentTasks(const DownloadTaskInfoList &l);

  QSize recentSize() const;
  void setRecentSize(const QSize &size);
  void clearRecentSize();
};

#endif // SETTINGS_H
