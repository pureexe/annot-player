#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

// dialogdialog.h
// 2/17/2012

#include "module/qtext/draggablemainwindow.h"

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QAction;
class QComboBox;
class QStandardItemModel;
class QSortFilterProxyModel;
class QTimer;
class QToolButton;
QT_END_NAMESPACE

class AcFilteredListView;
class TaskDialog;

class MrlResolver;
class DownloadManager;
class DownloadTask;
class ClipboardMonitor;
class Signer;

typedef QtExt::DraggableMainWindow MainWindowBase;
class MainWindow : public MainWindowBase
{
  Q_OBJECT
  typedef MainWindow Self;
  typedef MainWindowBase Base;

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
  void openFileRequested(const QString &path);

public slots:
  void showMessage(const QString &text);

  void stopAll();
  void clear();
  //void removeCurrent();

  void checkClipboard();

  void addUrls(const QStringList &urls, bool batch = false);
  void addUrl(const QString &url);
  void addTask(DownloadTask *t);

  void promptUrl(const QString &text);
  void promptUrls(const QStringList &urls);

  virtual void setVisible(bool visible); ///< \override

  // - Implementations -
protected:
  static void setHeaderData(QAbstractItemModel *model);
  int currentId() const;
  DownloadTask *currentTask() const;

protected slots:
  void add();
  void start();
  void stop();
  void remove();
  void open();
  void openDirectory();

  void refresh();
  void invalidateButtons();

  void finish(DownloadTask *task);

  void error(const QString &text);
  void warn(const QString &text);

protected:
  QString downloadStateToString(int state) const;
  QString downloadSizeToString(qint64 size) const;
  QString downloadSpeedToString(qreal speed) const;
  QString downloadTimeToString(qint64 msec) const;

  TaskDialog *taskDialog();

protected:
  virtual bool event(QEvent *event); ///< \override
  virtual void closeEvent(QCloseEvent *event); ///< \override

private:
  void createModels();
  void createLayout();
  void createActions();
private:
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
              *openDirectoryButton_;

  QTimer *refreshTimer_,
         *hideStatusBarTimer_;
};

#endif // DOWNLOADDIALOG_H
