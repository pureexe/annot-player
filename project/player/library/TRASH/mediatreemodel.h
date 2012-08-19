#ifndef MEDIATREEMODEL_H
#define MEDIATREEMODEL_H

// mediatreemodel.h
// 8/17/2012

#include "module/annotcloud/annotation.h"
#include <QtCore/QFileInfo>
#include <QtGui/QStandardItemModel>

class Media;
class MediaTreeModel : public QStandardItemModel
{
  Q_OBJECT
  //Q_DISABLE_COPY(MediaTreeModel)
  typedef MediaTreeModel Self;
  typedef QStandardItemModel Base;

public:
  enum HeaderData {
    HD_Title = 0,
    HD_Location,
    HD_Type,
    HD_Size,
    HD_Time,
    HD_Counter,
    HD_Digest,
    HD_Count
  };

public:
  explicit MediaTreeModel(QObject *parent = nullptr);

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  // - Queries -
public:
  static QString indexTitle(const QModelIndex &index)
  { return indexData(index, HD_Title); }

  static QString indexLocation(const QModelIndex &index)
  { return indexData(index, HD_Location); }

  static QString indexDigest(const QModelIndex &index)
  { return indexData(index, HD_Digest); }

protected:
  static QString indexData(const QModelIndex &index, HeaderData header);

  // - Properties -
public:
  bool isEmpty() const { return rowCount() == 0; }

public slots:
  void addMedia(const Media &media);
  void addFile(const QFileInfo &fi, QStandardItem *dirItem);
  void addFolder(const QString &location);

  void reset();

protected:
  void resetHeader() { setHeaderData(this); }
  static void setHeaderData(QAbstractItemModel *model);
};

#endif // MEDIATREEMODEL_H
