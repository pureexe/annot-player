// blacklistview_p.cc
// 11/17/2011

#include "blacklistview_p.h"
#include "tr.h"
#include "global.h"
#include "annotationbrowser.h" // TO BE REMOVED
#include "project/common/acfilteredtableview.h"
#include "project/common/acfilteredlistview.h"
#include "module/qtext/datetime.h"
#include <QtGui>

using namespace AnnotCloud;

// + TextFilterView +

TextFilterView::TextFilterView(QWidget *parent)
  : Base(parent)
{
  // Components
  sourceModel_ = new QStandardItemModel(0, HD_Count, this);
  setHeaderData(sourceModel_);

  proxyModel_ = new QSortFilterProxyModel; {
    proxyModel_->setSourceModel(sourceModel_);
    proxyModel_->setDynamicSortFilter(true);
    proxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
  }

  tableView_ = new AcFilteredListView(sourceModel_, proxyModel_, this);

  // Layout
  QLayout *layout = new QHBoxLayout; {
    layout->addWidget(tableView_);
    layout->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);
  }
  setLayout(layout);
}

void
TextFilterView::setHeaderData(QAbstractItemModel *model)
{
  Q_ASSERT(model);
  model->setHeaderData(HD_Text, Qt::Horizontal,TR(T_TEXT));
}

QLineEdit*
TextFilterView::lineEdit() const
{ return tableView_->lineEdit(); }

void
TextFilterView::clear()
{
  tableView_->clear();
  setHeaderData(sourceModel_);
}

void
TextFilterView::setRows(const QStringList &l)
{
  clear();
  foreach (const QString &t, l)
    addRow(t);
}

void
TextFilterView::addRow(const QString &text)
{
  sourceModel_->insertRow(0);
  sourceModel_->setData(sourceModel_->index(0, HD_Text), text);
  tableView_->updateCount();
}

QString
TextFilterView::current() const
{
  QString ret;
  QModelIndex mi = tableView_->currentIndex();
  if (mi.isValid())
    ret = mi.data().toString();
  return ret;
}

void
TextFilterView::removeCurrent()
{ tableView_->removeCurrentRow(); }

// + UserFilterView +

UserFilterView::UserFilterView(QWidget *parent)
  : Base(parent)
{
  // Components
  sourceModel_ = new QStandardItemModel(0, HD_Count, this);
  setHeaderData(sourceModel_);

  proxyModel_ = new QSortFilterProxyModel; {
    proxyModel_->setSourceModel(sourceModel_);
    proxyModel_->setDynamicSortFilter(true);
    proxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
  }

  tableView_ = new AcFilteredListView(sourceModel_, proxyModel_, this);

  // Layout
  QLayout *layout = new QHBoxLayout; {
    layout->addWidget(tableView_);
    layout->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);
  }
  setLayout(layout);
}

QLineEdit*
UserFilterView::lineEdit() const
{ return tableView_->lineEdit(); }

void
UserFilterView::setHeaderData(QAbstractItemModel *model)
{
  Q_ASSERT(model);
  model->setHeaderData(HD_UserAlias, Qt::Horizontal,TR(T_USER_ALIAS));
}

void
UserFilterView::clear()
{
  tableView_->clear();
  setHeaderData(sourceModel_);
}

void
UserFilterView::setRows(const QStringList &l)
{
  clear();
  foreach (const QString &t, l)
    addRow(t);
}

void
UserFilterView::addRow(const QString &text)
{
  sourceModel_->insertRow(0);
  sourceModel_->setData(sourceModel_->index(0, HD_UserAlias), text);
  tableView_->updateCount();
}

QString
UserFilterView::current() const
{
  QString ret;
  QModelIndex mi = tableView_->currentIndex();
  if (mi.isValid())
    ret = mi.data().toString();
  return ret;
}

void
UserFilterView::removeCurrent()
{ tableView_->removeCurrentRow(); }

// + AnnotationFilterView +

