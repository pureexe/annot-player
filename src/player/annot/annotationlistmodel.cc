// annotationlistmodel.cc
// 10/3/2011

#include "annotationlistmodel.h"
#include "tr.h"
#include "signalhub.h"
#include "qtx/qxdatetime.h"
#include "lib/annotcloud/traits.h"
#include <QtCore/QModelIndex>

// - Construction -

AnnotationListModel::AnnotationListModel(SignalHub *hub, QObject *parent)
  : Base(0, HD_Count, parent), hub_(hub)
{ resetHeader(); }

Qt::ItemFlags
AnnotationListModel::flags(const QModelIndex &index) const
{ return Base::flags(index) & ~Qt::ItemIsEditable; } // read-only

void
AnnotationListModel::setHeaderData(QAbstractItemModel *model)
{
  Q_ASSERT(model);
  model->setHeaderData(HD_Text, Qt::Horizontal, TR(T_TEXT));
  model->setHeaderData(HD_Pos, Qt::Horizontal, TR(T_POS));
  model->setHeaderData(HD_UserAlias, Qt::Horizontal, TR(T_USER_ALIAS));
  model->setHeaderData(HD_CreateTime, Qt::Horizontal, TR(T_CREATETIME));
  model->setHeaderData(HD_UpdateTime, Qt::Horizontal, TR(T_UPDATETIME));
  model->setHeaderData(HD_Id, Qt::Horizontal, TR(T_ID));
  model->setHeaderData(HD_Language, Qt::Horizontal, TR(T_LANGUAGE));
  model->setHeaderData(HD_Status, Qt::Horizontal, TR(T_STATUS));
  model->setHeaderData(HD_UserId, Qt::Horizontal, TR(T_USER_ID));
  model->setHeaderData(HD_Flags, Qt::Horizontal, TR(T_FLAGS));
  model->setHeaderData(HD_BlessCount, Qt::Horizontal, TR(T_BLESSCOUNT));
  model->setHeaderData(HD_CurseCount, Qt::Horizontal, TR(T_CURSECOUNT));
  model->setHeaderData(HD_BlockCount, Qt::Horizontal, TR(T_BLOCKCOUNT));
}

void
AnnotationListModel::addAnnotation(const Annotation &a)
{
#define FORMAT_TIME(_secs)        QDateTime::fromMSecsSinceEpoch(_secs * 1000).toString(Qt::ISODate)
#define FORMAT_POS(_msecs)        qxTimeFromMsec(_msecs).toString()
#define FORMAT_LANGUAGE(_lang)    languageToString(_lang)
#define FORMAT_FLAGS(_flags)      annotationFlagsToStringList(_flags)
#define FORMAT_STATUS(_status)    annotationStatusToString(_status)

  insertRow(0);

  setData(index(0, HD_Text), a.text(), Qt::DisplayRole);
  if (hub_->isMediaTokenMode())
    setData(index(0, HD_Pos), FORMAT_POS(a.pos()), Qt::DisplayRole);
  else
    setData(index(0, HD_Pos), a.pos(), Qt::DisplayRole);
  setData(index(0, HD_UserAlias), a.userAlias(), Qt::DisplayRole);
  if (a.createTime())
    setData(index(0, HD_CreateTime), FORMAT_TIME(a.createTime()), Qt::DisplayRole);
  else
    setData(index(0, HD_CreateTime), TR(T_NA), Qt::DisplayRole);
  if (a.updateTime())
    setData(index(0, HD_UpdateTime), FORMAT_TIME(a.updateTime()), Qt::DisplayRole);
  else
    setData(index(0, HD_UpdateTime), TR(T_NA), Qt::DisplayRole);

  setData(index(0, HD_Id), a.id(), Qt::DisplayRole);
  setData(index(0, HD_Language), FORMAT_LANGUAGE(a.language()), Qt::DisplayRole);
  setData(index(0, HD_Status), FORMAT_STATUS(a.status()), Qt::DisplayRole);

  setData(index(0, HD_UserId), a.userId(), Qt::DisplayRole);
  setData(index(0, HD_Flags), FORMAT_FLAGS(a.flags()), Qt::DisplayRole);
  setData(index(0, HD_BlessCount), a.blessCount(), Qt::DisplayRole);
  setData(index(0, HD_CurseCount), a.curseCount(), Qt::DisplayRole);
  setData(index(0, HD_BlockCount), a.blockCount(), Qt::DisplayRole);

  //for (int i = 0; i < HD_Count; i++)
  //  setData(index(0, i), headerData(i, Qt::Horizontal), Qt::ToolTipRole);

#undef FORMAT_TIME
#undef FORMAT_POS
#undef FORMAT_LANGUAGE
#undef FORMAT_FLAGS
#undef FORMAT_STATUS
}

