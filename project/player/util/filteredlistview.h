#ifndef FILTEREDLISTVIEW_H
#define FILTEREDLISTVIEW_H

// filteredlistview.h
// 11/17/2011

#include "processinfo.h"
#include <QWidget>
#include <QModelIndex>
#include <QHash>

QT_BEGIN_NAMESPACE
//class QComboBox;
class QComboBox;
class QLineEdit;
class QMenu;
class QStandardItemModel;
class QSortFilterProxyModel;
class QTreeView;
class QToolButton;
QT_END_NAMESPACE

///  This class is incompleted.sourceModel and proxyModel are missing data.
class FilteredListView : public QWidget
{
  Q_OBJECT
  typedef FilteredListView Self;
  typedef QWidget Base;

public:
  FilteredListView(QStandardItemModel *sourceModel, QSortFilterProxyModel *proxyModel, QWidget *parent = 0);

signals:
  void currentIndexChanged(QModelIndex index);

  // - Properties -
public:
  QLineEdit *lineEdit() const;

  QModelIndex currentIndex() const;
  void removeCurrentRow();

  // - Slots -
public slots:
  void clear();
  void invalidateCount();
protected slots:
  void invalidateFilterRegExp();
  void popup();
  //void invalidateFilterColumn();

  // - Implementaions
private:
  void createLayout();

private:
  QStandardItemModel *sourceModel_;
  QSortFilterProxyModel *proxyModel_;
  QTreeView *proxyView_;

  QComboBox *filterPatternEdit_;
  //QComboBox *filterSyntaxComboBox_;
  //QComboBox *filterColumnComboBox_;

  QToolButton *countButton_;
};

#endif // FILTEREDLISTVIEW_H
