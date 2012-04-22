#ifndef _AC_FILTEREDLISTVIEW_H
#define _AC_FILTEREDLISTVIEW_H

// ac/acfilteredlistview.h
// 11/17/2011

#include <QtGui/QWidget>
#include <QtCore/QModelIndex>
#include <QtCore/QHash>

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
class AcFilteredListView : public QWidget
{
  Q_OBJECT
  typedef AcFilteredListView Self;
  typedef QWidget Base;

public:
  AcFilteredListView(QStandardItemModel *sourceModel, QSortFilterProxyModel *proxyModel, QWidget *parent = 0);

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
  void sortByColumn(int col);
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

#endif // _AC_FILTEREDLISTVIEW_H
