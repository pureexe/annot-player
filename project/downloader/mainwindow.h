#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// mainwindow.h
// 2/17/2012

#include "project/common/acmainwindow.h"

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QAction;
class QComboBox;
class QMenu;
class QSortFilterProxyModel;
class QStandardItemModel;
class QTimer;
class QToolButton;
QT_END_NAMESPACE

class AcFilteredListView;
class AcDownloaderServer;
class TaskDialog;

struct DownloadTaskInfo;
class MrlResolver;
class DownloadManager;
class DownloadTask;
class ClipboardMonitor;
class Signer;
class AcPlayer;
class TrayIcon;

class MainWindow : public AcMainWindow
{
  Q_OBJECT
  typedef MainWindow Self;
  typedef AcMainWindow Base;

protected:
  enum HeaderData {
    HD_Name = 0,
    HD_State,
    HD_Size,
    HD_Speed,
    HD_Percentage,
    HD_RemainingTime,
    HD_Path,
    HD_Url,
    HD_Id,
    HD_Count
  };

  // - Construction -
public:
  explicit MainWindow(QWidget *parent = 0);
  bool isAddingUrls() const;

signals:
  void downloadFinished(const QString &path, const QString &url);

public slots:
  void clear();
  //void removeCurrent();

  void checkClipboard();

  void addUrls(const QStringList &urls, bool batch = false);
  void addUrl(const QString &url);
  void addTask(DownloadTask *t);
  void addTask(const DownloadTaskInfo &t);

  void promptUrl(const QString &text);
  void promptUrls(const QStringList &urls);

  virtual void setVisible(bool visible); ///< \override
  void openDirectory();

  // - Implementations -
protected:
  static void setHeaderData(QAbstractItemModel *model);
  int currentId() const;
  DownloadTask *currentTask() const;

protected slots:
  void about();
  void add();
  void start();
  void restart();
  void startAll();
  void stopAll();
  void removeAll();
  void stop();
  void remove();
  void open();

  void refresh();
  void updateButtons();
  void updateActions();

  void finish(DownloadTask *task);

protected:
  QString downloadStateToString(int state) const;
  QString downloadSizeToString(qint64 size) const;
  QString downloadSpeedToString(qreal speed) const;
  QString downloadTimeToString(qint64 msec) const;

  TaskDialog *taskDialog();

  // - Events -
public:
  virtual bool event(QEvent *event); ///< \override
protected:
  virtual void contextMenuEvent(QContextMenuEvent *e); ///< \override
  virtual void closeEvent(QCloseEvent *e); ///< \override

private:
  void createModels();
  void createLayout();
  void createActions();
private:
  AcDownloaderServer *appServer_;
  AcPlayer *playerDelegate_;
  TrayIcon *trayIcon_;
  DownloadManager *downloadManager_;
  ClipboardMonitor *clipboardMonitor_;
  Signer *signer_;

  TaskDialog *taskDialog_;
  AcFilteredListView *tableView_;
  QStandardItemModel *sourceModel_;
  QSortFilterProxyModel *proxyModel_;

  QToolButton *startButton_,
              *stopButton_,
              *removeButton_,
              *addButton_,
              *openButton_,
              *openDirectoryButton_;

  QTimer *refreshTimer_;

  QMenu *contextMenu_;
  QAction *newAct_,
          *startAct_,
          *stopAct_,
          *restartAct_,
          *startAllAct_,
          *stopAllAct_,
          *removeAllAct_,
          *removeAct_,
          *openAct_,
          *openDirectoryAct_;

  QAction *quitAct_,
          *hideAct_;
};

#endif // MAINWINDOW_H
