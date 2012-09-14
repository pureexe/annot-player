#ifndef FILTEREDTABLEVIEW_H
#define FILTEREDTABLEVIEW_H

// filteredtableview.h
// 11/17/2011

#include <QtCore/QHash>
#include <QtCore/QModelIndex>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QComboBox;
class QMenu;
class QSortFilterProxyModel;
class QToolButton;
class QTreeView;
QT_END_NAMESPACE

///  This class is incompleted.sourceModel and proxyModel are missing data.
class AcFilteredTableView : public QWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(AcFilteredTableView)
  typedef AcFilteredTableView Self;
  typedef QWidget Base;

public:
  AcFilteredTableView(QAbstractItemModel *sourceModel, QSortFilterProxyModel *proxyModel, QWidget *parent = nullptr);

signals:
  void currentIndexChanged(QModelIndex index);

  // - Properties -
public:
  QModelIndex currentIndex() const;
  void removeCurrentRow();

  QTreeView *view() const { return proxyView_; }

  // - Actions -
public slots:
  void clear();
  void setCurrentColumn(int col);
  void sortByColumn(int col, Qt::SortOrder order);
  void updateCount();

protected slots:
  void popup();
  void updateFilterRegExp();
  void updateFilterColumn();

  // - Implementaions
private:
  void createLayout();

private:
  QAbstractItemModel *sourceModel_;
  QSortFilterProxyModel *proxyModel_;
  QTreeView *proxyView_;

  QComboBox *filterPatternEdit_,
            *filterSyntaxComboBox_,
            *filterColumnComboBox_;

  QToolButton *countButton_;
};

#endif // FILTEREDTABLEVIEW_H
