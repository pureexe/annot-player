#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

// dialogdialog.h
// 2/17/2012

#include "module/qtext/dialog.h"

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QAction;
class QComboBox;
class QStandardItemModel;
class QSortFilterProxyModel;
class QTimer;
class QToolButton;
QT_END_NAMESPACE

class FilteredListView;
class DownloadTaskDialog;

class MrlResolver;
class DownloadManager;
class DownloadTask;

typedef QtExt::Dialog DownloadDialogBase;
class DownloadDialog : public DownloadDialogBase
{
  Q_OBJECT
  typedef DownloadDialog Self;
  typedef DownloadDialogBase Base;

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
  explicit DownloadDialog(QWidget *parent = 0);
  bool isAddingUrls() const;

signals:
  void downloadFinished(const QString &path, const QString &url);
  void openFileRequested(const QString &path);

public slots:
  void stopAll();
  void clear();
  //void removeCurrent();

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

protected:
  QString downloadStateToString(int state) const;
  QString downloadSizeToString(qint64 size) const;
  QString downloadSpeedToString(qreal speed) const;
  QString downloadTimeToString(qint64 msec) const;

  DownloadTaskDialog *downloadTaskDialog();

private:
  void createModels();
  void createLayout();
  void createActions();
private:
  DownloadManager *downloadManager_;

  DownloadTaskDialog *downloadTaskDialog_;
  FilteredListView *tableView_;
  QStandardItemModel *sourceModel_;
  QSortFilterProxyModel *proxyModel_;

  QToolButton *startButton_,
              *stopButton_,
              *removeButton_,
              *addButton_,
              *openButton_,
              *openDirectoryButton_;

  QTimer *refreshTimer_;
};

#endif // DOWNLOADDIALOG_H
