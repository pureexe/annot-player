// mediamodel.cc
// 10/3/2011

#include "mediamodel.h"
#include "media.h"
#include "tr.h"
#include "icons.h"
#include "global.h"
#include "lib/player/player.h"
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QModelIndex>

#define DEBUG "mediamodel"
#include "lib/debug/debug.h"

#define FORMAT_SIZE(_sz) \
( \
  (_sz) < 1024 ? QString::number(_sz) + " B" : \
  (_sz) < 1024 * 1024 ? QString::number((_sz) / 1014) + " KB" : \
  QString::number((_sz) / (1024.0 * 1024), 'f', 1) + " MB" \
)

#define TYPENO_GAME     0
#define TYPENO_FOLDER   1
#define TYPENO_URL      2
#define TYPENO_VIDEO    3
#define TYPENO_AUDIO    4
#define TYPENO_FILE     5

// - Construction -

MediaModel::MediaModel(QObject *parent)
  : Base(0, HD_Count, parent)
{ resetHeader(); }

Qt::ItemFlags
MediaModel::flags(const QModelIndex &index) const
{ return Base::flags(index) & ~Qt::ItemIsEditable; } // read-only

void
MediaModel::reset()
{
  //while (rowCount())
  //  removeRow(0);
  clear();
  setColumnCount(HD_Count);
  resetHeader();
}

void
MediaModel::setHeaderData(QAbstractItemModel *model)
{
  model->setHeaderData(HD_Title, Qt::Horizontal, tr("Title"));
  model->setHeaderData(HD_Location, Qt::Horizontal, tr("Location"));
  model->setHeaderData(HD_Type, Qt::Horizontal, tr("Type"));
  model->setHeaderData(HD_Size, Qt::Horizontal, tr("Size"));
  model->setHeaderData(HD_Time, Qt::Horizontal, tr("Time"));
  model->setHeaderData(HD_Counter, Qt::Horizontal, tr("Count"));
  model->setHeaderData(HD_TypeNo, Qt::Horizontal, tr("Type"));
  model->setHeaderData(HD_Digest, Qt::Horizontal, tr("Digest"));
}

// - Properties -

int
MediaModel::indexType(const QModelIndex &index)
{
  switch (indexData(index, HD_TypeNo).toInt()) {
  case TYPENO_GAME: return Media::Game;
  case TYPENO_AUDIO: return Media::Audio;
  case TYPENO_VIDEO: return Media::Video;
  case TYPENO_URL: return Media::Url;
  case TYPENO_FILE:
  case TYPENO_FOLDER:
  default:
    return 0;
  }
}

// - Actions -

void
MediaModel::addMedia(const Media &media)
{
#define FORMAT_TIME(_msecs)       QDateTime::fromMSecsSinceEpoch(_msecs).toString("yyyy/MM/dd H:m")
#define FORMAT_COUNT(_num)        QString::number(_num)
#define FORMAT_TYPE(_media) \
( \
  (_media).isGame() ?  TR(T_GALGAME) : \
  (_media).isUrl() ?   TR(T_URL) : \
  (_media).isVideo() ? TR(T_VIDEO) : \
  (_media).isAudio() ? TR(T_AUDIO) : \
                       TR(T_FILE) \
)
#define FORMAT_TYPENO(_media) \
( \
  (_media).isGame() ?  TYPENO_GAME : \
  (_media).isUrl() ?   TYPENO_URL : \
  (_media).isVideo() ? TYPENO_VIDEO : \
  (_media).isAudio() ? TYPENO_AUDIO : \
                       TYPENO_FILE \
)

  insertRow(0);

  setData(index(0, 0), media.icon(), Qt::DecorationRole);

  setData(index(0, HD_Title), media.title(), Qt::DisplayRole);
  setData(index(0, HD_Location), media.location(), Qt::DisplayRole);
  setData(index(0, HD_Type), FORMAT_TYPE(media), Qt::DisplayRole);
  setData(index(0, HD_TypeNo), FORMAT_TYPENO(media), Qt::DisplayRole);
  setData(index(0, HD_Size), "-", Qt::DisplayRole);
  setData(index(0, HD_Time), FORMAT_TIME(media.visitTime()), Qt::DisplayRole);
  setData(index(0, HD_Counter), FORMAT_COUNT(media.visitCount()), Qt::DisplayRole);
  setData(index(0, HD_Digest), media.digest(), Qt::DisplayRole);

  //for (int i = 0; i < HD_Count; i++)
  //  setData(index(0, i), headerData(i, Qt::Horizontal), Qt::ToolTipRole);

#undef FORMAT_TIME
#undef FORMAT_COUNT
}

