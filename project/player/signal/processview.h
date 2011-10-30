#ifndef PROCESSVIEW_H
#define PROCESSVIEW_H

// processview.h
// 10/16/2011

#include "processinfo.h"
#include "core/gui/dialog.h"
#include <QModelIndex>
#include <QHash>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QAction;
class QComboBox;
class QLabel;
class QLineEdit;
class QMenu;
class QStandardItemModel;
class QSortFilterProxyModel;
class QTreeView;
class QToolButton;
QT_END_NAMESPACE

typedef Core::Gui::Dialog ProcessViewBase;
class ProcessView : public ProcessViewBase
{
  Q_OBJECT
  typedef ProcessView Self;
  typedef ProcessViewBase Base;

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
  explicit ProcessView(QWidget *parent = 0);

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
  virtual void setVisible(bool t); ///< \override


protected:
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override

protected slots:
  virtual void dragEnterEvent(QDragEnterEvent *event); ///< \override
  virtual void dragMoveEvent(QDragMoveEvent *event); ///< \override
  virtual void dragLeaveEvent(QDragLeaveEvent *event); ///< \override
  virtual void dropEvent(QDropEvent *event); ///< \override

signals:
  void dragEnterEventReceived(QDragEnterEvent *event);
  void dragMoveEventReceived(QDragMoveEvent *event);
  void dragLeaveEventReceived(QDragLeaveEvent *event);
  void dropEventReceived(QDropEvent *event);

  // - Slots -
private slots:
  void invalidateFilterRegExp();
  void invalidateFilterColumn();

  void invalidateSourceModel(bool showAll = false);

  void attachProcess();
  void detachProcess();

  void invalidateButtons();

  // - Implementaions
protected:
  static void setProcessHeaderData(QAbstractItemModel *model);

private:
  void createLayout();
  void createActions();

private:
  QHash<ulong, ProcessInfo> pis_;

  QStandardItemModel *sourceModel_;
  QSortFilterProxyModel *proxyModel_;
  QTreeView *proxyView_;

  QLineEdit *filterPatternLineEdit_;
  QComboBox *filterSyntaxComboBox_;
  QComboBox *filterColumnComboBox_;

  QToolButton *attachButton_, *detachButton_;

  QMenu *contextMenu_;
  QAction *attachAct_, *detachAct_, *refreshAct_;
};

#endif // PROCESSVIEW_H
