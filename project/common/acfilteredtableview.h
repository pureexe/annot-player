#ifndef FILTEREDTABLEVIEW_H
#define FILTEREDTABLEVIEW_H

// filteredtableview.h
// 11/17/2011

#include <QtGui/QWidget>
#include <QtCore/QHash>
#include <QtCore/QModelIndex>

QT_BEGIN_NAMESPACE
class QComboBox;
class QMenu;
class QStandardItemModel;
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
  AcFilteredTableView(QStandardItemModel *sourceModel, QSortFilterProxyModel *proxyModel, QWidget *parent = nullptr);

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
  void updateCount();

protected slots:
  void popup();
  void updateFilterRegExp();
  void updateFilterColumn();

  // - Implementaions
private:
  void createLayout();

private:
  QStandardItemModel *sourceModel_;
  QSortFilterProxyModel *proxyModel_;
  QTreeView *proxyView_;

  QComboBox *filterPatternEdit_,
            *filterSyntaxComboBox_,
            *filterColumnComboBox_;

  QToolButton *countButton_;
};

#endif // FILTEREDTABLEVIEW_H
