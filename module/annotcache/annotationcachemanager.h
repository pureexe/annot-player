#ifndef ANNOTATIONCACHEMANAGER_H
#define ANNOTATIONCACHEMANAGER_H

// annotationcachemanager.h
// 6/27/2012

#include <QtCore/QByteArray>
#include <QtCore/QObject>
#include <QtCore/QString>

QT_FORWARD_DECLARE_CLASS(QDateTime)

class AnnotationCacheManager : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationCacheManager)
  typedef AnnotationCacheManager Self;
  typedef QObject Base;

  QString location_;

public:
  static Self *globalInstance() { static Self g; return &g; }

protected:
  explicit AnnotationCacheManager(QObject *parent = nullptr)
    : Base(parent) { }

public:
  const QString &location() const { return location_; }
  void setLocation(const QString &path) { location_ = path; }

  bool saveData(const QByteArray &data, const QString &url);
  bool saveFile(const QString &dataFile, const QString &url);
  QString findFile(const QString &url);

  static QString hashFileName(const QString &url);

protected:
  QString cacheFile(const QString &url) const;

  static QString xmlHeader(const QString &url, const QDateTime &ts);
  static QString jsonHeader(const QString &url, const QDateTime &ts);
};

#endif // ANNOTATIONCACHEMANAGER_H