AnnotationFilterView::AnnotationFilterView(QWidget *parent)
  : Base(parent)
{
  // Components
  sourceModel_ = new QStandardItemModel(0, HD_Count, this);
  setHeaderData(sourceModel_);

  proxyModel_ = new QSortFilterProxyModel; {
    proxyModel_->setSourceModel(sourceModel_);
    proxyModel_->setDynamicSortFilter(true);
    proxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
  }

  tableView_ = new AcFilteredTableView(sourceModel_, proxyModel_, this);

  // Layout
  QLayout *layout = new QHBoxLayout; {
    layout->addWidget(tableView_);
    layout->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);
  }
  setLayout(layout);
}

void
AnnotationFilterView::setHeaderData(QAbstractItemModel *model)
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
  model->setHeaderData(HD_BlessedCount, Qt::Horizontal, TR(T_BLESSEDCOUNT));
  model->setHeaderData(HD_CursedCount, Qt::Horizontal, TR(T_CURSEDCOUNT));
  model->setHeaderData(HD_BlockedCount, Qt::Horizontal, TR(T_BLOCKEDCOUNT));
}

void
AnnotationFilterView::clear()
{
  tableView_->clear();
  setHeaderData(sourceModel_);
}

void
AnnotationFilterView::setRows(const AnnotationList &l)
{
  clear();
  foreach (const Annotation &a, l)
    addRow(a);
}

void
AnnotationFilterView::addRow(const Annotation &a)
{
#define FORMAT_TIME(_secs)        QDateTime::fromMSecsSinceEpoch(_secs * 1000).toString(Qt::ISODate)
#define FORMAT_POS(_msecs)        QtExt::msecs2time(_msecs).toString()
#define FORMAT_LANGUAGE(_lang)    AnnotationBrowser::languageToString(_lang)
#define FORMAT_FLAGS(_flags)      AnnotationBrowser::annotationFlagsToStringList(_flags)
#define FORMAT_STATUS(_status)    AnnotationBrowser::annotationStatusToString(_status)

  sourceModel_->insertRow(0);

  sourceModel_->setData(sourceModel_->index(0, HD_Text), a.text(), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_Pos), FORMAT_POS(a.pos()), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_UserAlias), a.userAlias(), Qt::DisplayRole);
  if (a.createTime())
    sourceModel_->setData(sourceModel_->index(0, HD_CreateTime), FORMAT_TIME(a.createTime()), Qt::DisplayRole);
  else
    sourceModel_->setData(sourceModel_->index(0, HD_CreateTime), TR(T_NA), Qt::DisplayRole);
  if (a.updateTime())
    sourceModel_->setData(sourceModel_->index(0, HD_UpdateTime), FORMAT_TIME(a.updateTime()), Qt::DisplayRole);
  else
    sourceModel_->setData(sourceModel_->index(0, HD_UpdateTime), TR(T_NA), Qt::DisplayRole);

  sourceModel_->setData(sourceModel_->index(0, HD_Id), a.id(), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_Language), FORMAT_LANGUAGE(a.language()), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_Status), FORMAT_STATUS(a.status()), Qt::DisplayRole);

  sourceModel_->setData(sourceModel_->index(0, HD_UserId), a.userId(), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_Flags), FORMAT_FLAGS(a.flags()), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_BlessedCount), a.blessedCount(), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_CursedCount), a.cursedCount(), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_BlockedCount), a.blockedCount(), Qt::DisplayRole);

  for (int i = 0; i < HD_Count; i++)
    sourceModel_->setData(sourceModel_->index(0, i), sourceModel_->headerData(i, Qt::Horizontal), Qt::ToolTipRole);

#undef FORMAT_TIME
#undef FORMAT_POS
#undef FORMAT_LANGUAGE
#undef FORMAT_FLAGS
#undef FORMAT_STATUS

  tableView_->updateCount();
}

qint64
AnnotationFilterView::currentAnnotationId() const
{
  QModelIndex mi = tableView_->currentIndex();
  if (!mi.isValid())
    return 0;
  mi = mi.sibling(mi.row(), HD_Id);
  if (!mi.isValid())
    return 0;
  bool ok;
  qint64 ret = mi.data().toLongLong(&ok);
  if (!ok)
      return 0;
  return ret;
}

void
AnnotationFilterView::removeCurrent()
{ tableView_->removeCurrentRow(); }

// EOF