// - Queries -

int
AnnotationListModel::rowById(qint64 id) const
{
  for (int row = 0; row < rowCount(); row++) {
    QModelIndex i = index(row, HD_Id);
    bool ok;
    qint64 rowId = data(i).toLongLong(&ok);
    if (ok && rowId == id)
      return row;
  }
  return -1;
}

qint64
AnnotationListModel::indexId(const QModelIndex &input)
{
  if (!input.isValid())
    return 0;
  int row = input.row();
  QModelIndex index = input.sibling(row, HD_Id);
  return index.isValid() ? index.data().toLongLong() : 0;
}

qint64
AnnotationListModel::indexUserId(const QModelIndex &input)
{
  if (!input.isValid())
    return 0;
  int row = input.row();
  QModelIndex index = input.sibling(row, HD_UserId);
  return index.isValid() ? index.data().toLongLong() : 0;
}

QString
AnnotationListModel::indexUserAlias(const QModelIndex &input)
{
  if (!input.isValid())
    return QString();

  int row = input.row();
  QModelIndex index = input.sibling(row, HD_UserAlias);
  return !index.isValid() ? QString() :
    index.data().toString();
}

QString
AnnotationListModel::indexText(const QModelIndex &input)
{
  if (!input.isValid())
    return QString();

  int row = input.row();
  QModelIndex index = input.sibling(row, HD_Text);
  return !index.isValid() ? QString() :
    index.data().toString();
}

// - Formatter -

QString
AnnotationListModel::languageToString(int lang)
{
  switch(lang) {
  //case Traits::AnyLanguage:     return TR(T_ANYLANGUAGE);
  case Traits::English:         return TR(T_ENGLISH);
  case Traits::Japanese:        return TR(T_JAPANESE);
  case Traits::TraditionalChinese:     return TR(T_CHINESE);
  case Traits::SimplifiedChinese:     return TR(T_SIMPLIFIEDCHINESE);
  case Traits::Korean:          return TR(T_KOREAN);
  case Traits::French:          return TR(T_FRENCH);
  case Traits::German:          return TR(T_GERMAN);
  case Traits::Italian:         return TR(T_ITALIAN);
  case Traits::Spanish:         return TR(T_SPANISH);
  case Traits::Portuguese:      return TR(T_PORTUGUESE);
  case Traits::Russian:         return TR(T_RUSSIAN);

  //case Traits::UnknownLanguage:
  default:                      return TR(T_ALIEN);
  }
}

QStringList
AnnotationListModel::annotationFlagsToStringList(int flags)
{
  QStringList ret;
  if (flags & Annotation::AF_Anonymous)
    ret.append(TR(T_ANONYMOUS));

  if (ret.isEmpty())
    ret.append(TR(T_NA));
  return ret;
}

QString
AnnotationListModel::annotationStatusToString(int status)
{
  switch (status) {
  case Annotation::AS_Deleted:  return TR(T_DELETED);
  case Annotation::AS_Active:   return TR(T_ACTIVE);
  case Annotation::AS_Blocked:  return TR(T_BLOCKED);
  default: return TR(T_NA);
  }
}

// - Actions -

void
AnnotationListModel::updateAnnotationTextWithId(const QString &text, qint64 id)
{
  int row = rowById(id);
  if (row >= 0) {
    setData(index(row, HD_Text), text, Qt::DisplayRole);
    setData(index(row, HD_UpdateTime), QDateTime::currentDateTime(), Qt::DisplayRole);
  }
}

void
AnnotationListModel::updateAnnotationUserIdWithId(qint64 userId, qint64 id)
{
  int row = rowById(id);
  if (row >= 0)
    setData(index(row, HD_UserId), userId, Qt::DisplayRole);
}

// EOF
