#ifndef FILTEREDTABLEVIEW_H
#define FILTEREDTABLEVIEW_H

// filteredtableview.h
// 11/17/2011

#include "processinfo.h"
#include <QWidget>
#include <QModelIndex>
#include <QHash>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QComboBox;
class QLineEdit;
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
  explicit FilteredTableView(QStandardItemModel *sourceModel, QWidget *parent = 0);

signals:
  void currentIndexChanged(QModelIndex index);

  // - Properties -
public:
  ///  Never return null
  QSortFilterProxyModel *proxyModel()                   { return proxyModel_; }
  const QSortFilterProxyModel *proxyModel() const       { return proxyModel_; }

  QModelIndex currentIndex() const;
  void removeCurrentRow();

  // - Slots -
public slots:
  void clear();
private slots:
  void invalidateFilterRegExp();
  void invalidateFilterColumn();

  // - Implementaions
private:
  void createLayout();

private:
  QStandardItemModel *sourceModel_;
  QSortFilterProxyModel *proxyModel_;
  QTreeView *proxyView_;

  QLineEdit *filterPatternLineEdit_;
  QComboBox *filterSyntaxComboBox_;
  QComboBox *filterColumnComboBox_;
};

#endif // FILTEREDTABLEVIEW_H
