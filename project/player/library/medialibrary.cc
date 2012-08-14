// medialibrary.cc
// 8/5/2012

#include "medialibrary.h"
#include "mediastandardmodel.h"
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMutexLocker>

#define DEBUG "medialibrary"
#include "module/debug/debug.h"

// - Construction -

MediaLibrary::MediaLibrary(const QString &xmlLocation, QObject *parent)
  : Base(parent), libraryLocation_(xmlLocation), dirty_(false), standardModel_(0), treeModel_(0)
{ }

// - Properties -

QStandardItemModel*
MediaLibrary::standardModel() const
{
  if (!standardModel_)
    standardModel_ = new MediaStandardModel(const_cast<Self *>(this));
  return standardModel_;
}

QAbstractItemModel*
MediaLibrary::treeModel() const
{
  return treeModel_;
}

const QSet<QString>&
MediaLibrary::folders() const
{
  if (folders_.isEmpty() && dirty_)
    const_cast<Self *>(this)->loadFolders();
  return folders_;
}

void
MediaLibrary::loadFolders()
{
  foreach (const Media &media, library_)
    if (!media.isGame() && !media.isUrl())
      folders_.insert(QFileInfo(media.location()).absolutePath());
}

const QSet<Media>&
MediaLibrary::games() const
{
  if (games_.isEmpty() && dirty_)
    const_cast<Self *>(this)->loadGames();
  return games_;
}

void
MediaLibrary::loadGames()
{
  foreach (const Media &media, library_)
    if (media.isGame())
      games_.insert(media);
}

const QSet<Media>&
MediaLibrary::urls() const
{
  if (urls_.isEmpty() && dirty_)
    const_cast<Self *>(this)->loadUrls();
  return urls_;
}

void
MediaLibrary::loadUrls()
{
  foreach (const Media &media, library_)
    if (media.isUrl())
      urls_.insert(media);
}

// - Actions -

void
MediaLibrary::load()
{
  //DOUT("locking");
  //QMutexLocker lock(&mutex_);
  //DOUT("locked");

  library_ = Media::readHash(libraryLocation_);
  dirty_ = true;
  DOUT("library size =" << library_.size());
}

void
MediaLibrary::save()
{
  DOUT("library size =" << library_.size());

  DOUT("locking");
  QMutexLocker lock(&mutex_);
  DOUT("locked");

  if (library_.isEmpty())
    QFile::remove(libraryLocation_);
  else
    Media::write(library_, libraryLocation_);
}

void
MediaLibrary::clear()
{
  library_.clear();
  dirty_ = true;
}

void
MediaLibrary::refresh()
{
  QMutexLocker locker(&mutex_);

  dirty_ = false;

  if (!folders_.isEmpty())
    folders_.clear();
  loadFolders();

  if (!games_.isEmpty())
    games_.clear();
  loadGames();

  if (!urls_.isEmpty())
    urls_.clear();
  loadUrls();

  if (!standardModel_)
    standardModel_ = new MediaStandardModel(this);

  if (!standardModel_->isEmpty())
    standardModel_->reset();

  foreach (const Media &media, games_)
    standardModel_->addMedia(media);
  foreach (const Media &media, urls_)
    standardModel_->addMedia(media);
  foreach (const QString &folder, folders_)
    standardModel_->addFolder(folder);
}

// - Visit -

void
MediaLibrary::visit(const Media &media)
{
  if (!media.hasLocation())
    return;
  QString k = media.key();
  Q_ASSERT(!k.isEmpty());

  DOUT("enter: type =" << media.type() << ", location =" << media.location() << ", title =" << media.title());

  DOUT("locking");
  QMutexLocker lock(&mutex_);
  DOUT("locked");

  auto p = library_.find(k);
  if (p == library_.end()) {
    library_[k] = media;
    dirty_ = true;
  } else {
    p->visitCount() += media.visitCount();
    p->setVisitTime(media.visitTime());
    if (media.hasLocation())
      p->setLocation(media.location());
    if (media.hasTitle())
      p->setTitle(media.title());
    if (media.hasDigest())
      p->setDigest(media.digest());
  }

  DOUT("exit");
}

// EOF
