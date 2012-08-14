#ifndef FILTEREDLISTVIEW_H
#define FILTEREDLISTVIEW_H

// acfilteredlistview.h
// 11/17/2011

#include <QtCore/QHash>
#include <QtCore/QModelIndex>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QComboBox;
class QLineEdit;
class QMenu;
class QSortFilterProxyModel;
class QToolButton;
class QTreeView;
QT_END_NAMESPACE

///  This class is incompleted.sourceModel and proxyModel are missing data.
class AcFilteredListView : public QWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(AcFilteredListView)
  typedef AcFilteredListView Self;
  typedef QWidget Base;

public:
  AcFilteredListView(QAbstractItemModel *sourceModel, QSortFilterProxyModel *proxyModel, QWidget *parent = nullptr);

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
  void updateCount();
  void sortByColumn(int col);
protected slots:
  void updateFilterRegExp();
  void popup();
  //void updateFilterColumn();

  // - Implementaions
private:
  void createLayout();

private:
  QAbstractItemModel *sourceModel_;
  QSortFilterProxyModel *proxyModel_;
  QTreeView *proxyView_;

  QComboBox *filterPatternEdit_;
  //QComboBox *filterSyntaxComboBox_;
  //QComboBox *filterColumnComboBox_;

  QToolButton *countButton_;
};

#endif // FILTEREDLISTVIEW_H
