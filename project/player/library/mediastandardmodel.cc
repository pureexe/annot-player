// mediastandardmodel.cc
// 10/3/2011

#include "mediastandardmodel.h"
#include "media.h"
#include "tr.h"
#include "icons.h"
#include "global.h"
#include "module/player/player.h"
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QModelIndex>

#define DEBUG "mediastandardmodel"
#include "module/debug/debug.h"

#define FORMAT_SIZE(_sz) \
( \
  (_sz) < 1024 ? QString::number(_sz) + " B" : \
  (_sz) < 1024 * 1024 ? QString::number((_sz) / 1014) + " KB" : \
  QString::number((_sz) / (1024.0 * 1024), 'f', 1) + " MB" \
)

// - Construction -

MediaStandardModel::MediaStandardModel(QObject *parent)
  : Base(0, HD_Count, parent)
{ resetHeader(); }

Qt::ItemFlags
MediaStandardModel::flags(const QModelIndex &index) const
{ return Base::flags(index) & ~Qt::ItemIsEditable; } // read-only

void
MediaStandardModel::reset()
{
  //while (rowCount())
  //  removeRow(0);
  clear();
  setColumnCount(HD_Count);
  resetHeader();
}

void
MediaStandardModel::setHeaderData(QAbstractItemModel *model)
{
  model->setHeaderData(HD_Title, Qt::Horizontal, tr("Title"));
  model->setHeaderData(HD_Location, Qt::Horizontal, tr("Location"));
  model->setHeaderData(HD_Type, Qt::Horizontal, tr("Type"));
  model->setHeaderData(HD_Size, Qt::Horizontal, tr("Size"));
  model->setHeaderData(HD_VisitTime, Qt::Horizontal, tr("Time"));
  model->setHeaderData(HD_VisitCount, Qt::Horizontal, tr("Count"));
  model->setHeaderData(HD_Digest, Qt::Horizontal, tr("Digest"));
}

void
MediaStandardModel::addMedia(const Media &media)
{
#define FORMAT_TIME(_msecs)       QDateTime::fromMSecsSinceEpoch(_msecs).toString(Qt::ISODate)
#define FORMAT_COUNT(_num)        QString::number(_num)
#define FORMAT_TYPE(_media) \
( \
  (_media).isGame() ? TR(T_GALGAME) : \
  (_media).isUrl() ? TR(T_URL) : \
  (_media).isVideo() ? TR(T_VIDEO) : \
  (_media).isAudio() ? TR(T_AUDIO) : \
                       TR(T_FILE) \
)

  insertRow(0);

  setData(index(0, 0), media.icon(), Qt::DecorationRole);

  setData(index(0, HD_Title), media.title(), Qt::DisplayRole);
  setData(index(0, HD_Location), media.location(), Qt::DisplayRole);
  setData(index(0, HD_Type), FORMAT_TYPE(media), Qt::DisplayRole);
  setData(index(0, HD_Size), "-", Qt::DisplayRole);
  setData(index(0, HD_VisitTime), FORMAT_TIME(media.visitTime()), Qt::DisplayRole);
  setData(index(0, HD_VisitCount), FORMAT_COUNT(media.visitCount()), Qt::DisplayRole);

  for (int i = 0; i < HD_Count; i++)
    setData(index(0, i), headerData(i, Qt::Horizontal), Qt::ToolTipRole);

#undef FORMAT_TIME
#undef FORMAT_COUNT
}

void
MediaStandardModel::addFile(const QFileInfo &fi)
{
  insertRow(0);

  setData(index(0, 0), Icons::iconForFile(fi), Qt::DecorationRole);

  setData(index(0, HD_Title), fi.fileName(), Qt::DisplayRole);
  setData(index(0, HD_Location), fi.absoluteFilePath(), Qt::DisplayRole);
  setData(index(0, HD_Type), TR(T_VIDEO), Qt::DisplayRole);
  setData(index(0, HD_Size), FORMAT_SIZE(fi.size()), Qt::DisplayRole);
  setData(index(0, HD_VisitTime), fi.created(), Qt::DisplayRole);
  setData(index(0, HD_VisitCount), "-", Qt::DisplayRole);

  for (int i = 0; i < HD_Count; i++)
    setData(index(0, i), headerData(i, Qt::Horizontal), Qt::ToolTipRole);
}

void
MediaStandardModel::addFolder(const QString &location)
{
  QDir dir(location);
  if (!dir.exists())
    return;

  static const QStringList filters =
      Player::supportedVideoFilters() +
      Player::supportedImageFilters() +
      Player::supportedAudioFilters();

  foreach (const QFileInfo &fi, dir.entryInfoList(filters, QDir::Files))
    addFile(fi);
}

// - Queries -

QString
MediaStandardModel::indexData(const QModelIndex &input, HeaderData header)
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
