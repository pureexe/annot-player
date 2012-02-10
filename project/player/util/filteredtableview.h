#ifndef FILTEREDTABLEVIEW_H
#define FILTEREDTABLEVIEW_H

// filteredtableview.h
// 11/17/2011

#include "processinfo.h"
#include <QWidget>
#include <QModelIndex>
#include <QHash>

QT_BEGIN_NAMESPACE
class QComboBox;
class QMenu;
class QStandardItemModel;
class QSortFilterProxyModel;
class QTreeView;
class QToolButton;
QT_END_NAMESPACE

///  This class is incompleted.sourceModel and proxyModel are missing data.
class FilteredTableView : public QWidget
{
  Q_OBJECT
  typedef FilteredTableView Self;
  typedef QWidget Base;

public:
  FilteredTableView(QStandardItemModel *sourceModel, QSortFilterProxyModel *proxyModel, QWidget *parent = 0);

signals:
  void currentIndexChanged(QModelIndex index);

  // - Properties -
public:
  QModelIndex currentIndex() const;
  void removeCurrentRow();

  // - Actions -
public slots:
  void clear();
  void setCurrentColumn(int col);
  void sortByColumn(int col, Qt::SortOrder order);
  void invalidateCount();

protected slots:
  void popup();
  void invalidateFilterRegExp();
  void invalidateFilterColumn();

  // - Implementaions
private:
  void createLayout();

private:
  QStandardItemModel *sourceModel_;
  QSortFilterProxyModel *proxyModel_;
  QTreeView *proxyView_;

  QComboBox *filterPatternEdit_;
  QComboBox *filterSyntaxComboBox_;
  QComboBox *filterColumnComboBox_;

  QToolButton *countButton_;
};

#endif // FILTEREDTABLEVIEW_H
