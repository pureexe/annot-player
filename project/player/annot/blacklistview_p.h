#ifndef BLACKLISTVIEW_P_H
#define BLACKLISTVIEW_P_H

// blacklistview_p.h
// 11/16/2011

#include "module/annotcloud/annotation.h"
#include <QtGui/QWidget>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QAction;
class QLineEdit;
class QStandardItemModel;
class QSortFilterProxyModel;
QT_END_NAMESPACE

class AcFilteredListView;
class AcFilteredTableView;

// Text filter table
class TextFilterView : public QWidget
{
  Q_OBJECT
  typedef TextFilterView Self;
  typedef QWidget Base;

  // - Constructions -
protected:
  enum HeaderData {
    HD_Text = 0, // file base name
    HD_Count
  };

public:
  explicit TextFilterView(QWidget *parent = 0);

  QLineEdit *lineEdit() const;

  QString current() const;

public slots:
  void setRows(const QStringList &l);
  void addRow(const QString &text);
  void clear();

  void removeCurrent();

  // - Implementations -
protected:
  static void setHeaderData(QAbstractItemModel *model);
private:
  AcFilteredListView *tableView_;
  QStandardItemModel *sourceModel_;
  QSortFilterProxyModel *proxyModel_;
};

// User filter table
class UserFilterView : public QWidget
{
  Q_OBJECT
  typedef UserFilterView Self;
  typedef QWidget Base;

  // - Constructions -
protected:
  enum HeaderData {
    HD_UserAlias = 0, // file base name
    HD_Count
  };

public:
  explicit UserFilterView(QWidget *parent = 0);

  QLineEdit *lineEdit() const;

  QString current() const;

public slots:
  void setRows(const QStringList &l);
  void addRow(const QString &text);
  void clear();

  void removeCurrent();

  // - Implementations -
protected:
  static void setHeaderData(QAbstractItemModel *model);
private:
  AcFilteredListView *tableView_;
  QStandardItemModel *sourceModel_;
  QSortFilterProxyModel *proxyModel_;
};

// Annotation filter table
class AnnotationFilterView : public QWidget
{
  Q_OBJECT
  typedef UserFilterView Self;
  typedef QWidget Base;

  typedef AnnotCloud::Annotation Annotation;
  typedef AnnotCloud::AnnotationList AnnotationList;

  // - Constructions -
  // TODO: code reuse!!!!!! these codes are duplicated with annotationbrowser.
protected:
  enum HeaderData {
    HD_Text = 0,
    HD_Pos,
    HD_UserAlias,
    HD_CreateTime,
    HD_UpdateTime,
    HD_Id,
    HD_Language,
    HD_Status,
    HD_UserId,
    HD_Flags,
    HD_BlessedCount,
    HD_CursedCount,
    HD_BlockedCount,
    HD_Count
  };
public:
  explicit AnnotationFilterView(QWidget *parent = 0);

  qint64 currentAnnotationId() const;

public slots:
  void setRows(const AnnotationList &l);
  void addRow(const Annotation &a);
  void clear();
  void removeCurrent();

  // - Implementations -
protected:
  static void setHeaderData(QAbstractItemModel *model);
private:
  AcFilteredTableView *tableView_;
  QStandardItemModel *sourceModel_;
  QSortFilterProxyModel *proxyModel_;
};

#endif // BLACKLISTVIEW_P_H
