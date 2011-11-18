#ifndef FILTEREDLISTVIEW_H
#define FILTEREDLISTVIEW_H

// filteredlistview.h
// 11/17/2011

#include "processinfo.h"
#include <QWidget>
#include <QModelIndex>
#include <QHash>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
//class QComboBox;
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
  explicit FilteredListView(QStandardItemModel *sourceModel, QWidget *parent = 0);

signals:
  void currentIndexChanged(QModelIndex index);

  // - Properties -
public:
  ///  Never return null
  QSortFilterProxyModel *proxyModel() const       { return proxyModel_; }

  QLineEdit *lineEdit() const                     { return filterPatternLineEdit_; }

  QModelIndex currentIndex() const;
  void removeCurrentRow();

  // - Slots -
public slots:
  void clear();
private slots:
  void invalidateFilterRegExp();
  //void invalidateFilterColumn();

  // - Implementaions
private:
  void createLayout();

private:
  QStandardItemModel *sourceModel_;
  QSortFilterProxyModel *proxyModel_;
  QTreeView *proxyView_;

  QLineEdit *filterPatternLineEdit_;
  //QComboBox *filterSyntaxComboBox_;
  //QComboBox *filterColumnComboBox_;
};

#endif // FILTEREDLISTVIEW_H