void
MediaModel::addFile(const QFileInfo &fi)
{
  insertRow(0);

  setData(index(0, 0), Icons::iconForFile(fi), Qt::DecorationRole);

  setData(index(0, HD_Title), fi.fileName(), Qt::DisplayRole);
  setData(index(0, HD_Location), fi.absoluteFilePath(), Qt::DisplayRole);
  setData(index(0, HD_Type), TR(T_VIDEO), Qt::DisplayRole);
  setData(index(0, HD_TypeNo), TYPENO_VIDEO, Qt::DisplayRole);
  setData(index(0, HD_Size), FORMAT_SIZE(fi.size()), Qt::DisplayRole);
  setData(index(0, HD_Time), fi.created(), Qt::DisplayRole);
  setData(index(0, HD_Counter), "-", Qt::DisplayRole);

  //for (int i = 0; i < HD_Count; i++)
  //  setData(index(0, i), headerData(i, Qt::Horizontal), Qt::ToolTipRole);
}

void
MediaModel::addFile(const QFileInfo &fi, QStandardItem *dirItem)
{
  if (!dirItem) {
    addFile(fi);
    return;
  }

  QStandardItem *i = new QStandardItem;
  i->setData(Icons::iconForFile(fi), Qt::DecorationRole);
  i->setData(fi.fileName(), Qt::DisplayRole);
  dirItem->insertRow(0, i);

  i = new QStandardItem;
  i->setData(fi.absoluteFilePath(), Qt::DisplayRole);
  dirItem->setChild(0, HD_Location, i);

  i = new QStandardItem;
  i->setData(TR(T_VIDEO), Qt::DisplayRole);
  dirItem->setChild(0, HD_Type, i);

  i = new QStandardItem;
  i->setData(TYPENO_VIDEO, Qt::DisplayRole);
  dirItem->setChild(0, HD_TypeNo, i);

  i = new QStandardItem;
  i->setData(FORMAT_SIZE(fi.size()), Qt::DisplayRole);
  dirItem->setChild(0, HD_Size, i);

  i = new QStandardItem;
  i->setData(fi.created(), Qt::DisplayRole);
  dirItem->setChild(0, HD_Time, i);

  i = new QStandardItem;
  i->setData("-", Qt::DisplayRole);
  dirItem->setChild(0, HD_Counter, i);
}

void
MediaModel::addFolder(const QString &location)
{
  QDir dir(location);
  if (!dir.exists())
    return;

  static const QStringList filters =
      Player::supportedVideoFilters() +
      Player::supportedImageFilters() +
      Player::supportedAudioFilters();

  QFileInfoList l = dir.entryInfoList(filters, QDir::Files);
  if (l.isEmpty())
    return;

  QString dirName = dir.dirName();
  if (dirName.isEmpty())
    dirName = '/';

  insertRow(0);
  setData(index(0, 0), Icons::folderIcon(), Qt::DecorationRole);
  setData(index(0, HD_Title), dirName, Qt::DisplayRole);
  setData(index(0, HD_Location), dir.absolutePath(), Qt::DisplayRole);
  setData(index(0, HD_Type), TR(T_FOLDER), Qt::DisplayRole);
  setData(index(0, HD_TypeNo), TYPENO_FOLDER, Qt::DisplayRole);
  setData(index(0, HD_Size), "-", Qt::DisplayRole);
  setData(index(0, HD_Time), QFileInfo(location).created(), Qt::DisplayRole);
  setData(index(0, HD_Counter), l.size(), Qt::DisplayRole);

  //for (int i = 0; i < HD_Count; i++)
  //  setData(index(0, i), headerData(i, Qt::Horizontal), Qt::ToolTipRole);

  QStandardItem *dirItem = item(0);
  foreach (const QFileInfo &fi, l) {
    addFile(fi, dirItem);
    addFile(fi);
  }
}

// - Queries -

QString
MediaModel::indexData(const QModelIndex &input, HeaderData header)
{
  QString ret;
  if (!input.isValid())
    return ret;
  int row = input.row();
  QModelIndex index = input.sibling(row, header);
  if (!index.isValid())
    return ret;
  ret = index.data().toString();
  return ret;
}

// EOF
