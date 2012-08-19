#ifndef MEDIALIBRARY_H
#define MEDIALIBRARY_H

// medialibrary.h
// 8/5/2012

#include "media.h"
#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QString>

QT_FORWARD_DECLARE_CLASS(QStandardItemModel)

class MediaModel;
class MediaLibrary : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(MediaLibrary)
  typedef MediaLibrary Self;
  typedef QObject Base;

  QMutex mutex_;
  QHash<QString, Media> library_;
  QString libraryLocation_;
  bool dirty_;

  mutable MediaModel *model_;

  QSet<QString> folders_;
  QSet<Media> games_;
  QSet<Media> urls_;

public:
  explicit MediaLibrary(const QString &xmlLocation, QObject *parent = nullptr);

  bool isDirty() const { return dirty_; }

  QStandardItemModel *model() const;

  const QSet<QString> &folders() const;
  const QSet<Media> &games() const;
  const QSet<Media> &urls() const;

  bool exists() const;

public slots:
  void save();
  void load();

  void refresh();
  void clear();

  void visit(const Media &media);

protected:
  void loadFolders();
  void loadGames();
  void loadUrls();
};

#endif // MEDIALIBRARY_H
