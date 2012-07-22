#ifndef PROCESSVIEW_H
#define PROCESSVIEW_H

// processview.h
// 10/16/2011

#include "processinfo.h"
#include "module/qtext/dialog.h"
#include <QtCore/QModelIndex>
#include <QtCore/QHash>

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QStandardItemModel;
class QSortFilterProxyModel;
class QToolButton;
QT_END_NAMESPACE

class AcFilteredTableView;

class ProcessView : public QtExt::Dialog
{
  Q_OBJECT
  Q_DISABLE_COPY(ProcessView)
  typedef ProcessView Self;
  typedef QtExt::Dialog Base;

protected:
  enum ProcessHeaderData {
    HD_Name = 0, // file base name
    HD_Pid,
    HD_Status,   // attached, etc; hidden column
    HD_Folder,   // parent folder
    HD_Path,     // full path
    HD_Count
  };

  //enum ProcessDataRole {
  //  DR_Pid = Qt::UserRole + 1,
  //  DR_Count
  //};

public:
  explicit ProcessView(QWidget *parent = nullptr);

signals:
  void attached(ProcessInfo pi);
  void detached(ProcessInfo pi);

  void message(QString msg);
  void warning(QString msg);
  void errorMessage(QString msg);
  void notification(QString msg);

  // - Properties -
public:
  ProcessInfo attachedProcessInfo() const;

protected:
  QModelIndex currentIndex() const;
  ulong currentPid() const;
  ulong attachedPid() const;

  // - Slots -
public slots:
  void refresh();
  void clear();

protected slots:
  void setCurrentItemAttached(bool attached);

  // - Events -
public:
  void setVisible(bool t) override;

protected:
  void contextMenuEvent(QContextMenuEvent *event) override;

//protected slots:
//  void dragEnterEvent(QDragEnterEvent *event) override;
//  void dragMoveEvent(QDragMoveEvent *event) override;
//  void dragLeaveEvent(QDragLeaveEvent *event) override;
//  void dropEvent(QDropEvent *event) override;
//
//signals:
//  void dragEnterEventReceived(QDragEnterEvent *event);
//  void dragMoveEventReceived(QDragMoveEvent *event);
//  void dragLeaveEventReceived(QDragLeaveEvent *event);
//  void dropEventReceived(QDropEvent *event);

  // - Slots -
private slots:
  //void invalidateFilterRegExp();
  //void invalidateFilterColumn();

  void invalidateSourceModel(bool showAll = false);

  void attachProcess();
  void detachProcess();

  void invalidateButtons();

  // - Implementaions
protected:
  static void setProcessHeaderData(QAbstractItemModel *model);

private:
  void createModel();
  void createLayout();
  void createContextMenu();

private:
  AcFilteredTableView *tableView_; // process table

  QHash<ulong, ProcessInfo> pis_;

  QStandardItemModel *sourceModel_;
  QSortFilterProxyModel *proxyModel_;

  QToolButton *attachButton_, *detachButton_;

  QMenu *contextMenu_;
  QAction *attachAct_, *detachAct_, *refreshAct_;
};

#endif // PROCESSVIEW_H
